// ***************************************************************
// Copyright (c) 2023 Jittor. All Rights Reserved. 
// Maintainers: Dun Liang <randonlang@gmail.com>. 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
// ***************************************************************
#include "pybind/py_var_tracer.h"
#include "grad.h"
#include "var.h"
#include "op.h"
#include "graph.h"
#include "ops/op_register.h"
#include "var_holder.h"

namespace jittor {

#define PREVENT_LARGE_FUSED_OP 16

DECLARE_FLAG(int, auto_mixed_precision_level);

static auto make_binary = get_op_info("binary")
    .get_constructor<VarPtr, Var*, Var*, NanoString>();
static auto make_unary = get_op_info("unary")
    .get_constructor<VarPtr, Var*, NanoString>();
static auto make_number = get_op_info("number")
    .get_constructor<VarPtr, float, Var*>();

#ifdef _WIN32
template<class T> struct StackIniter {
    T* a;
    int n;
    inline StackIniter(T* a, int n) :a(a), n(n) {
        for (int i=0; i<n; i++)
            new(a+i) T();
    }
    inline ~StackIniter() {
        for (int i=0; i<n; i++)
            a[i].~T();
    }
};

#define STACK_ALLOC2(T, a, n) T* a = (T*)_alloca(sizeof(T)*(n)); StackIniter<T> __init_##a(a, n);
#else
#define STACK_ALLOC2(T, a, n) T a[n]
#endif

struct AmpGradGuard {
    int amp_reg_bk;
    AmpGradGuard(Op* op) {
        amp_reg_bk = amp_reg;
        amp_reg |= (op->flags.flags >> NodeFlags::_prefer_32);
    }

