#pragma once
#include <vector>
#include <string>

enum class Relationship {
    ORIGIN,
    CUSTOMER,
    PEER,
    PROVIDER
};

struct Announcement {
    std::string prefix;
    std::vector<int> asPath;
    int nextHop;
    Relationship from;
    bool rov_invalid = false;

};
