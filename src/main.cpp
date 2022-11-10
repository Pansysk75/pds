#include "DirectedGraph.hpp"
#include <numeric>
#include <algorithm>

int main(){
    // DirectedGraph graph("../celegansneural.mtx");
    DirectedGraph graph;
    graph.size = 6;
    graph.edges = { 
        {0,1}, {1,2},
        {1,4}, {1,5},
        {2,3}, {3,5},
        {4,0}, {5,2},
         };
    
    GraphCSR csr_graph(graph);

    // Fill colors with 0,1,2...n so that,
    // for nodes 0,1,2...n,  colors[i] = i;
    std::vector<unsigned int> colors(graph.size);
    std::iota(colors.begin(), colors.end(), 0);

    // Each node will be assigned an integer according to 
    // which SCC it belongs. Nodes belonging to the same SCC
    // will be assigned the same integer.
    // -1 means that the node hasn't been assigned to a SCC yet.
    std::vector<int> groups(graph.size, -1);

    bool changed_color = true;
    while(changed_color){
        changed_color = false;
        for(auto& elem : graph.edges){
            unsigned int from_color = colors[elem.from];
            unsigned int to_color = colors[elem.to];
            if (to_color > from_color){
               colors[elem.to] = from_color;
               changed_color = true;
            }
        }
    }
    unsigned int n_nodes_assigned = 0;
    while (n_nodes_assigned < graph.size) {
        //TODO
    }


    
}