// ***************************************************************
// Copyright (c) 2023 Jittor. All Rights Reserved. 
// Maintainers: Dun Liang <randonlang@gmail.com>. 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
// ***************************************************************
#include "fused_op.h"
#include "var.h"
#include "op_compiler.h"
#include "profiler/profiler.h"
#include "misc/fast_shared_ptr.h"
#include "misc/cuda_flags.h"

namespace jittor {

#ifndef JIT

string_view_map<FusedOpContext*> jit_fused_ops;

std::ostream& operator<<(std::ostream& os, const VarInfo& vi) {
    return os << vi.var << " type:" << vi.type;
}

int FusedOp::get_loop_option(const string& key, const int& _default) {
    auto iter = loop_options->find(key);
    return iter == loop_options->end() ? _default : iter->second;
}

loop_options_t& FusedOp::get_loop_options_tuned() {
    loop_options_tuned = *loop_options_origin;
    loop_options = &loop_options_tuned;
    return loop_options_tuned;
}

void FusedOp::update_jit_key() {
    JK& jk = get_jk();
    jk.clear();
    do_jit_prepare(jk);
}

void FusedOp::update_ops() {
    loop_options_merged.clear();
    loop_options_tuned.clear();
    loop_options = loop_options_origin = nullptr;

    _inputs.clear();
    _outputs.clear();
    vars.clear();
    for (Op* op : ops) {
        for (Var* o : op->outputs()) {
            if (o->loop_options) {
                if (loop_options_origin == nullptr)
                    loop_options_origin = &o->loop_options.data();
                else if (loop_options_origin != &o->loop_options.data()) {
                    // merge loop options
                    for (auto& kv : o->loop_options.data())
                        loop_options_merged[kv.first] = kv.second;
                }
            }
            // bit0 represents can fuse or not
            if (o->custom_data&1)
                // this var can not fuse
                _outputs.emplace_back((Node*)o, 0);
        }
    }

    if (loop_options_origin) {
        if (loop_options_merged.size()) {
            // merge loop_options_origin into loop_options_merged
            for (auto& kv : *loop_options_origin)
                loop_options_merged.emplace(kv);
        }
    } else {
        loop_options_origin = &loop_options_merged;
    }
    loop_options = loop_options_origin;

    ASSERT(outputs().size());
    LOGvvvv << "set fused output" << outputs();
    
    // var.custom_data
    // meaning of custom_data&1(input): 1: cannot fuse, 0 can fuse
    // meaning of custom_data&2: visited or not
    // meaning of custom_data>>2: index of vars

    // op.custom_data: opid
    for (uint i=0; i<ops.size(); i++) {
        auto opi = ops[i];
        opi->custom_data = i;
        for (Var* i : opi->inputs()) {
            i->custom_data &= 1;
        }
        for (Var* o : opi->outputs()) {
            o->custom_data &= 1;
        }
    }
    for (Op* opi : ops) {
        for (Var* i : opi->inputs()) {
            auto &c = i->custom_data;
            // if not visited
            if (!(c&2)) {
                c += 2 + vars.size()*4;
                vars.push_back({i, 0});
                _inputs.emplace_back((Node*)i);
            }
        }
        for (Var* o : opi->outputs()) {
            auto &c = o->custom_data;
            // if not visited
            if (!(c&2)) {
                c += 2 + vars.size()*4;
                // intermediate(can fuse) or output
                vars.push_back({o, int((c&1)+1)});
            }
        }
    }
    LOGvvvv << "Var info" << vars;
}


FusedOp::FusedOp() {
    Op::number_of_lived_ops--;
}

FusedOp::FusedOp(const FusedOp& other) {
    Op::number_of_lived_ops--;
    ops = other.ops;
    edges = other.edges;
    vars = other.vars;
    loop_options_merged = other.loop_options_merged;
    loop_options_tuned = other.loop_options_tuned;
    loop_options = other.loop_options;
    loop_options_origin = other.loop_options_origin;
    context = other.context;
}

FusedOp::~FusedOp() {
    _inputs.clear();
    _outputs.clear();
    Op::number_of_lived_ops++;
}

void FusedOp::infer_shape() {
    for (Op* op : ops) {
        op->init();
    }
}

void FusedOp::statistics(uint64_t& in, uint64_t& out, uint64_t& compute) {
    in = out = compute = 0;
    for (auto& vi : vars) {
        compute = std::max(compute, (uint64_t)vi.var->num);
        if (vi.type == 0) in += vi.var->size;
        if (vi.type == 2) out += vi.var->size;
    }
}

void FusedOp::do_jit_prepare(JK& jk) {
    jk.clear();
    for (uint i=0; i<ops.size(); i++) {
        Op* op = ops[i];
        jk << "«opkey" << i << JK::val;
        jk << op->name();
        op->jit_prepare(jk);
    }
    jk << "«JIT:1";
    if (!use_cuda) {
        // only cpu
        jk << "«JIT_cpu:1";
        this->flags.set(NodeFlags::_cuda, 0);
        this->flags.set(NodeFlags::_cpu, 1);
    } else {
        jk << "«JIT_cuda:1";
        this->flags.set(NodeFlags::_cpu, 0);
        this->flags.set(NodeFlags::_cuda, 1);
    }
    jk << "«graph:";
    for (auto& t : edges) {
        uint i,j,k,l;
        std::tie(i,j,k,l) = t;
        jk << JK::hex2(i) << JK::hex1(j) << JK::hex2(k) << JK::hex1(l) << ',';
    }
    jk << "«var_info:" << JK::val;
    bool use_int64_t = false;
    for (auto& vi : vars) {
        jk << JK::hex1(vi.type) << JK::hex1(vi.var->shape.size());
        if (vi.type != 1 && vi.var->num >= std::numeric_limits<int32_t>::max())
            use_int64_t = true;
    }
    if (use_int64_t)
        jk << "«index_t:int64";
    else
        jk << "«index_t:int32";
    if (loop_options->size()) {
        if (get_loop_option("compile_shapes")) {
            jk << "«shapes:";
            for (auto& vi : vars) {
                jk << '[';
                for (auto a : vi.var->shape)
                    jk << a << ',';
                jk << "],";
            }
        }
        jk << "«choices:";
        for (auto& kv : *loop_options) {
            if (kv.first.size() && kv.first[0] != '_')
                jk << kv.first << ':' << kv.second << ',';
        }
    }
    jk.finilize();
}

void FusedOp::do_prepare(JK& jk) {
    do_jit_prepare(jk);
}

void FusedOp::do_run_after_prepare(JK& jk) {
    const char* jit_key = jk.to_cstring();
    auto iter = jit_fused_ops.find(string_view(jit_key, jk.size));
    if (iter != jit_fused_ops.end()) {
        LOGvvv <<  "Jit fused op key found:" << jit_key << "jit op entry:" << (void*)iter->second;
        context = iter->second;
        iter->second->vrm.fop = this;
        Profiler::record_and_run(iter->second->entry, this, jit_key);
        return;
    }
    LOGvv << "Jit op key not found:" << jit_key;
    // compile JIT op
    context = new FusedOpContext();
    context->setup(this);
    string prev_jit_key = jit_key;
    context->entry = OpCompiler::do_compile(this);
    string new_jit_key = get_jit_key(jk);
    jit_fused_ops[new_jit_key] = jit_fused_ops[prev_jit_key] = context;
    jit_key_mapper[prev_jit_key] = new_jit_key;
    LOGvv << "Get jit op entry:" << (void*)(context->entry);
    Profiler::record_and_run(context->entry, this, new_jit_key.c_str());
}

void FusedOpContext::setup(FusedOp* fop) {
    node_id.clear();
    vrm.fop = fop;
    for (int i=0; i<fop->ops.size(); i++)
        node_id[fop->ops[i]] = i;
    for (int i=0; i<fop->vars.size(); i++)
        node_id[fop->vars[i].var] = i;
}

int FusedOp::get_node_id(Node* node) {
    ASSERT(context);
    return context->node_id.at(node);
}

int FusedOp::has(Node* node) {
    ASSERT(context);
    return context->node_id.count(node);
}

void FusedOp::do_run() {
    JK& jk = get_jk();
    do_prepare(jk);
    do_run_after_prepare(jk);
}

#else // JIT
void FusedOp::jit_run() {
    for (uint i=0; i<ops.size(); i++) {
        LOGvvvv << "fuse run:" << ops[i] << ops[i]->inputs() << ops[i]->outputs();
        ops[i]->do_run();
    }
}
#endif // JIT

}
