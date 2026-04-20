#include <iostream>
#include "ASGraph.h"

void ASGraph::addProviderCustomer(int provider, int customer){

    nodes.try_emplace(provider, ASNode{provider});
    nodes.try_emplace(customer, ASNode{customer});

    nodes[provider].customer.insert(customer);
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

    for(int child : graph[node].customer) {
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
