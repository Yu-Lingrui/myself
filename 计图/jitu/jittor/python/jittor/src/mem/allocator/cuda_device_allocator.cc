// ***************************************************************
// Copyright (c) 2023 Jittor. All Rights Reserved. 
// Maintainers: Dun Liang <randonlang@gmail.com>. 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
// ***************************************************************
#ifdef HAS_CUDA
#include <cuda_runtime.h>
#include "mem/mem_info.h"
#include "helper_cuda.h"
#include "mem/allocator/cuda_device_allocator.h"

namespace jittor {

CudaDeviceAllocator cuda_device_allocator;
EXTERN_LIB bool no_cuda_error_when_free;

const char* CudaDeviceAllocator::name() const {return "cuda_device";}

void* CudaDeviceAllocator::alloc(size_t size, size_t& allocation) {
    if (size==0) return (void*)0x10;
    void* ptr;
    try {
        checkCudaErrors(cudaMalloc(&ptr, size));
        return ptr;
    } catch (...) {
        // clean the last error
        cudaGetLastError();
    }
    display_memory_info(__FILELINE__);
    LOGf << "Unable to alloc cuda device memory for size" << size;
    checkCudaErrors(cudaMallocManaged(&ptr, size));
    return ptr;
}

void CudaDeviceAllocator::free(void* mem_ptr, size_t size, const size_t& allocation) {
    if (size==0) return;
    if (no_cuda_error_when_free) return;
    checkCudaErrors(cudaFree(mem_ptr));
}

} // jittor

#endif
