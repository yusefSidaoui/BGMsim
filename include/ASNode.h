#pragma once
#include <unordered_set>

struct ASNode {
    int asn;
    std::unordered_set<int> providers;
    std::unordered_set<int> customers;
    std::unordered_set<int> peers;
};
