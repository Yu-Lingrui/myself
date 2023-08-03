// ***************************************************************
// Copyright (c) 2023 Jittor. All Rights Reserved. 
// Maintainers: 
//     Xiangli Li <1905692338@qq.com>
//     Dun Liang <randonlang@gmail.com>. 
// 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
// ***************************************************************
#pragma once
#include "op.h"


namespace jittor {

struct CubWhereOp : Op {
    Var* cond;
    unique_ptr<Var*[]> outs;
     /**
    Where Operator generate index of true condition.

    * [in] cond:    condition for index generation

    * [in] dtype:   type of return indexes
    
    * [out] out:  return an array of indexes, same length with number of dims of cond 
    
    Example::

        jt.where([[0,0,1],[1,0,0]])
        # return ( [0,2], [1,0] )
     */
    // @attrs(multiple_outputs)

    CubWhereOp(Var* cond, NanoString dtype=ns_int32);
    void infer_shape() override;    
    const char* name() const override { return "cub_where"; }
    DECLARE_jit_run;
};

} // jittor