#include "scc_algorithms.hpp"

#include <numeric>
#include <algorithm>
#include <list>
#include <queue>
#include <iostream>

#include <atomic>
#include <cilk/cilk.h>



std::pair<std::vector<int>, int>  ColoringSCCAlgorithm(GraphCSC& graph) {
    // std::cout << "Starting coloring algorithm\n";
    unsigned int iteration_counter = 1;


   //scc_id of -1 means that the node hasn't been added to a SCC yet
    std::vector<int> scc_ids(graph.size, -1);
    std::atomic<int> max_scc_id(TrimSCC(graph, scc_ids));
    //std::cout << "trimmed: " << max_scc_id << std::endl;

    std::vector<unsigned int> queue;

    std::vector<unsigned int> colors;
    colors.resize(graph.size);
    queue.reserve(graph.size);

    for (unsigned int i = 0; i < graph.size; i++) {
        if (scc_ids[i] == -1) 
        queue.push_back(i);
    }

    //start algorithm
    while (!queue.empty()) {
        // std::cout << "Starting iteration " << iteration_counter << "\n";
        //init colors
        for (auto& elem : queue) {
            colors[elem] = elem;
        }

        //color propagation
        std::atomic<bool> any_changed_color(true);
        // bool any_changed_color(true);
        
        while (any_changed_color) {
            any_changed_color = false;
            
            cilk_for(int v_idx=0; v_idx<queue.size(); v_idx++){
                auto v = queue[v_idx];
                //get all "u"s that point to this "v"
                unsigned int u_idx_start = graph.vec_to_idx[v];
                unsigned int u_idx_end = graph.vec_to_idx[v + 1];
                for (unsigned int u_idx = u_idx_start; u_idx < u_idx_end; u_idx++) {
                    auto u = graph.vec_from[u_idx];
                    if (scc_ids[u] == -1) { //does the order here matter?
                        if(colors[v] > colors[u]){
                            any_changed_color = true;
                            colors[v] = colors[u];
                        }
                    }
                }
            }
        }
        //color propagation finished
        //From every node where node_id == node_color, start BFS
        cilk_for (int v_idx=0; v_idx<queue.size(); v_idx++) {
            auto v = queue[v_idx];
            if (colors[v] == v){
                unsigned int curr_color = colors[v];
                int curr_scc_id = max_scc_id.fetch_add(1);
                //BFS from v to its predecessors of the same color, and assign
                //all visited nodes to the SCC
                std::queue<unsigned int> bfs_queue;
                bfs_queue.push(v);
                scc_ids[v] = curr_scc_id;
                while (!bfs_queue.empty()) {
                    unsigned int curr_node = bfs_queue.front();
                    bfs_queue.pop();
                    //get predecessors
                    unsigned int u_idx_start = graph.vec_to_idx[curr_node];
                    unsigned int u_idx_end = graph.vec_to_idx[curr_node + 1];
                    for (unsigned int u_idx = u_idx_start; u_idx < u_idx_end; u_idx++) {
                        auto u = graph.vec_from[u_idx];
                        if (scc_ids[u] == -1 && colors[u] == curr_color) {
                            bfs_queue.push(u);
                            scc_ids[u] = curr_scc_id;
                        }
                    }
                }
            }
        }

        // std::cout << "Completed iteration " << iteration_counter++ << "\n";
        //update queue so that it only contains nodes where scc_id[node]==-1
        queue.erase(
            std::remove_if(queue.begin(), queue.end(), [&scc_ids](auto& elem) { return scc_ids[elem] != -1; }),
            queue.end());
    }

    return {std::move(scc_ids), max_scc_id}; //make a pair by moving (aka not copying) the vector into it
}
