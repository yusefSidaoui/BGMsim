#include <iostream>
#include <string>
#include <fstream>
#include <cctype>
#include "ASGraph.h"
#include <sstream>
#include "Parser.h"

void parseASRel(const std::string& filename, ASGraph& graph) {
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);

        std::string a, b, typeStr, source;

        std::getline(ss, a, '|');
        std::getline(ss, b, '|');
        std::getline(ss, typeStr, '|');

        int as1 = std::stoi(a);
        int as2 = std::stoi(b);
        int type = std::stoi(typeStr);

        graph.nodes.try_emplace(as1, ASNode{as1});
        graph.nodes.try_emplace(as2, ASNode{as2});

        if (type == -1) {
            graph.nodes[as1].customers.insert(as2);
            graph.nodes[as2].providers.insert(as1);
        }
        else if (type == 0) {
            graph.nodes[as1].peers.insert(as2);
            graph.nodes[as2].peers.insert(as1);
        }
    }
}

void parseROV(const std::string& filename, ASGraph& graph) {
    std::ifstream file(filename);

    int asn;

    while(file >> asn) {
        if (graph.nodes.count(asn)) {
            graph.nodes[asn].rovEnabled = true;
        }
    }
}

void seedFromFile(const std::string& filename, ASGraph& graph) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open AS-rel file: " << filename << "\n";
        return;
    }
    std::string line;

    std::getline(file, line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);

        std::string asnStr, prefix, rovStr;

        std::getline(ss, asnStr, ',');
        std::getline(ss, prefix, ',');
        std::getline(ss, rovStr, ',');

        int asn = std::stoi(asnStr);
        bool rov_invalid = (rovStr == "True");

        Announcement ann;
        ann.prefix = prefix;
        ann.asPath = {asn};
        ann.nextHop = asn;
        ann.from = Relationship::ORIGIN;
        ann.rov_invalid = rov_invalid;

        graph.nodes[asn].localRIB[prefix] = ann;
    }

}
