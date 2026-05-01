#include <iostream>
#include "ASGraph.h"
#include <vector>
#include <queue>

void ASGraph::addProviderCustomer(int provider, int customer){

    nodes.try_emplace(provider, ASNode{provider});
    nodes.try_emplace(customer, ASNode{customer});

    nodes[provider].customers.insert(customer);
    nodes[customer].providers.insert(provider);

}

void ASGraph::addPeer(int a, int b){

    nodes.try_emplace(a, ASNode{a});
    nodes.try_emplace(b, ASNode{b});

    nodes[a].peers.insert(b);
    nodes[b].peers.insert(a);

}

ASNode& ASGraph::getNode(int asn){
    return nodes.at(asn);

}

bool dfs(int node, std::unordered_map<int, ASNode>& graph, std::unordered_set<int>& visiting, std::unordered_set<int>& visited) {

    visiting.insert(node);

    for(int child : graph[node].customers) {
        if (visited.count(child)) continue;

        if (visiting.count(child)) return true;

        if (dfs(child, graph, visiting, visited)) return true;
    }

    visited.erase(node);
    visited.insert(node);

    return false;
}

bool ASGraph::detectCycles() {
    std::unordered_set<int> visited;
    std::unordered_set<int> visiting;

    for (auto& [asn, node] : nodes) {
        if (!visited.count(asn)){
            if(dfs(asn, nodes, visiting, visited)) {
                return true;
            }
        }
    }
    return false;
}

std::vector<std::vector<int>> ASGraph::computeRanks() {
    std::vector<std::vector<int>> ranks;

    std::queue<int> q;

    for (auto& [asn, node] : nodes) {
        if (node.customers.empty()) {
            node.rank = 0;
            q.push(asn);
        }
    }

    while (!q.empty()) {
        int curr = q.front(); q.pop();
        int currRank = nodes[curr].rank;

        for (int provider : nodes[curr].providers) {
            if (nodes[provider].rank < currRank + 1) {
                nodes[provider].rank = currRank + 1;
                q.push(provider);
            }
        }
    }

    int maxRank = 0;
    for (auto& [asn, node] : nodes)
        maxRank = std::max(maxRank, node.rank);

    ranks.resize(maxRank + 1);

    for (auto& [asn, node] : nodes)
        ranks[node.rank].push_back(asn);

    return ranks;
}
