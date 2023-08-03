// ***************************************************************
// Copyright (c) 2023 Jittor. All Rights Reserved. 
// Maintainers: Dun Liang <randonlang@gmail.com>. 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
// ***************************************************************
#include "var.h"
#include "opt/pass_manager.h"
#include "opt/pass/replace_for_num_pass.h"
#include "opt/pass/loop_var_analyze_pass.h"
#include "opt/pass/remove_loop_pass.h"
#include "opt/pass/rename_loop_index_pass.h"
#include "opt/pass/compile_shapes_pass.h"
#include "opt/pass/split_loop_pass.h"
#include "opt/pass/reorder_loop_pass.h"
#include "opt/pass/merge_loop_pass.h"
#include "opt/pass/merge_loop_var_pass.h"
#include "opt/pass/const_var_pass.h"
#include "opt/pass/expand_empty_block_pass.h"
#include "opt/pass/solve_conflict_define_pass.h"
#include "opt/pass/remove_intermediate_pass.h"
#include "opt/pass/restride_pass.h"
#include "opt/pass/vectorize_pass.h"
#include "opt/pass/unroll_pass.h"
#include "opt/pass/use_movnt_pass.h"
#include "opt/pass/loop_to_func_pass.h"
#include "opt/pass/assume_aligned_pass.h"
#include "opt/pass/parallel_pass.h"
#include "opt/pass/atomic_tuner_pass.h"
#include "opt/pass/shared_reduce_pass.h"
#include "opt/pass/float_atomic_fix_pass.h"
#include "opt/pass/insert_profile_loop_pass.h"
#include "opt/pass/fake_main_pass.h"
#include "opt/pass/check_cache_pass.h"
#include "opt/pass/mark_raw_pass.h"
#include "utils/str_utils.h"

namespace jittor {

DECLARE_FLAG(string, cc_type);
DEFINE_FLAG(string, exclude_pass, "", "Don't run certain pass.");
DEFINE_FLAG(string, log_op_hash, "", "Output compiler pass result of certain hash of op.");


PassManager::PassManager(OpCompiler* oc) : oc(oc), all(oc->get_src()) {
    main_ir = nullptr;
    for (auto& c : all.children)
        if (c->type=="func" && c->attrs["lvalue"]=="jittor::FusedOp::jit_run") {
            main_ir = c.get();
            break;
        }
    ASSERT(main_ir);
}

bool PassManager::check(Pass* pass) {
    if (exclude_pass=="*") return false;
    if (exclude_pass==pass->name) return false;
    if (startswith(exclude_pass, "after:")) {
        auto n = (uint)stoi(exclude_pass.substr(6));
        if (finished_passes.size()>=n)
            return false;
    }
    return true;
}

void PassManager::run_passes() {
    auto& ir = *main_ir;

    LOGvvvv << "KernelIR:\n" << ir.to_string();
    if (oc->op->ops.size() == 1 && oc->op->ops[0]->name() == string("array")) {
        ir.remove_all_unused();
        if (oc->op->flags.get(NodeFlags::_cuda)) {
            ir.children.back()->erase();
            string type = oc->op->ops[0]->outputs().front()->dtype().to_cstring();
            ir.push_back("kernel<<<1,1>>>(op0_outputp, op0_outputv);");
            auto jt_type = type == "bool" ? type : "jittor::" + type;
            ir.push_back("__global__ static void kernel("+jt_type+"* xp, "+jt_type+" x) { xp[0] = x; } ", &ir.before, true);
        }
        return;
    }
    run_pass<MarkRawPass>();
    run_pass<ReplaceForNumPass>();
    run_pass<LoopVarAnalyzePass>();
    run_pass<RemoveLoopPass>();
    run_pass<RenameLoopIndexPass>();
    run_pass<CompileShapesPass>();
    
    run_pass<SplitLoopPass>();
    run_pass<ReorderLoopPass>();
    run_pass<MergeLoopPass>();
    run_pass<ExpandEmptyBlockPass>();
    run_pass<SolveConflictDefinePass>();

    run_pass<RemoveIntermediatePass>();
    
    run_pass<SolveConflictDefinePass>();
    run_pass<MergeLoopVarPass>();
    // tmp disable ConstVarPass
    // run_pass<ConstVarPass>();

    run_pass<RestridePass>();
    
    if (cc_type == "icc") {
        // only icc supports pragma
        run_pass<VectorizePass>();
        run_pass<UnrollPass>();
        run_pass<UnrollPass>();
    }
    run_pass<UseMovntPass>();
    run_pass<CheckCachePass>();
    run_pass<LoopToFuncPass>();
    run_pass<AssumeAlignedPass>();
    run_pass<ParallelPass>();
    run_pass<AtomicTunerPass>();
    run_pass<SharedReducePass>();
    run_pass<FloatAtomicFixPass>();
    
    run_pass<InsertProfileLoopPass>();
    
    run_pass<SolveConflictDefinePass>();
    
    run_pass<FakeMainPass>();
}

} // jittor

