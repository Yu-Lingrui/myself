// ***************************************************************
// Copyright (c) 2023 Jittor. All Rights Reserved. 
// Maintainers: Dun Liang <randonlang@gmail.com>. 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
// ***************************************************************
#pragma once
#ifdef HAS_CUDA
#include "mem/allocator.h"

namespace jittor {

struct CudaManagedAllocator : Allocator {
    uint64 flags() const override { return _cuda; }
    const char* name() const override;
    void* alloc(size_t size, size_t& allocation) override;
    void free(void* mem_ptr, size_t size, const size_t& allocation) override;
};

EXTERN_LIB CudaManagedAllocator cuda_managed_allocator;
DECLARE_FLAG(int, use_cuda_managed_allocator);

}

#endif
