#include "DirectedGraph.hpp"
#include <numeric>
#include <algorithm>
#include <list>
#include <queue>

int main(){
 //    DirectedGraph graph("../celegansneural.mtx");

    std::cout << "Reading file...\n";

    //DirectedGraph graph("../wb-edu.mtx");
    DirectedGraph graph("../celegansneural.mtx");
    // 
    // 
 //   DirectedGraph graph;
 //   graph.size = 6;
 //   graph.edges = { 
 //       {0,4},{1,0},
 //       {2,1}, {2,5},
 //       {3,2}, {4,1},
 //       {5,1}, {5,3}
 //        };
    
    GraphCSR csr_graph(graph);
    std::cout << "Finished preprocessing of " << graph.size << " nodes and " << graph.edges.size() << " edges\n";
    std::cout << "Starting coloring algorithm\n";
    unsigned int iteration_counter = 1;




    //scc_id of -1 means that the node hasn't been added to a SCC yet
    std::vector<int> scc_ids(graph.size, -1);
    int max_ssc_id = 0;

    //init queue

    std::vector<unsigned int> queue;
    
    std::vector<unsigned int> colors;
    colors.resize(graph.size);
    queue.reserve(graph.size);

    for (unsigned int i = 0; i < graph.size; i++) {
        queue.push_back(i);
    }

    //start algorithm
    while (!queue.empty()) {
        std::cout << "Starting iteration " << iteration_counter << "\n";
        //init colors
        for (auto& elem : queue) {
            colors[elem] = elem;
        }

        //color propagation
       bool any_changed_color = true;
       while (any_changed_color) {
            any_changed_color = false;
            for (auto& v : queue) {
                //get all "u"s that point to this "v"
                unsigned int u_idx_start = csr_graph.vec_from_idx[v];
                unsigned int u_idx_end = csr_graph.vec_from_idx[v + 1];
                for (unsigned int u_idx = u_idx_start; u_idx < u_idx_end; u_idx++) {
                    auto u = csr_graph.vec_to[u_idx];
                    if ((scc_ids[u] == -1) && (colors[v] > colors[u])) { //does the order here matter?
                        colors[v] = colors[u];
                        any_changed_color = true;
                    }
                }
            }
       }
       //color propagation finished
       //From every node where node_id == node_color, start BFS
       for (auto& v : queue) {
           if (colors[v] == v) {
               unsigned int curr_color = colors[v];
               int curr_scc_id = max_ssc_id;
               max_ssc_id++; //DATA RACE, CAREFUL HERE
               //BFS from v to its predecessors of the same color, and assign
               //all visited nodes to the SCC
               std::queue<unsigned int> bfs_queue;
               bfs_queue.push(v);
               scc_ids[v] = curr_scc_id;
               while (!bfs_queue.empty()) {
                   unsigned int curr_node = bfs_queue.front();
                   bfs_queue.pop();
                   //get predecessors
                   unsigned int u_idx_start = csr_graph.vec_from_idx[curr_node];
                   unsigned int u_idx_end = csr_graph.vec_from_idx[curr_node + 1];
                   for (unsigned int u_idx = u_idx_start; u_idx < u_idx_end; u_idx++) {
                       auto u = csr_graph.vec_to[u_idx];
                       if (scc_ids[u] == -1 && colors[u] == curr_color) {
                           bfs_queue.push(u);
                           scc_ids[u] = curr_scc_id;
                       }
                   }
               }
           }
       }

       std::cout << "Completed iteration " << iteration_counter++ << "\n";
        //update queue so that it only contains nodes where scc_id[node]==-1
       queue.erase(
           std::remove_if(queue.begin(), queue.end(), [&scc_ids](auto& elem) { return scc_ids[elem] != -1; }),
           queue.end());


    }

    



}