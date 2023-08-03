// ***************************************************************
// Copyright (c) 2023 Jittor. All Rights Reserved. 
// Maintainers: Dun Liang <randonlang@gmail.com>. 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.
// ***************************************************************
#include <sstream>
#include <algorithm>
#include "graph.h"
#include "var_holder.h"
#include "var.h"

namespace jittor {

DEFINE_FLAG(int, check_graph, 0, "Unify graph sanity check.");


template <typename T>
string ss_convert(T x) {
    std::stringstream ss;
    ss << x;
    return ss.str();
}

void do_graph_check() {
    vector<Node*> queue;
    unordered_map<Node*,int> visited;
    for (auto& vh : hold_vars) {
        if (0==visited[vh->var]++)
            queue.push_back(vh->var);
    }
    LOGvv << "Check hold_vars size" << queue.size();
    int vhsize = queue.size();
    for (auto* node : queue) {
        // ASSERTop(node->forward_liveness,>,0);
        ASSERTop(node->backward_liveness,>,0);
    }
    for (uint i=0; i<queue.size(); i++) {
        auto* node = queue[i];
        for (auto* i : node->inputs()) {
            if (visited.count(i)) continue;
            visited[i] = 0;
            queue.push_back(i);
        }
    }
    LOGvv << "Check all var size" << queue.size();
    for (int i=0; i<(int)queue.size(); i++) {
        auto* node = queue[i];
        LOGvvvv << "Check node" << i << node;
        int f=0, b=0, p=0;
        if (i<vhsize) {
            f+=visited.at(node), b+=visited.at(node), p+=visited.at(node);
        }
        for (auto* i : node->inputs()) {
            if (i->is_stop_grad()) continue;
            if (!i->forward_liveness) continue;
            f ++;
        }
        for (auto* o : node->outputs()) {
            if (o->backward_liveness)
                b ++;
            if (o->pending_liveness && !o->is_finished())
                p++;
        }
        // if (f>0 && b>0 && !node->is_finished()) p++;
        if (f!=node->forward_liveness || b!=node->backward_liveness || p!=node->pending_liveness) {
            LOGf << "ERROR" << node << '\n' 
                << f << b << p << i << '\n' 
                << node->inputs() << '\n' 
                << node->outputs();
            continue;
        }
    }
    for (auto& kv : lived_nodes) {
        if (!kv.second) continue;
        auto* node = (Node*) kv.first;
        if (!visited.count(node) && node->tflag != -1) {
            if (node->is_var() && node->_inputs.size())
                continue;
            LOGf << "ERROR dnode" << (void*)node << kv.second << node;
        }
    }
}

DumpGraphs dump_all_graphs() {
    DumpGraphs graphs;
    vector<Node*> queue;
    auto t = ++tflag_count;
    for (auto& vh : hold_vars)
        if (vh->var->tflag != t) {
            vh->var->tflag = t;
            queue.push_back(vh->var);
            graphs.hold_vars.emplace_back(ss_convert(vh->var));
        }
    bfs_both(queue, [](Node*){return true;});
    std::sort(queue.begin(), queue.end(),
        [](Node* a, Node* b) { return a->id < b->id;});
    for (uint i=0; i<queue.size(); i++)
        queue[i]->custom_data = i;
    for (Node* node : queue) {
        graphs.nodes_info.emplace_back(ss_convert(node));
        
        graphs.inputs.emplace_back();
        auto& inputs = graphs.inputs.back();
        inputs.reserve(node->_inputs.size());
        for (auto i : node->_inputs)
            inputs.push_back(i.node->custom_data);

        graphs.outputs.emplace_back();
        auto& outputs = graphs.outputs.back();
        outputs.reserve(node->_outputs.size());
        for (auto o : node->_outputs)
            outputs.push_back(o.node->custom_data);
    }
    return graphs;
}

void clean_graph() {
    vector<Node*> queue;
    auto t = ++tflag_count;
    for (auto& vh : hold_vars)
        if (vh->var->tflag != t) {
            vh->var->tflag = t;
            queue.push_back(vh->var);
        }
    bfs_both(queue, [](Node*){return true;});
    t = ++tflag_count;
    for (auto& vh : hold_vars)
        vh->var->tflag = t;
    SetupFreeBuffer setup_free_buffer;
    for (auto node : queue) {
        if (node->tflag != t) {
            node->set_stop_grad();
        }
    }
}

void check_circle(Node* s) {
    vector<Node*> q = {s};
    vector<int> fa = {-1};
    unordered_set<Node*> visited = {s};
    for (int i=0; i<q.size(); i++) {
        auto n = q[i];
        for (auto o : n->outputs()) {
            if (o == s) {
                LOGe << "Found circle:";
                int j=i;
                vector<Node*> nodes{o};
                while (j) {
                    nodes.push_back(q[j]);
                    j = fa[j];
                }
                for (int i=0; i<nodes.size(); i++) {
                    auto n = nodes[i];
                    auto out = nodes[(i-1+nodes.size())%nodes.size()];
                    auto in = nodes[(i+1)%nodes.size()];
                    int in_id=0, out_id=0;
                    for (auto ii : n->inputs()) {
                        if (ii == in) break;
                        in_id ++;
                    }
                    for (auto oo : n->outputs()) {
                        if (oo == out) break;
                        out_id ++;
                    }
                    LOGe << n << "in:" >> in_id >> '/' >> n->inputs().size() << "out:" >> out_id >> '/' >> n->outputs().size();
                }
                LOGf << "found circle";
            }
            if (!visited.count(o)) {
                visited.emplace(o);
                q.push_back(o);
                fa.push_back(i);
            }
        }
    }

}

} // jittor