#include <vector>
#include "graph.hpp"

std::pair<std::vector<int>, int>  ColoringSCCAlgorithm(GraphCSC& graph);
std::pair<std::vector<int>, int>  ColoringSCCAlgorithm_OpenCilk(GraphCSC& graph);
std::pair<std::vector<int>, int>  ColoringSCCAlgorithm_OpenMP(GraphCSC& graph);
std::pair<std::vector<int>, int>  ColoringSCCAlgorithm_PThreads(GraphCSC& graph);

std::vector<int> TarjanSCCAlgorithm(GraphCSR& graph);
bool EqualityTestSCC(std::vector<int>& scc_ids_1, std::vector<int>& scc_ids_2);
