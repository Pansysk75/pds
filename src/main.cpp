#include "scc_algorithms/scc_algorithms.hpp"
#include <iostream>
#include <utility>
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

    unsigned int from_sum = 0;
    unsigned int to_sum = 0;

    for(auto& edge : coo_graph.edges){
        from_sum += edge.from + 1 ;
        to_sum += edge.to + 1;
    }

    std::cout << "from_sum: " << from_sum << std::endl;
    std::cout << "to_sum: " << to_sum << std::endl;


    std::cout << "____Starting Benchmark____\n";

    utilities::timer timer;

    int iterations = 1;
    double totaltime = 0;
    for(int i=0; i<iterations; i++){
        timer.start();

        auto result = ColoringSCCAlgorithm(csc_graph);

        timer.stop();
        totaltime+= timer.get()/iterations;

        std::cout << "Number of SCCs: " << std::get<1>(result) << std::endl;
    }

    std::cout << "\n____Finished Benchmark____\n";

    std::cout << "t = " << totaltime/1000000.0 << " ms" <<std::endl;

    return 0;

}