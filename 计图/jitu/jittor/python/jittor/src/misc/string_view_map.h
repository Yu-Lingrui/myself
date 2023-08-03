// ***************************************************************
// Copyright (c) 2023 Jittor. All Rights Reserved. 
// Maintainers: Dun Liang <randonlang@gmail.com>. 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
// ***************************************************************
#pragma once

#if defined(__clang__)
#include <string_view>
#elif defined(__GNUC__)
#include <experimental/string_view>
#endif

#include "common.h"

namespace jittor {

#if __cplusplus < 201400L || defined(IS_ACL)
using string_view = string;
#elif defined(__clang__)
using std::string_view;
#elif defined(__GNUC__)
using std::experimental::string_view;
#else
using std::string_view;
#endif

template<class T>
struct string_view_map {
    typedef typename std::unordered_map<string_view, T> umap_t;
    typedef typename umap_t::iterator iter_t;
    umap_t umap;
    vector<string> holder;

    iter_t find(string_view sv) {
        return umap.find(sv);
    }

    iter_t begin() { return umap.begin(); }
    iter_t end() { return umap.end(); }

    const T& at(string_view sv) { return umap.at(sv); }
    size_t size() { return umap.size(); }

    T& operator[](string_view sv) {
        auto iter = find(sv);
        if (iter != end()) return iter->second;
        holder.emplace_back(sv);
        string_view nsv = holder.back();
        return umap[nsv];
    }
};


} // jittor
