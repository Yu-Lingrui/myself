// ***************************************************************
// Copyright (c) 2023 Jittor. All Rights Reserved. 
// Maintainers: 
//     Guowei Yang <471184555@qq.com>
//     Dun Liang <randonlang@gmail.com>. 
// 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
// ***************************************************************
#pragma once
#include "common.h"
#include "var.h"
#include "ops/reduce_op.h"
#include "opt/tuner_manager.h"

namespace jittor {

struct ReduceTuner : Tuner {
    ReduceTuner() : Tuner("reduce") {}
    void run(PassManager* pm, TunerManager* tm);
};

}