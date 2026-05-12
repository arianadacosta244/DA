#include "ReviewAssigner.h"
#include <queue>
#include <set>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <limits>

// ---------------------------------------------------------------------------
// Construção do grafo de fluxo
// ---------------------------------------------------------------------------
Graph<int> ReviewAssigner::buildFlowGraph(const ConferenceData& data, int excludeReviewer) {
    Graph<int> g;
    const auto& p = data.params;

    g.addVertex(NodeId::source());
    g.addVertex(NodeId::sink());

    int mode = p.generateAssignments;

    std::set<int> topics;
    for (const auto& s : data.submissions) {
        topics.insert(s.primaryTopic);
        if (mode >= 2 && s.secondaryTopic > 0) topics.insert(s.secondaryTopic);
    }
    for (int t : topics) g.addVertex(NodeId::topic(t));
    for (const auto& r : data.reviewers)
        if (r.id != excludeReviewer) g.addVertex(NodeId::reviewer(r.id));
    for (const auto& s : data.submissions) g.addVertex(NodeId::submission(s.id));

    // Helper: adiciona aresta forward + aresta reversa (grafo residual)
    auto addFlowEdge = [&](int u, int v, double cap) {
        auto* vu = g.findVertex(u);
        auto* vv = g.findVertex(v);
        if (!vu || !vv) return;
        auto* e1 = vu->addEdge(vv, cap);
        auto* e2 = vv->addEdge(vu, 0.0);
        e1->setReverse(e2); e2->setReverse(e1);
        e1->setFlow(0);     e2->setFlow(0);
    };

    // Source → Tópico
    for (int t : topics)
        addFlowEdge(NodeId::source(), NodeId::topic(t), std::numeric_limits<double>::max() / 2);

    // Tópico → Reviewer  (capacidade = MaxReviewsPerReviewer)
    for (const auto& r : data.reviewers) {
        if (r.id == excludeReviewer) continue;
        if (topics.count(r.primaryExpertise))
            addFlowEdge(NodeId::topic(r.primaryExpertise), NodeId::reviewer(r.id), p.maxReviewsPerReviewer);
    }

    // Reviewer → Submission  (capacidade = 1)
    // Modo 1: só tópico primário da submission
    // Modo 2: tópico primário ou secundário da submission
    for (const auto& r : data.reviewers) {
        if (r.id == excludeReviewer) continue;
        for (const auto& s : data.submissions) {
            bool match = (r.primaryExpertise == s.primaryTopic);
            if (!match && mode >= 2 && s.secondaryTopic > 0)
                match = (r.primaryExpertise == s.secondaryTopic);
            if (match)
                addFlowEdge(NodeId::reviewer(r.id), NodeId::submission(s.id), 1.0);
        }
    }

    // Submission → Sink  (capacidade = MinReviewsPerSubmission)
    for (const auto& s : data.submissions)
        addFlowEdge(NodeId::submission(s.id), NodeId::sink(), p.minReviewsPerSubmission);

    return g;
}

