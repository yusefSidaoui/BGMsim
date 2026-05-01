#pragma once
#include "ASGraph.h"
#include "Announcement.h"
#include <vector>
#include <string>

bool better(const Announcement& a, const Announcement& b);

void processNode(ASNode& node);

void propagate(ASGraph& graph, const std::vector<std::vector<int>>& ranks);

void seed(ASGraph& graph);

void dumpCSV(ASGraph& graph, const std::string& outFile);
