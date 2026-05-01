#pragma once
#include <string>
#include "ASGraph.h"

void parseASRel(const std::string& filename, ASGraph& graph);
void parseROV(const std::string& filename, ASGraph& graph);
void seedFromFile(const std::string& filename, ASGraph& graph);
