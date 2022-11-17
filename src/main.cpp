#include "scc_algorithms.hpp"
#include <iostream>
#include "utilities.hpp"

int main(int argc, char *argv[]){

    if(argc<2){
        std::cout << "Usage: program some_data_file.mtx" << std::endl;
        return 1;
    }
    std::string filename(argv[1]);

    std::cout << "Parsing file '" << filename << "'\n";

    auto coo_graph = DirectedGraph(filename);
    GraphCSC csc_graph(coo_graph);
    GraphCSR csr_graph(coo_graph);

    std::cout << "____Starting Benchmark____\n";

    utilities::timer timer;
    timer.start();

    auto result = TarjanSCCAlgorithm(csr_graph);

    timer.stop();

    std::cout << "____Finished Benchmark____\n";

    std::cout << "t = " << timer.get()/1000000.0 << " ms" <<std::endl;

    return 0;

}