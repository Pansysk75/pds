#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cstring>

struct DirectedEdge{
    unsigned int from;
    unsigned int to;
};

struct DirectedGraph{
    // DirectedGraph "contains" all nodes 1,2,3, ... , size
    // Nodes are referred (and indexed) using this id,
    // which doesn't need to be stored.
    unsigned int size;
    std::vector<DirectedEdge> edges;


    DirectedGraph(std::string filename){

        // Get a directed graph by parsing 
        // a .mtx (sparse matrix) file
        std::ifstream file(filename);
        std::string line;


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
            if (file >> weight >> from >> to){
                this->edges.push_back(DirectedEdge{from, to});
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