    ~AmpGradGuard() {
        amp_reg = amp_reg_bk;
    }
};

VarPtr make_grad(Op* op, Var* out, Var* dout, Var* x, int x_index) {
    if (dout == nullptr) return nullptr;
    if (x_index<0) return nullptr;
    LOGvvvv << "Make grad op:" >> op->name() << "inputs:" >> op->inputs()
        << "out:" >> out << "dout:" >> dout << "x:" >> x << "xid:" >> x_index;
    AmpGradGuard agg(op);
    auto dx = op->grad(out, dout, x, x_index);
    if (x->loop_options)
        dx->loop_options = x->loop_options;
    return dx;
}

inline static void assign_attrs(Var* a, Var* b) {
    if (b->flags.get(NodeFlags::_stop_fuse))
        a->flags.set(NodeFlags::_stop_fuse);
}

map<string,int> grad_breaks;

void warn_grad_break(int i, Var* v) {
    if (grad_breaks.count(v->name.c_str())) return;
    grad_breaks[v->name.c_str()] = 1;
    LOGw << "grads[">>i>>"] '">> v->name>>"' doesn't have gradient. It will be set to zero:" << v;
}

vector<VarPtr> grad(Var* loss, vector<Var*> targets, bool retain_graph) {
    LOGvv << "loss:" >> loss << "targets:" >> targets;
    CHECK(loss->is_float()) << "Loss should be float";
    for (Var* var : targets)
        CHECK(var->is_float()) << "Targets of grad should be float";
    // successors of targets
    vector<Node*> ts(targets.begin(), targets.end());
    // bfs visit find all successors of targets
    LOGvv << "Size of successors:" << ts.size();
    bfs_forward(ts, [](Node*){ return true; });
    vector<Node*> gnodes;
    gnodes.reserve(ts.size());
    auto nt = tflag_count;
    if (loss->tflag == nt)
        gnodes.push_back(loss);
    bfs_backward(gnodes, [&](Node* node) {
        if (node->tflag != nt)
            return false;
        if (node->is_stop_grad())
            return false;
        return true;
    });
    LOGvv << "Size of grad nodes:" << gnodes.size();
    
    vector<Node*> sorted;
    toplogical_sort_backward(gnodes, sorted, [](Node*){});
    nt = tflag_count;
    vector<Var*> gvars;
    gvars.reserve(sorted.size());
    for (Node* node : sorted)
        if (node->is_var()) {
            Var* v = node->var();
            v->custom_data = gvars.size();
            gvars.push_back(v);
        }
    LOGvv << "Size of grad vars:" << gvars.size();
    
    vector<VarPtr> grads(gvars.size());
    vector<VarPtr> results(targets.size());
    vector<int> target_id(targets.size());
    for (int i=0; i<targets.size(); i++) {
        Var* var = targets[i];
        target_id[i] = (var->tflag == nt) ?
            var->custom_data : -1;
    }

    if (grads.size()) {
        grads[0] = make_number(1.f, loss);
        assign_attrs(grads[0].ptr, loss);
    }

    vector<pair<Node*, int64>> id_buffer;
    id_buffer.reserve(sorted.size()+10);

    // backup id in custum data
    for (int i=1; i<gvars.size(); i++) {
        Var* var = gvars[i];
        for (auto it : var->outputs_with_index()) {
            Op* op = it.op;
            auto index = it.index;
            if (op->tflag != nt) continue;
            id_buffer.emplace_back(op, index);
        
            // backward together
            if (op->flags.get(NodeFlags::_grads)) {
                // dont backward next time
                op->tflag = 0;
                for (Var* out : op->outputs()) {
                    id_buffer.emplace_back(
                        out, 
                        out->tflag == nt ? out->custom_data : -1);
                }
                for (Var* in : op->inputs()) {
                    id_buffer.emplace_back(
                        in, 
                        in->tflag == nt ? in->custom_data : -1);
                }
            } else {
                // single var backward
                for (Var* out : op->outputs()) {
                    id_buffer.emplace_back(
                        out, 
                        out->tflag == nt ? out->custom_data : -1);
                }
            }
        }
        // end of var output
        id_buffer.emplace_back(nullptr, 0);
    }
    
    // real backward construction from prev backuped ids
    int j=0;
    for (int i=1; i<gvars.size(); i++,j++) {
        Var* var = gvars[i];
        auto& grad = grads[i];
        #ifdef PREVENT_LARGE_FUSED_OP
        int gsum = 0;
        #endif
        // dump  "for (auto it : var->outputs_with_index())"
        while (id_buffer[j].first) {
            Op* op = id_buffer[j].first->op();
            auto index = id_buffer[j].second;
            j++;
            auto n_o = op->outputs().size();
        
            if (op->flags.get(NodeFlags::_grads)) {
                // backward together
                auto n_i = op->inputs().size();
                STACK_ALLOC(Var*, douts, n_o);
                STACK_ALLOC2(VarPtr, dins, n_i);
                // dump "for (Var* out : op->outputs())"
                for (int i=0; i<n_o; i++,j++) {
                    auto id = id_buffer[j].second;
                    if (id>=0) {
                        douts[i] = grads[id];
                    } else
                        douts[i] = nullptr;
                }
                trace_grad_op = op;
                {
                    AmpGradGuard agg(op);
                    op->grads(douts, dins);
                }
                // dump "for (Var* in : op->inputs())"
                for (int i=0; i<n_i; i++,j++) {
                    auto id = id_buffer[j].second;
                    if (id>=0) {
                        auto& din = dins[i];
                        auto& grad = grads[id];
                        if (din && grad) {
                            grad = make_binary(grad, din, ns_add);
                        } else
                            grad = move(din);
                    }
                }
            } else {
                // single var backward
                // dump "for (Var* out : op->outputs())"
                for (int i=0; i<n_o; i++,j++) {
                    auto id = id_buffer[j].second;
                    auto out = id_buffer[j].first->var();
                    if (id<0) continue;
                    Var* dout = grads[id];
                    trace_grad_op = op;
                    VarPtr dvar = make_grad(op, out, dout, var, index);
                    if (dvar && dvar->num>=0 && var->num>0)
                        // var->num == 0 represents a any match var
                        ASSERT(dvar->num==var->num && dvar->shape.size()==var->shape.size())
                        << "dvar" << dvar << "var" << var;
                    if (!grad)
                        grad = move(dvar);
                    else if (dvar) {
                        grad = make_binary(grad, dvar, ns_add);
                        #ifdef PREVENT_LARGE_FUSED_OP
                        gsum ++;
                        if (gsum>=PREVENT_LARGE_FUSED_OP) {
                            // TODO: this is a dirty fix for
                            // stopping fuse lots of op together,
                            // try to find a better solution
                            grad->flags.set(NodeFlags::_stop_fuse);
                        }
                        #endif
                        assign_attrs(grad.ptr, var);
                    }
                }
            }
        }
        if (auto_mixed_precision_level == 3 && grad->ns != var->ns) {
            grad = make_unary(grad, var->ns);
        }
    }
    trace_grad_op = nullptr;
    // set zero grad
    for (size_t i=0; i<results.size(); i++) {
        Var* var = targets[i];
        VarPtr& grad = results[i];
        auto id = target_id[i];
        if (id>=0)
            grad = move(grads[id]);
        if (!grad) {
            // TODO: better warning message
            warn_grad_break(i, var);
            grad = make_number(0.f, var);
            assign_attrs(grad.ptr, var);
        }
    }
    if (!retain_graph) {
        auto t = ++tflag_count;
        for (auto& vh : hold_vars)
            if (vh->var->tflag != t) {
                vh->var->tflag = t;
            }
        SetupFreeBuffer setup_free_buffer;
        for (int i=int(gvars.size())-1; i>=0; i--)
            if (gvars[i]->tflag != t && gvars[i]->backward_liveness)
                gvars[i]->set_stop_grad();
        for (int i=0; i<grads.size(); i++)
            if (grads[i])
                grads[i]->set_stop_grad();
    }
    return results;
}

} // jittor