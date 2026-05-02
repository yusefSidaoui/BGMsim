#include "BGP.h"
#include <iostream>
#include <queue>
#include <fstream>
#include <tuple>
bool better(const Announcement& a, const Announcement& b) {

    auto rank = [](Relationship r) -> int {
        switch (r) {
            case Relationship::CUSTOMER: return 3;
            case Relationship::PEER:     return 2;
            case Relationship::PROVIDER: return 1;
            case Relationship::ORIGIN:   return 4;
        }
        return 0;
    };

    int ra = rank(a.from);
    int rb = rank(b.from);

    if (ra != rb)
        return ra > rb;

    if (a.asPath.size() != b.asPath.size())
        return a.asPath.size() < b.asPath.size();

    return a.nextHop < b.nextHop;
}

void seed(ASGraph& graph) {
    Announcement ann;
    ann.prefix = "1.2.0.0/16";
    ann.prefix = {1};
    ann.nextHop = 1;
    ann.from = Relationship::ORIGIN;

    graph.nodes[1].localRIB[ann.prefix] = ann;
}

std::vector<std::vector<int>> computeRanks(ASGraph& graph) {
    std::vector<std::vector<int>> ranks;

    for(auto& [asn,node] : graph.nodes){
        node.rank = -1;
    }

    std::queue<int> q;

    for (auto& [asn, node] : graph.nodes) {
        if (node.customers.empty()) {
            node.rank = 0;
            q.push(asn);
        }
    }

    while (!q.empty()) {
        int curr = q.front(); q.pop();
        int currRank = graph.nodes[curr].rank;

        for (int provider : graph.nodes[curr].providers) {
            if (graph.nodes[provider].rank == -1) {
                graph.nodes[provider].rank = currRank + 1;
                q.push(provider);
            }
        }
    }

    int maxRank = 0;
    for (auto& [asn, node] : graph.nodes)
        maxRank = std::max(maxRank, node.rank);

    ranks.resize(maxRank + 1);

    for (auto& [asn, node] : graph.nodes)
        ranks[node.rank].push_back(asn);

    return ranks;
}

void dumpCSV(ASGraph& graph, const std::string& outFile) {
    std::ofstream out(outFile);
    out << "asn,prefix,as_path\n";

    for (auto& [asn, node] : graph.nodes) {
        for (auto& [prefix, ann] : node.localRIB) {
            out << asn << "," << prefix << ",\"(";
            for (int i = 0; i < (int)ann.asPath.size(); i++) {
                out << ann.asPath[i] << ",";
                if (i + 1 < (int)ann.asPath.size()) out << " ";
            }
            out << ")\"\n";
        }
    }
}

void sendAnnouncement(ASNode& fromNode, ASNode& toNode, const Announcement& ann, Relationship rel) {
    Announcement copy = ann;
    copy.nextHop = fromNode.asn;
    copy.from = rel;

    toNode.receivedQueue[copy.prefix].push_back(copy);
}


void processNode(ASNode& node) {
    for (auto& [prefix, announcements] : node.receivedQueue) {
        for (auto& ann : announcements) {
            if(ann.rov_invalid && node.rovEnabled) continue;
            Announcement newAnn = ann;
            newAnn.asPath.insert(newAnn.asPath.begin(), node.asn);

            auto it = node.localRIB.find(prefix);
            if (it == node.localRIB.end() || better(newAnn, it->second)) {
                node.localRIB[prefix] = newAnn;
            }
        }
    }
    node.receivedQueue.clear();
}

void sendUpPhase(ASGraph& graph,
    const std::vector<std::vector<int>>& ranks) {

    for (int r = 0; r < (int)ranks.size(); r++) {

        std::vector<std::tuple<int, std::string, Announcement>> sendBuffer;


        for (int asn : ranks[r]) {
            ASNode& node = graph.nodes[asn];

            for (auto& [prefix, ann] : node.localRIB) {
                sendBuffer.emplace_back(asn, prefix, ann);

            }
        }

        for (auto& [asn, prefix, ann] : sendBuffer){
            ASNode& node = graph.nodes[asn];

            for( int provider : node.providers)
                sendAnnouncement(node, graph.nodes[provider],ann,Relationship::CUSTOMER);
        }

        if (r+1 < (int)ranks.size()){
            for(int asn:ranks[r+1]){
                processNode(graph.nodes[asn]);
            }
        }
    }
}

void peerPhase(ASGraph& graph) {

    std::vector<std::tuple<int, std::string, Announcement>> sendBuffer;

    for (auto& [asn, node] : graph.nodes) {
        for (auto& [prefix, ann] : node.localRIB) {
            sendBuffer.emplace_back(asn,prefix,ann);
        }
    }
    for( auto& [asn, prefix, ann] : sendBuffer){
        ASNode& node = graph.nodes[asn];

        for (int peer : node.peers) {
            sendAnnouncement(node,graph.nodes[peer],ann, Relationship::PEER);
        }

    }

    for (auto& [asn, node] : graph.nodes) {
        processNode(node);
    }
}
void sendDownPhase(ASGraph& graph,
                   const std::vector<std::vector<int>>& ranks) {

    for (int r = ranks.size() - 1; r >= 0; r--) {

        std::vector<std::tuple<int, std::string, Announcement>> sendBuffer;

        for (int asn : ranks[r]) {
            ASNode& node = graph.nodes[asn];

            for (auto& [prefix, ann] : node.localRIB) {
                sendBuffer.emplace_back(asn, prefix, ann);
            }
        }

        for (auto& [asn, prefix, ann] : sendBuffer) {
            ASNode& node = graph.nodes[asn];

            for (int customer : node.customers) {
                sendAnnouncement(node,
                                 graph.nodes[customer],
                                 ann,
                                 Relationship::PROVIDER);
            }
        }

        if (r - 1 >= 0) {
            for (int asn : ranks[r - 1]) {
                processNode(graph.nodes[asn]);
            }
        }
    }
}

void propagate(ASGraph& graph, const std::vector<std::vector<int>>& ranks) {

    sendUpPhase(graph, ranks);

    peerPhase(graph);

    sendDownPhase(graph, ranks);
}