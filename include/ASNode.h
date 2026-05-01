#pragma once
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "Announcement.h"

struct ASNode {
    int asn;
    int rank = -1;
    std::unordered_set<int> providers;
    std::unordered_set<int> customers;
    std::unordered_set<int> peers;


    std::unordered_map<std::string, Announcement> localRIB;
    std::unordered_map<std::string, std::vector<Announcement>> receivedQueue;

    bool rovEnabled = false;
};
