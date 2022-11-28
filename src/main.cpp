#include "scc_algorithms/scc_algorithms.hpp"
#include <iostream>
#include <utility>
#include "utilities.hpp"

int main(int argc, char *argv[]){

    if(argc<2){
        std::cout << "Usage: program some_data_file.mtx n_iterations(default=1)" << std::endl;
        return 1;
    }
    std::string filename(argv[1]);

    int iterations = 1;
    if (argc>2) iterations = std::atoi(argv[2]);

    std::cout << "Parsing file '" << filename << "'\n";

    GraphCSC csc_graph = GraphCSC(DirectedGraph{filename});


    std::cout << "____Starting Benchmark____\n";

    utilities::timer timer;

    double totaltime = 0;
    for(int i=0; i<iterations; i++){
        timer.start();

        auto result = ColoringSCCAlgorithm(csc_graph);

        timer.stop();
        totaltime+= timer.get()/iterations;
        // std::cout << ".";
        std::cout << "Number of SCCs: " << std::get<1>(result) << std::endl;
    }

    std::cout << "\n____Finished Benchmark____\n";

    std::cout << "t = " << totaltime/1000000.0 << " ms" <<std::endl;

    return 0;

}