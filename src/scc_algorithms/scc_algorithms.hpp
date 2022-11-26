#include <vector>
#include "../graph.hpp"

std::pair<std::vector<int>, int>  ColoringSCCAlgorithm(GraphCSC& graph);

std::vector<int> TarjanSCCAlgorithm(GraphCSR& graph);

static int TrimSCC(GraphCSC& graph, std::vector<int>& scc_ids){
    // all nodes with 0 in-degree or 0-out-degree will form their own scc

    std::vector<bool> has_zero_in_degree(graph.size, false);
    std::vector<bool> has_zero_out_degree(graph.size, true);

    // find those with 0 out-degree (no edge starts from that node)
    for (auto& from : graph.vec_from){
        has_zero_out_degree[from] = false;
    }

    // find those with 0 in-degree (no edge go to that node)
    for(int v_idx=0; v_idx<graph.vec_to_idx.size()-1; v_idx++) {
        unsigned int& u_idx_start = graph.vec_to_idx[v_idx];
        unsigned int& u_idx_end = graph.vec_to_idx[v_idx+1];
        if(u_idx_start == u_idx_end){
            has_zero_in_degree[v_idx] = true;
        }
    }

    int scc_id_count = 0;
    for(int i=0; i<graph.size; i++){
        if(has_zero_out_degree[i] || has_zero_in_degree[i]){
            scc_ids[i] = scc_id_count++;
        }
    }
    return scc_id_count;
}

bool EqualityTestSCC(std::vector<int>& scc_ids_1, std::vector<int>& scc_ids_2);
