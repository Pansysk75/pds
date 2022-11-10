#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cstring>
#include <stdexcept>
#include <numeric>







struct DirectedEdge {
    unsigned int from;
    unsigned int to;
};


struct DirectedGraph{
    // DirectedGraph "contains" all nodes 1,2,3, ... , size
    // Nodes are referred (and indexed) using this id,
    // which doesn't need to be stored.
    unsigned int size;
    std::vector<DirectedEdge> edges;

    DirectedGraph() 
        :size(0) 
    {

    }
    DirectedGraph(std::string filename){

        // Get a directed graph by parsing 
        // a .mtx (sparse matrix) file
        std::ifstream file(filename);
        std::string line;
        if (!file.good()) {
            throw std::invalid_argument("Unable to read file: " + filename);
        }

        unsigned int rows, cols, n_values;
        // discard comments and first line of integers, which
        // indicate size of data 
        char c = file.peek();
        while(c == '%'){
            file.clear();
            file.ignore(256, '\n');
            c = file.peek();
        }
        
        file >> rows >> cols >> n_values;

        this->size = rows;

        unsigned int weight, from, to;
        //read file while ignoring lines that are not 
        //in the form "integer integer integer"
        while(!file.eof()){ 
            if (file >> from >> to >> weight){
                //Subtract 1 so that node ids start from 0
                this->edges.push_back(DirectedEdge{to-1, from-1});
                // std::cout << "S" ;
            }
            else{
                file.clear();
                file.ignore(256, '\n');
                // std::cout << "F" ;
            }
        }
    }
};


struct GraphCSR {
    //stores directed graph edges in "Compressed Sparse Row" format
    std::vector<unsigned int> vec_from_idx;
    std::vector<unsigned int> vec_to;
    unsigned int max_node_id;

    GraphCSR(DirectedGraph& graph_coo) {
        auto& edges = graph_coo.edges;
        /*
        //doesn't assume graph_coo edges are sorted in any way

        //counts[i] will contain the amount of edges that satistfy from==i
        std::vector<unsigned int> counts(graph_coo.max_node_id);
        for (auto& edge : edges) {
            counts[edge.from]++;
        }

        from_idx.resize(counts.size() + 1);
        from_idx = std::exclusive_scan(); //todo
        */

        //LETS ASSUME PRIMARY SORT -> TO, SECONDARY SORT -> FROM
        int n_nodes = graph_coo.size;
        vec_to.reserve(edges.size());
        std::vector<unsigned int> counts(n_nodes, 0);
        for (auto& edge : edges) {
            counts[edge.from]++;
            vec_to.push_back(edge.to);
        }
        vec_from_idx.resize(n_nodes + 1, 0);
        vec_from_idx[0] = 0;
        //accumulate count
        for (int i = 0; i < n_nodes; i++) {
            vec_from_idx[i + 1] = vec_from_idx[i] + counts[i];
        }


    }
};


