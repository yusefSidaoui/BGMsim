#pragma once
#include "ASNode.h"
#include <unordered_map>
#include <vector>

class ASGraph {
public:
    std::unordered_map<int, ASNode> nodes;

    void addProviderCustomer(int provider, int customer);
    void addPeer(int a, int b);

    bool detectCycles();

    ASNode& getNode(int asn);

    std::vector<std::vector<int>> computeRanks();
};

