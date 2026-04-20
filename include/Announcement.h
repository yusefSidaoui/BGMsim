#pragma once
#include <vector>
#include <string>

struct Announcement {
    std::string prefix;
    std::vector<int> asPath;
    int nextHop;
    std::string fromRelationship;
    bool rovInvalid = false;

};
