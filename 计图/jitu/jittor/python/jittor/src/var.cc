// ***************************************************************
// Copyright (c) 2023 Jittor. All Rights Reserved. 
// Maintainers: Dun Liang <randonlang@gmail.com>. 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
// ***************************************************************
#include <type_traits>

#include "var.h"
#include "op.h"
#include "mem/allocator.h"
#include "pybind/py_var_tracer.h"
#include "mem/swap.h"

namespace jittor {

int64 Var::number_of_lived_vars = 0;

DEFINE_FLAG(fast_shared_ptr<loop_options_t>, compile_options, {}, 
    "Override the default loop transfrom options");
DEFINE_FLAG(bool, no_grad, 0, 
    "No grad for all jittor Var creation");
DEFINE_FLAG(bool, no_fuse, 0, 
    "No fusion optimization for all jittor Var creation");
DEFINE_FLAG(uint8, node_order, 0, "id prior");
DEFINE_FLAG(uint8, th_mode, 0, "th mode");
// TODO: fuse multiple flags
DEFINE_FLAG(int, amp_reg, 0, "Auto mixed-precision control registers, bit 0: prefer 32; bit 1: prefer 16; bit 2: keep reduce type; bit 3 keep white list type; bit 4: array like op prefer too; bit 5, reduce16 intermediate not use 32");

DEFINE_FLAG_WITH_SETTER(int, auto_mixed_precision_level, 0, "Auto mixed-precision optimization level, 0: not use fp16, 1-3: preserve level, not use fp16 for now; 4: perfer fp16, but some ops use fp32 e.g. sum,exp; 5: simular with 4, and array op will automatically convert to fp16; 6: all ops prefer fp16");

void (*_var_free_hook)(Var*);

void free_var(Var* v) {
    if (PREDICT_BRANCH_NOT_TAKEN((bool)_var_free_hook)) _var_free_hook(v);
    Var::number_of_lived_vars--;
    if (save_mem)
        free_with_swap(v);
    else
    if (v->mem_ptr != nullptr) {
        auto mem_ptr = v->mem_ptr;
        auto allocation = v->allocation;
        auto allocator = v->allocator;
        v->mem_ptr = nullptr;
        v->allocator = nullptr;
        v->allocation = 0;
        allocator->free(mem_ptr, v->size, allocation);
    }
}

void free_var_mem(Var* v) {
    if (save_mem)
        free_with_swap(v);
    else {
        auto mem_ptr = v->mem_ptr;
        auto allocation = v->allocation;
        auto allocator = v->allocator;
        v->mem_ptr = nullptr;
        v->allocator = nullptr;
        v->allocation = 0;
        allocator->free(mem_ptr, v->size, allocation);
    }
}

void setter_auto_mixed_precision_level(int value) {
    if (value <= 2) amp_reg = 0; else
    if (value == 3) amp_reg = amp_keep_reduce | amp_keep_white; else
    if (value == 4) amp_reg = amp_prefer16; else
    if (value == 5) amp_reg = amp_prefer16 | amp_array_prefer; else
    if (value == 6) amp_reg = amp_prefer16 | amp_array_prefer | amp_keep_reduce | amp_keep_white;
}

Var::Var(NanoVector shape, NanoString dtype)
    : shape(shape), 
      loop_options(compile_options) {
    flags.set(NodeFlags::_var, 1);
    flags.set(NodeFlags::_stop_grad, !dtype.is_float() || no_grad);
    flags.set(NodeFlags::_stop_fuse, no_fuse);
    ns = dtype;
    ASSERT(ns.is_dtype());
    number_of_lived_vars++;
    numel();
    if (PREDICT_BRANCH_NOT_TAKEN(trace_py_var)) trace_data.record_node(this);
}
    
string Var::to_string() {
    string s = dtype().to_cstring();
    s += shape.to_string();
    return s;
}

int64 Var::numel() {
    bool negtive = 0;
    num=1;
    for (auto k : shape) {
        if (k<0) {
            negtive = 1;
            num *= -k;
        } else {
            num *= k;
        }
    }
    size = num * dsize();
    if (negtive) num = -num;
    if (shape.size() == 0) {shape.push_back(1);}
    return num;
}

void Var::set_shape(NanoVector shape) {
    this->shape = shape;
    numel();
}

bool Var::alloc(Allocator* allocator) {
    if (mem_ptr) return true;
    if (auto* x = (Var*)(this->allocator)) {
        if (x->allocator->share_with(size, x->allocation)) {
            mem_ptr = ((char*) x->mem_ptr) + allocation;
            allocation = x->allocation;
            this->allocator = x->allocator;
            return true;
        }
    }
    mem_ptr = allocator->alloc(size, allocation);
    this->allocator = allocator;
    return mem_ptr;
}

VarPtr clone(Var* x);
void VarPtr::set_stop_grad(bool stop_grad) {
    if (stop_grad == ptr->is_stop_grad()) return;
    if (stop_grad)
        ptr->set_stop_grad();
    else {
        bool no_grad_bk = no_grad;
        auto th_mode_bk = th_mode;
        no_grad = 0;
        th_mode = 0;
        *this = clone(ptr);
        no_grad = no_grad_bk;
        th_mode = th_mode_bk;
    }
}

std::ostream& operator<<(std::ostream& os, const Var& var) {
    os << "Var" << '(' << var.id
        << ':' << var.forward_liveness
        << ':' << var.backward_liveness
        << ':' << var.pending_liveness
        << ":i" << var._inputs.size()
        << ":o" << var._outputs.size()
        << ":s" << var.is_finished()
        << ":n" << var.flags.get(NodeFlags::_needed_by_backward)
        << ',' 
        << var.dtype().to_cstring() << ',' << var.name << ',' << std::hex <<(uint64)var.mem_ptr << std::dec
        << ')' << var.shape;
    if (trace_py_var) {
        os << '{';
        print_node_trace(&var, os);
        os << '}';
    }
    return os;
}
std::ostream& operator<<(std::ostream& os, const Var* var) {
    return os << *var;
}
std::ostream& operator<<(std::ostream& os, const VarPtr& v) { return os << v.ptr; }

} // jittor