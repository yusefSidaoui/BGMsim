#include <iostream>
#include "ASNode.h"
#include "ASGraph.h"

using std::cout, std::endl;

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

int main(){

    ASGraph graph;

    parseASRel("caida.as-rel2", graph);

    if (graph.detectCycles()) return 1;

    return 0;
}
