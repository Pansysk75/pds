//#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <stdexcept>
#include <numeric>
#include <algorithm>
#include <vector>


struct DirectedEdge {
    unsigned int from;
    unsigned int to;
};


struct DirectedGraph{
    // DirectedGraph "contains" all nodes 0,1,2, ... , size-1
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
        if (!file.good()) {
            throw std::invalid_argument("Unable to read file: " + filename);
        }

        unsigned int rows, cols, n_values;
        // discard first lines of file, which we don't need
        char c = file.peek();
        while(c == '%'){
            file.clear();
            file.ignore(256, '\n');
            c = file.peek();
        }
        
        file >> rows >> cols >> n_values;

        this->size = rows;
        this->edges.reserve(n_values);

        // I dont like C and pointers, but I didn't manage to find a
        // C++ facility that was as fast as this
        const int line_bufsize = 64;
        char line_buffer[line_bufsize];      
        unsigned int ints[3];
        while (!file.eof()) {
            int i = 0;
            file.getline(line_buffer, line_bufsize);
            char* token = strtok(line_buffer, " ");
            
            if (token == NULL) continue;
            while (token != NULL) {
                if (i > 1) { break; }
                ints[i] = atoi(token);
                token = strtok(NULL, " \n");
                i++;
                
            }
            // std::cout << ints[0] << " " << ints[1] << " " << ints[2] << "\n";
            this->edges.push_back(DirectedEdge{ ints[1] - 1, ints[0] - 1 });      
        }
    }
};


struct GraphCSR {
    //stores directed graph edges in "Compressed Sparse Row" format
    std::vector<unsigned int> vec_from_idx;
    std::vector<unsigned int> vec_to;
    unsigned int size; //number of nodes

    GraphCSR(DirectedGraph& graph_coo) {
        this->size = graph_coo.size;
        auto edges = graph_coo.edges; //makes a copy
       
        auto comparison_funct = [](DirectedEdge& e1, DirectedEdge& e2)
        {
            //return true if e1 < e2
            if (e1.from < e2.from) return true;
            if (e1.from == e2.from) return e1.to < e2.to;
            return false;
        };

        std::sort(edges.begin(), edges.end(), comparison_funct);

        //now edges are sorted
        vec_to.reserve(edges.size());
        std::vector<unsigned int> counts(size, 0);
        for (auto& edge : edges) {
            counts[edge.from]++;
            vec_to.push_back(edge.to);
        }
        vec_from_idx.resize(size + 1, 0);
        vec_from_idx[0] = 0;
        //accumulate count
        for (int i = 0; i < size; i++) {
            vec_from_idx[i + 1] = vec_from_idx[i] + counts[i];
        }
    }
};

struct GraphCSC {
    //stores directed graph edges in "Compressed Sparse Row" format
    std::vector<unsigned int> vec_to_idx;
    std::vector<unsigned int> vec_from;
    unsigned int size; //number of nodes

    GraphCSC(DirectedGraph& graph_coo) {
        this->size = graph_coo.size;
        auto edges = graph_coo.edges; //makes a copy

        auto comparison_funct = [](DirectedEdge& e1, DirectedEdge& e2)
        {
            //return true if e1 < e2
            if (e1.to < e2.to) return true;
            if (e1.to == e2.to) return e1.from < e2.from;
            return false;
        };

        std::sort(edges.begin(), edges.end(), comparison_funct);

        //now edges are sorted
        vec_from.reserve(edges.size());
        std::vector<unsigned int> counts(size, 0);
        for (auto& edge : edges) {
            counts[edge.to]++;
            vec_from.push_back(edge.from);
        }
        vec_to_idx.resize(size + 1, 0);
        vec_to_idx[0] = 0;
        //accumulate count
        for (int i = 0; i < size; i++) {
            vec_to_idx[i + 1] = vec_to_idx[i] + counts[i];
        }
    }
};



