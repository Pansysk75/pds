#include <vector>
#include "graph.hpp"

std::vector<int> ColoringSCCAlgorithm(GraphCSC& graph);
std::vector<int> TarjanSCCAlgorithm(GraphCSR& graph);
bool EqualityTestSCC(std::vector<int>& scc_ids_1, std::vector<int>& scc_ids_2);
