// ***************************************************************
// Copyright (c) 2023 Jittor. All Rights Reserved. 
// Maintainers: Dun Liang <randonlang@gmail.com>. 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
// ***************************************************************
#pragma once
#include "common.h"
#include "opt/tuner_manager.h"

namespace jittor {

struct ReorderTuner : Tuner {
    ReorderTuner() : Tuner("reorder") {}
    void run(PassManager* pm, TunerManager* tm);
};

}