// ---------------------------------------------------------------------------
// BFS para encontrar caminho aumentante
// ---------------------------------------------------------------------------
bool ReviewAssigner::bfs(Graph<int>& g, int source, int sink,
                          std::map<int, Edge<int>*>& parent) {
    for (auto* v : g.getVertexSet()) v->setVisited(false);
    parent.clear();
    std::queue<int> q;
    auto* src = g.findVertex(source);
    if (!src) return false;
    src->setVisited(true);
    q.push(source);
    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (auto* e : g.findVertex(u)->getAdj()) {
            int v = e->getDest()->getInfo();
            auto* vv = g.findVertex(v);
            if (!vv->isVisited() && e->getWeight() - e->getFlow() > 1e-9) {
                vv->setVisited(true);
                parent[v] = e;
                if (v == sink) return true;
                q.push(v);
            }
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// Edmonds-Karp  O(V * E^2)
// ---------------------------------------------------------------------------
double ReviewAssigner::edmondsKarp(Graph<int>& g, int source, int sink) {
    for (auto* v : g.getVertexSet())
        for (auto* e : v->getAdj()) e->setFlow(0);

    double total = 0;
    std::map<int, Edge<int>*> parent;
    while (bfs(g, source, sink, parent)) {
        // Bottleneck
        double pathFlow = std::numeric_limits<double>::max();
        for (int cur = sink; cur != source; ) {
            auto* e = parent[cur];
            pathFlow = std::min(pathFlow, e->getWeight() - e->getFlow());
            cur = e->getOrig()->getInfo();
        }
        // Augment
        for (int cur = sink; cur != source; ) {
            auto* e = parent[cur];
            e->setFlow(e->getFlow() + pathFlow);
            e->getReverse()->setFlow(e->getReverse()->getFlow() - pathFlow);
            cur = e->getOrig()->getInfo();
        }
        total += pathFlow;
    }
    return total;
}

// ---------------------------------------------------------------------------
// Extrai assignments das arestas Reviewer→Submission com flow > 0
// ---------------------------------------------------------------------------
std::vector<Assignment> ReviewAssigner::extractAssignments(
        const Graph<int>& g, const ConferenceData& data) {
    std::vector<Assignment> result;
    for (const auto& r : data.reviewers) {
        auto* vr = g.findVertex(NodeId::reviewer(r.id));
        if (!vr) continue;
        for (auto* e : vr->getAdj()) {
            int dest = e->getDest()->getInfo();
            if (dest >= 4000 && dest < 5000 && e->getFlow() > 0.5) {
                int sId = dest - 4000;
                result.push_back({sId, r.id, r.primaryExpertise});
            }
        }
    }
    std::sort(result.begin(), result.end(), [](const Assignment& a, const Assignment& b){
        return a.submissionId != b.submissionId ? a.submissionId < b.submissionId
                                                : a.reviewerId   < b.reviewerId;
    });
    return result;
}

bool ReviewAssigner::isFullyCovered(const Graph<int>& g, const ConferenceData& data) {
    for (const auto& s : data.submissions) {
        auto* vs = g.findVertex(NodeId::submission(s.id));
        if (!vs) return false;
        double flow = 0;
        for (auto* e : vs->getAdj())
            if (e->getDest()->getInfo() == NodeId::sink()) flow += e->getFlow();
        if (flow < data.params.minReviewsPerSubmission - 1e-9) return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// API pública
// ---------------------------------------------------------------------------
std::vector<Assignment> ReviewAssigner::assign(const ConferenceData& data) {
    Graph<int> g = buildFlowGraph(data);
    edmondsKarp(g, NodeId::source(), NodeId::sink());
    return extractAssignments(g, data);
}

std::vector<int> ReviewAssigner::riskAnalysisK1(const ConferenceData& data) {
    // Se o assignment base já não cobre tudo, não há reviewers críticos
    Graph<int> base = buildFlowGraph(data);
    edmondsKarp(base, NodeId::source(), NodeId::sink());
    if (!isFullyCovered(base, data)) return {};

    std::vector<int> risky;
    for (const auto& r : data.reviewers) {
        Graph<int> g = buildFlowGraph(data, r.id);
        edmondsKarp(g, NodeId::source(), NodeId::sink());
        if (!isFullyCovered(g, data)) risky.push_back(r.id);
    }
    std::sort(risky.begin(), risky.end());
    return risky;
}

void ReviewAssigner::printSummary(const ConferenceData& data,
                                   const std::vector<Assignment>& assignments) {
    std::map<int, int> cnt;
    for (const auto& s : data.submissions) cnt[s.id] = 0;
    for (const auto& a : assignments) cnt[a.submissionId]++;

    std::cout << "\n#SubmissionId, ReviewerId, Domain\n";
    for (const auto& a : assignments)
        std::cout << a.submissionId << ", " << a.reviewerId << ", " << a.matchedDomain << "\n";

    std::map<int, std::vector<Assignment>> byRev;
    for (const auto& a : assignments) byRev[a.reviewerId].push_back(a);

    std::cout << "#ReviewerId, SubmissionId, Domain\n";
    for (auto& [rid, list] : byRev) {
        std::sort(list.begin(), list.end(), [](const Assignment& x, const Assignment& y){
            return x.submissionId < y.submissionId; });
        for (const auto& a : list)
            std::cout << a.reviewerId << ", " << a.submissionId << ", " << a.matchedDomain << "\n";
    }
    std::cout << "#Total: " << assignments.size() << "\n";

    bool ok = true;
    for (const auto& s : data.submissions) {
        if (cnt[s.id] < data.params.minReviewsPerSubmission) {
            if (ok) { std::cout << "\n#SubmissionId, Domain, MissingReviews\n"; ok = false; }
            std::cout << s.id << ", " << s.primaryTopic << ", "
                      << (data.params.minReviewsPerSubmission - cnt[s.id]) << "\n";
        }
    }
    if (ok) std::cout << "[OK] All submissions fully covered.\n";
}

void ReviewAssigner::writeOutput(const ConferenceData& data,
                                  const std::vector<Assignment>& assignments) {
    std::ofstream out(data.params.outputFileName);
    if (!out.is_open()) {
        std::cerr << "[ERROR] Cannot write: " << data.params.outputFileName << "\n";
        return;
    }
    std::map<int, int> cnt;
    for (const auto& s : data.submissions) cnt[s.id] = 0;
    for (const auto& a : assignments) cnt[a.submissionId]++;

    out << "#SubmissionId,ReviewerId,Match\n";
    for (const auto& a : assignments)
        out << a.submissionId << ", " << a.reviewerId << ", " << a.matchedDomain << "\n";

    std::map<int, std::vector<Assignment>> byRev;
    for (const auto& a : assignments) byRev[a.reviewerId].push_back(a);
    out << "#ReviewerId,SubmissionId,Match\n";
    for (auto& [rid, list] : byRev) {
        std::sort(list.begin(), list.end(), [](const Assignment& x, const Assignment& y){
            return x.submissionId < y.submissionId; });
        for (const auto& a : list)
            out << a.reviewerId << ", " << a.submissionId << ", " << a.matchedDomain << "\n";
    }
    out << "#Total: " << assignments.size() << "\n";
    bool missingHeader = false;
    for (const auto& s : data.submissions) {
        if (cnt[s.id] < data.params.minReviewsPerSubmission) {
            if (!missingHeader) { out << "#SubmissionId,Domain,MissingReviews\n"; missingHeader = true; }
            out << s.id << ", " << s.primaryTopic << ", "
                << (data.params.minReviewsPerSubmission - cnt[s.id]) << "\n";
        }
    }

    std::cout << "[OK] Output written to: " << data.params.outputFileName << "\n";
}

void ReviewAssigner::printRiskAnalysis(int K, const std::vector<int>& risky) {
    std::cout << "\n#Risk Analysis: " << K << "\n";
    if (risky.empty()) {
        std::cout << "[OK] No reviewer is critical.\n";
    } else {
        for (int i = 0; i < (int)risky.size(); i++) {
            if (i) std::cout << ", ";
            std::cout << risky[i];
        }
        std::cout << "\n";
    }
}

void ReviewAssigner::appendRiskAnalysisToFile(const std::string& filename, int K, const std::vector<int>& risky) {
    std::ofstream out(filename, std::ios::app);
    if (!out.is_open()) {
        std::cerr << "[ERROR] Cannot append to: " << filename << "\n";
        return;
    }
    out << "#Risk Analysis: " << K << "\n";
    if (risky.empty()) {
        out << "\n";
    } else {
        for (int i = 0; i < (int)risky.size(); i++) {
            if (i) out << ", ";
            out << risky[i];
        }
        out << "\n";
    }
}
