#include "DirectedGraph.hpp"
#include <numeric>
#include <algorithm>
#include <list>
#include <queue>

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

std::vector<int> ColoringSCCAlgorithm(GraphCSC& graph) {
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
                unsigned int u_idx_start = graph.vec_to_idx[v];
                unsigned int u_idx_end = graph.vec_to_idx[v + 1];
                for (unsigned int u_idx = u_idx_start; u_idx < u_idx_end; u_idx++) {
                    auto u = graph.vec_from[u_idx];
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

        std::cout << "Completed iteration " << iteration_counter++ << "\n";
        //update queue so that it only contains nodes where scc_id[node]==-1
        queue.erase(
            std::remove_if(queue.begin(), queue.end(), [&scc_ids](auto& elem) { return scc_ids[elem] != -1; }),
            queue.end());
    }

    return scc_ids; //
}

bool EqualityTestSCC(std::vector<int>& scc_ids_1, std::vector<int>& scc_ids_2) {
    std::vector<int> pairs(scc_ids_1.size(), -1);
    for (int v = 0; v < scc_ids_1.size(); v++) {
        int id1 = scc_ids_1[v];
        int id2 = scc_ids_2[v];
        if (pairs[id1] == -1) {
            pairs[id1] = id2;
        }
        else {
            if (pairs[id1] != id2) {
                return false;
            }

        }

    }
    return true;
}

int main(int argc, char *argv[]){

    if(argc<2){
        std::cout << "Usage: sequential my_graph_file.mtx" << std::endl;
        return 1;
    }
    std::string filename(argv[1]);
    std::cout << "Reading file '" << filename << "'\n";

   // DirectedGraph coo_graph;
   // coo_graph.size = 6;
   // coo_graph.edges = { 
   //     {0,1}, {1,2},
   //     {1,4}, {1,5},
   //     {2,3}, {3,5},
   //     {4,0}, {5,2}
   //      };
    
     auto coo_graph = DirectedGraph(filename);
    //auto coo_graph = DirectedGraph("../language.mtx");
    // auto coo_graph = DirectedGraph("../wb-edu.mtx");

   GraphCSR csr_graph(coo_graph);
   GraphCSC csc_graph(coo_graph);
  // GraphCSR graph(DirectedGraph("../wb-edu.mtx"));
    //GraphCSR graph(DirectedGraph("../celegansneural.mtx"));

    // std::cout << "Finished preprocessing of " << graph.size << " nodes and " << graph.vec_to.size() << " edges\n";

    auto result1 = ColoringSCCAlgorithm(csc_graph);
    auto result2 = TarjanSCCAlgorithm(csr_graph);

    // EqualityTestSCC(result1, result2);
    return 0;

}