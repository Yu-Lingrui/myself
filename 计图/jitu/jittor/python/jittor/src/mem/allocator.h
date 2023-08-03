// ***************************************************************
// Copyright (c) 2023 Jittor. All Rights Reserved. 
// Maintainers: Dun Liang <randonlang@gmail.com>. 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
// ***************************************************************
#pragma once
#include "common.h"

namespace jittor {

struct Allocator {
    enum Flag {
        _cuda=1,
        _aligned=2
    };
    int64 used_memory=0, unused_memory=0;
    inline virtual uint64 flags() const { return 0; };
    inline bool is_cuda() const { return flags() & _cuda; }
    inline bool is_aligned() const { return flags() & _aligned; }
    virtual const char* name() const = 0;
    virtual void* alloc(size_t size, size_t& allocation) = 0;
    virtual void free(void* mem_ptr, size_t size, const size_t& allocation) = 0;
    inline virtual void gc() {};
    inline virtual bool share_with(size_t size, size_t allocation) { return false; };
    inline virtual ~Allocator() {}
};

struct AlignedAllocator;
EXTERN_LIB AlignedAllocator aligned_allocator;

struct Allocation {
    void* ptr;
    size_t allocation, size;
    Allocator* allocator;
    inline Allocation() = default;
    inline Allocation(void* ptr, size_t allocation, size_t size, Allocator* allocator)
        : ptr(ptr), allocation(allocation), size(size), allocator(allocator) {}
    inline Allocation(Allocation&& o)
        : ptr(o.ptr), allocation(o.allocation), size(o.size), allocator(o.allocator)
        { o.ptr = nullptr; }
    inline Allocation(unique_ptr<char[]>&& p) 
        { ptr = p.release(); allocator = (Allocator*)&aligned_allocator; }
    inline Allocation(Allocator* at, size_t size)
        : size(size), allocator(at)
        { allocator = at; ptr = at->alloc(size, allocation); }
    inline ~Allocation()
        { if (ptr) allocator->free(ptr, size, allocation); }
};

EXTERN_LIB Allocator* cpu_allocator;
Allocator* get_allocator(bool temp_allocator=false);
// @pyjt(gc)
void gc_all();

void migrate_to_cpu(Var* var, Allocator* allocator);
void migrate_to_gpu(Var* var, Allocator* allocator);

} // jittor