#include <iostream>
#include "ASNode.h"
#include "ASGraph.h"
#include "Parser.h"
#include "BGP.h"
#include <string>

int main(int argc, char* argv[]){

    if (argc != 5){
        std::cerr << "Usage: ./simulator <as-rel> <announcements> <rov> <output>\n";
        return 1;
    }

    std::string asRelFile = argv[1];
    std::string annFile = argv[2];
    std::string rovFile = argv[3];
    std::string outFile = argv[4];

    ASGraph graph;

    parseASRel(asRelFile, graph);

    if (graph.detectCycles()) {
        std::cerr << "Cycle detected\n";
        return 1;
    }

    parseROV(rovFile, graph);

    seedFromFile(annFile, graph);

    auto ranks = graph.computeRanks();


    propagate(graph, ranks);

    dumpCSV(graph, outFile);


    return 0;
}
