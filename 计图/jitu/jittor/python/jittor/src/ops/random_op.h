// ***************************************************************
// Copyright (c) 2023 Jittor. All Rights Reserved. 
// Maintainers: Dun Liang <randonlang@gmail.com>. 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
// ***************************************************************
#pragma once
#include "op.h"

namespace jittor {

struct RandomOp : Op {
    Var* output;
    NanoString type;
    RandomOp(NanoVector shape, NanoString dtype=ns_float32, NanoString type=ns_uniform);
    
    const char* name() const override { return "random"; }
    DECLARE_jit_run;
};

} // jittor