// ***************************************************************
// Copyright (c) 2023 Jittor. All Rights Reserved. 
// Maintainers: Dun Liang <randonlang@gmail.com>. 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
// ***************************************************************
#pragma once
#include <functional>
#include "op.h"
#include "var.h"
#include "mem/allocator.h"
#include "ops/array_op.h"

namespace jittor {

struct FetchResult;

struct FetchFunc {
    typedef FetchResult R;
    std::function<void(R*)> callback;
    std::function<void()> deleter;
    FetchFunc() = default;
    FetchFunc(FetchFunc&& other) : callback(other.callback), deleter(other.deleter) {
        other.callback = nullptr;
        other.deleter = nullptr;
    };
    FetchFunc(const FetchFunc&) = delete;
    FetchFunc(std::function<void(R*)>&& callback) : callback(move(callback)) {}
    FetchFunc(std::function<void(R*)>&& callback, std::function<void()>&& deleter)
    : callback(move(callback)), deleter(move(deleter)) {};
    ~FetchFunc() {
        if (deleter) {
            deleter();
        }
    }
    void operator =(FetchFunc&& other) { this->~FetchFunc(); new (this) FetchFunc(move(other)); }
};


struct SimpleFunc {
    std::function<void(int64)> callback;
    std::function<void()> deleter;
    SimpleFunc() = default;
    SimpleFunc(SimpleFunc&& other) : callback(other.callback), deleter(other.deleter) {
        other.callback = nullptr;
        other.deleter = nullptr;
    };
    SimpleFunc(const SimpleFunc&) = delete;
    SimpleFunc(std::function<void(int64)>&& callback) : callback(move(callback)) {}
    SimpleFunc(std::function<void(int64)>&& callback, std::function<void()>&& deleter)
    : callback(move(callback)), deleter(move(deleter)) {};
    ~SimpleFunc() {
        if (deleter) {
            deleter();
        }
    }
    void operator =(SimpleFunc&& other) { this->~SimpleFunc(); new (this) SimpleFunc(move(other)); }
};

struct FetchResult {
    FetchFunc func;
    vector<Allocation> allocations;
    vector<ArrayArgs> arrays;

    inline void call() { func.callback(this); }
};

struct FetchOp final : Op {
    vector<Var*> fetch_vars;
    FetchFunc func;
    list<VarPtr>::iterator fetcher_iter;

    FetchOp(vector<Var*>&& inputs, FetchFunc&& func);

    const char* name() const override { return "fetch"; }
    void run() override;
};

} // jittor