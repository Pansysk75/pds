
#include "../scc_algorithms.hpp"
#include <vector>

bool EqualityTestSCC(std::vector<int>& scc_ids_1, std::vector<int>& scc_ids_2) {
    std::vector<int> pairs(scc_ids_1.size(), -1);
    for (int v = 0; v < scc_ids_1.size(); v++) {
        int id1 = scc_ids_1[v];
        int id2 = scc_ids_2[v];
        if (pairs[id1] == -1) {
            pairs[id1] = id2;
        }
        else {
            if (pairs[id1] != id2) {
                return false;
            }

        }

    }
    return true;
}
