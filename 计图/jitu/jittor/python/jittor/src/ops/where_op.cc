// ***************************************************************
// Copyright (c) 2023 Jittor. All Rights Reserved. 
// Maintainers: Dun Liang <randonlang@gmail.com>. 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
// ***************************************************************
#include "var.h"
#include "ops/where_op.h"
#include "misc/cuda_flags.h"
#include "ops/op_register.h"
#ifdef JIT_cuda
#include "executor.h"
#include <assert.h>
#include <cuda_runtime.h>
#include "helper_cuda.h"
#endif

namespace jittor {

#ifndef JIT
WhereOp::WhereOp(Var* cond, NanoString dtype) : cond(cond) {
    flags.set(NodeFlags::_cpu);
    flags.set(NodeFlags::_cuda);
    flags.set(NodeFlags::_manual_set_vnbb);
    auto ndim = cond->shape.size();
    #ifdef HAS_CUDA
    if (use_cuda) {
        static auto cub_where = has_op("cub_where") ? get_op_info("cub_where")
                .get_constructor<std::vector<VarPtr>, Var*, NanoString>() : nullptr;
        if (cub_where && (ndim>1 || std::abs(cond->num)>4096)) {
            auto var = cub_where(cond, dtype);
            for(uint i=0;i<ndim;i++)
                forward(var[i]);
            return;
        }
    }
    #endif
    outs.reset(new Var*[ndim]);
    for (uint i=0; i<ndim; i++)
        outs[i] = create_output(nullptr, dtype);
}
static auto make_ternary = get_op_info("ternary")
    .get_constructor<VarPtr, Var*, Var*, Var*>();
WhereOp::WhereOp(Var* cond, Var* x, Var* y) {
    forward(make_ternary(cond, x, y));
    return;
}

void WhereOp::infer_shape() {
    auto ndim = cond->shape.size();
    auto num = -cond->num;
    for (uint i=0; i<ndim; i++)
        outs[i]->set_shape({num});
}

void WhereOp::jit_prepare(JK& jk) {
    jk << "«Ti:" << cond->dtype();
    jk << "«To:" << outs[0]->dtype();
    jk << "«NDIM=" << JK::hex1(cond->shape.size());
}

#else // JIT
#ifdef JIT_cuda

__global__ static void where_kernel(
    @for(i, 0, NDIM, 1, index_t condshape@i, )
    Ti* __restrict__ condp,
    @for(i, 0, NDIM, 1, To* __restrict__ outs@i@@p, )
    int* __restrict__ np
) {
    __shared__ uint n;
    int tid = threadIdx.x;
    int tnum = blockDim.x;
    if (tid == 0)
        n = 0;
    // define cond stride
    index_t condstride@{NDIM-1} = 1;
    @for(i, NDIM-2, -1, -1, auto condstride@i = condstride@{i+1} * condshape@{i+1};)
    __syncthreads();

    // generate d-for loop
    @for(d, 0, NDIM-1, for (index_t i@d=0; i@d < condshape@d; i@d++)) 
    for (index_t i@{NDIM-1}=tid; i@{NDIM-1}<condshape@{NDIM-1}; i@{NDIM-1}+=tnum)
    {
        auto condid = @for(d, 0, NDIM, + i@d * condstride@d);
        if (condp[condid]) {
            uint cn = atomicInc(&n, 1u<<30);
            @for(i, 0, NDIM, outs@i@@p[cn] = i@i;)
        }
    }
    __syncthreads();
    if (tid == 0)
        (*np) = n;
}


__device__ inline uint prefix_sum(uint val, uint lane_id) {
    #define FULL_MASK 0xffffffff
    #pragma unroll
    for (int offset = 16; offset > 0; offset /= 2) {
        uint x = __shfl_up_sync(FULL_MASK, val, offset);
        val += lane_id>=offset? x : 0;
    }
    return val;
}

__device__ inline uint bc(uint val, uint lane_id) {
    return __shfl_sync(FULL_MASK, val, lane_id);
}

__global__ static void where_kernel_one_warp(
    @for(i, 0, NDIM, 1, index_t condshape@i, )
    Ti* __restrict__ condp,
    @for(i, 0, NDIM, 1, To* __restrict__ outs@i@@p, )
    int* __restrict__ np
) {
    uint n = 0;
    int tid = threadIdx.x;
    int tnum = 32;
    // define cond stride
    index_t condstride@{NDIM-1} = 1;
    @for(i, NDIM-2, -1, -1, auto condstride@i = condstride@{i+1} * condshape@{i+1};)

    // generate d-for loop
    @for(d, 0, NDIM-1, for (index_t i@d=0; i@d < condshape@d; i@d++)) 
    for (index_t i=0; i<condshape@{NDIM-1}; i+=tnum)
    {
        index_t i@{NDIM-1} = i + tid;
        auto condid = @for(d, 0, NDIM, + i@d * condstride@d);
        uint x = i@{NDIM-1}<condshape@{NDIM-1} ? !!condp[condid] : 0;
        uint prefix_x = prefix_sum(x, tid);
        if (x) {
            uint cn = n + prefix_x - 1;
            @for(i, 0, NDIM, outs@i@@p[cn] = i@i;)
        }
        n += bc(prefix_x, 31);
    }
    if (tid == 0)
        (*np) = n;
}

#define WTN 1024

__global__ static void where_kernel_one_block(
    @for(i, 0, NDIM, 1, index_t condshape@i, )
    Ti* __restrict__ condp,
    @for(i, 0, NDIM, 1, To* __restrict__ outs@i@@p, )
    int* __restrict__ np
) {
    uint n = 0;
    int tid = threadIdx.x;
    int tnum = WTN;
    __shared__ uint s[WTN/32];
    int wid = tid / 32;
    int lid = tid % 32;
    int wnum = WTN / 32;
    // define cond stride
    index_t condstride@{NDIM-1} = 1;
    @for(i, NDIM-2, -1, -1, auto condstride@i = condstride@{i+1} * condshape@{i+1};)

    // generate d-for loop
    @for(d, 0, NDIM-1, for (index_t i@d=0; i@d < condshape@d; i@d++)) 
    for (index_t i=0; i<condshape@{NDIM-1}; i+=tnum)
    {
        index_t i@{NDIM-1} = i + tid;
        auto condid = @for(d, 0, NDIM, + i@d * condstride@d);
        uint x = i@{NDIM-1}<condshape@{NDIM-1} ? !!condp[condid] : 0;
        uint prefix_x = prefix_sum(x, lid);
        uint warp_sum = bc(prefix_x, 31);

        // prefix sum between warps
        if (lid == 0) {
            s[wid] = warp_sum;
        }
        __syncthreads();
        if (wid == 0) {
            s[lid] = prefix_sum(s[lid], lid);
        }
        __syncthreads();
        uint warp_prefix_sum = s[wid];
        uint block_sum = s[wnum-1];
        __syncthreads();

        if (x) {
            uint cn = n + prefix_x - 1 + warp_prefix_sum - warp_sum;
            @for(i, 0, NDIM, outs@i@@p[cn] = i@i;)
        }
        n += block_sum;
    }
    if (tid == 0)
        (*np) = n;
}

void WhereOp::jit_run() {
    auto* __restrict__ condp = cond->ptr<Ti>();
    // define cond shape
    @for(i, 0, NDIM, index_t condshape@i = cond->shape[@i];)
    
    // define outs
    @for(i, 0, NDIM,  auto* __restrict__ outs@i@@p = outs[@i]->ptr<To>();)
    
    size_t n_allocation;
    int* np = (int*)exe.temp_allocator->alloc(4, n_allocation);

    // one block kernel, result maybe unstable
    // int tnum = condshape@{NDIM-1};
    // tnum = std::max(1, std::min(1024, tnum));
    // where_kernel<<<1,tnum>>>(
    //     @for(i, 0, NDIM, 1, condshape@i, )
    //     condp,
    //     @for(i, 0, NDIM, 1, outs@i@@p, )
    //     np
    // );


    int tnum = condshape@{NDIM-1};
    if (tnum < 100) {
        // one warp kernel, result is stable
        where_kernel_one_warp<<<1,32>>>(
            @for(i, 0, NDIM, 1, condshape@i, )
            condp,
            @for(i, 0, NDIM, 1, outs@i@@p, )
            np
        );
    } else {
        // one block kernel, result is stable
        where_kernel_one_block<<<1,WTN>>>(
            @for(i, 0, NDIM, 1, condshape@i, )
            condp,
            @for(i, 0, NDIM, 1, outs@i@@p, )
            np
        );
    }

    int n=0;
    // checkCudaErrors(cudaDeviceSynchronize());
    checkCudaErrors(cudaMemcpy(&n, np, 4, cudaMemcpyDeviceToHost));
    @for(i, 0, NDIM, outs[@i]->set_shape({n});)
    exe.temp_allocator->free(np, 4, n_allocation);
}
#else

void WhereOp::jit_run() {
    auto* __restrict__ condp = cond->ptr<Ti>();
    // define cond shape
    @for(i, 0, NDIM, index_t condshape@i = cond->shape[@i];)
    // define cond stride
    index_t condstride@{NDIM-1} = 1;
    @for(i, NDIM-2, -1, -1, auto condstride@i = condstride@{i+1} * condshape@{i+1};)
    
    // define outs
    @for(i, 0, NDIM,  auto* __restrict__ outs@i@@p = outs[@i]->ptr<To>();)
    int64 n=0;

    // generate d-for loop
    @for(d, 0, NDIM, for (index_t i@d=0; i@d < condshape@d; i@d++)) {
        auto condid = @for(d, 0, NDIM, + i@d * condstride@d);
        if (condp[condid]) {
            @for(i, 0, NDIM, outs@i@@p[n] = i@i;)
            n++;
        }
    }
    @for(i, 0, NDIM, outs[@i]->set_shape({n});)
}

#endif
#endif // JIT

} // jittor
