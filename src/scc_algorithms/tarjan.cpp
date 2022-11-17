#include "../scc_algorithms.hpp"

#include <vector>
#include <stack>
#include <functional>

std::vector<int> TarjanSCCAlgorithm(GraphCSR& graph) {

    std::vector<int> scc_ids(graph.size, -1);
    std::vector<int> low(graph.size);
    std::vector<bool> on_stack(graph.size, false);
    std::stack<unsigned int> stack;
    unsigned int id = 0;
    unsigned int scc_count = 0;

    std::function<void(int)> dfs;

    dfs = [&](unsigned int at) -> void {
        stack.push(at);
        on_stack[at] = true;
        scc_ids[at] = id;
        low[at] = id;
        id++;

        // visit all neighbours
        // (not efficient but i wrote this alg just for validation)
        unsigned int to_idx_start = graph.vec_from_idx[at];
        unsigned int to_idx_end = graph.vec_from_idx[at + 1];
        for (unsigned int to_idx = to_idx_start; to_idx < to_idx_end; to_idx++) {
            auto to = graph.vec_to[to_idx];
            if (scc_ids[to] == -1) dfs(to); //call recursion
            if (on_stack[to]) low[at] = std::min(low[at], low[to]);
        }
        if (scc_ids[at] == low[at]) {
           while(!stack.empty()) {
               auto node = stack.top();
               stack.pop();
               on_stack[node] = false;
               low[node] = scc_ids[at];
               if (node == at) break;
            }
           scc_count++;

        }
    };  

    for (int i = 0; i < graph.size; i++) {
        if (scc_ids[i] == -1) {
            dfs(i);
        }
    }
    return low;
}
