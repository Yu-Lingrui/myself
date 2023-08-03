// ***************************************************************
// Copyright (c) 2023 Jittor. All Rights Reserved.  
//     Guoye Yang <498731903@qq.com>. 
//     Dun Liang <randonlang@gmail.com>. 
// All Rights Reserved.
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
// ***************************************************************
#include "var.h"
#include "nccl_broadcast_op.h"
#include "utils/str_utils.h"

#include <nccl.h>
#include <cuda_runtime.h>
#include "helper_cuda.h"
#include "nccl_wrapper.h"
#include "ops/op_register.h"
namespace jittor {

#ifndef JIT
NcclBroadcastOp::NcclBroadcastOp(Var* x, int root) : x(x), root(root) {
    flags.set(NodeFlags::_cpu, 0);
    flags.set(NodeFlags::_cuda, 1);
    y = create_output(nullptr, x->dtype());
}

void NcclBroadcastOp::infer_shape() {
    y->set_shape(x->shape);
}

VarPtr NcclBroadcastOp::grad(Var* out, Var* dout, Var* v, int v_index) {
    static auto nccl_reduce = 
        get_op_info("nccl_reduce").get_constructor<VarPtr, Var*, int>();
    return nccl_reduce(dout,root);
}

void NcclBroadcastOp::jit_prepare(JK& jk) {
    jk << "«Tx:" << x->dtype();
}

#else // JIT
#ifdef JIT_cuda

void NcclBroadcastOp::jit_run() {
    @define(T_NCCL,
        @if(@strcmp(@Tx,float)==0 || @strcmp(@Tx,float32)==0, ncclFloat)
        @if(@strcmp(@Tx,int)==0 || @strcmp(@Tx,int32)==0, ncclInt)
        @if(@strcmp(@Tx,float64)==0, ncclFloat64)
        @if(@strcmp(@Tx,int64)==0, ncclInt64)
        @if(@strcmp(@Tx,uint8)==0, ncclUint8)
    )
    auto* __restrict__ xp = x->ptr<Tx>();
    auto* __restrict__ yp = y->ptr<Tx>();
    checkCudaErrors(ncclBroadcast(xp, yp, y->num, @T_NCCL, root, comm, 0));
}

#endif
#endif // JIT

} // jittor
