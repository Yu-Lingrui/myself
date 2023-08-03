// ***************************************************************
// Copyright (c) 2023 Jittor. All Rights Reserved. 
// Maintainers: Dun Liang <randonlang@gmail.com>. 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
// ***************************************************************
#pragma once
#include "opt/pass/pass.h"

namespace jittor {

struct ExpandEmptyBlockPass : Pass {
    ExpandEmptyBlockPass() : Pass("expand_empty_block") {};
    void run() override;
};

} // jittor
