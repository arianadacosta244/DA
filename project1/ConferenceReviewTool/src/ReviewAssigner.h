#ifndef REVIEW_ASSIGNER_H
#define REVIEW_ASSIGNER_H

#include "data_structures/Graph.h"
#include "Parser.h"
#include <vector>
#include <map>

/**
 * @brief Uma review atribuída: paper <-> reviewer com o tópico de match.
 */
struct Assignment {
    int submissionId;
    int reviewerId;
    int matchedDomain;
};

/**
 * @brief IDs dos nós na rede de fluxo:
 *   0           = Source
 *   1           = Sink
 *   2000 + t    = Tópico t
 *   3000 + r    = Reviewer r
 *   4000 + s    = Submission s
 */
struct NodeId {
    static int source()        { return 0; }
    static int sink()          { return 1; }
    static int topic(int t)    { return 2000 + t; }
    static int reviewer(int r) { return 3000 + r; }
    static int submission(int s){ return 4000 + s; }
};

/**
 * @brief Motor de atribuição baseado em Max-Flow (Edmonds-Karp).
 *
 * Rede (GenerateAssignments == 1, domínios primários):
 *   Source → Tópico(t)          cap = INF
 *   Tópico(t) → Reviewer(r)     cap = MaxReviewsPerReviewer  (se r.primary == t)
 *   Reviewer(r) → Submission(s) cap = 1                      (se s.primary == t)
 *   Submission(s) → Sink        cap = MinReviewsPerSubmission
 *
 * Complexidade Edmonds-Karp: O(V * E^2)
 */
class ReviewAssigner {
public:
    static std::vector<Assignment> assign(const ConferenceData& data);
    static void writeOutput(const ConferenceData& data, const std::vector<Assignment>& assignments);
    static void printSummary(const ConferenceData& data, const std::vector<Assignment>& assignments);

    /**
     * @brief Risk Analysis K=1: identifica reviewers cuja remoção quebra a cobertura.
     * Complexidade: O(R * V * E^2)
     */
    static std::vector<int> riskAnalysisK1(const ConferenceData& data);
    static void printRiskAnalysis(int K, const std::vector<int>& risky);
    static void appendRiskAnalysisToFile(const std::string& filename, int K, const std::vector<int>& risky);

private:
    static Graph<int> buildFlowGraph(const ConferenceData& data, int excludeReviewer = -1);
    static double edmondsKarp(Graph<int>& g, int source, int sink);
    static bool bfs(Graph<int>& g, int source, int sink, std::map<int, Edge<int>*>& parent);
    static std::vector<Assignment> extractAssignments(const Graph<int>& g, const ConferenceData& data);
    static bool isFullyCovered(const Graph<int>& g, const ConferenceData& data);
};

#endif // REVIEW_ASSIGNER_H
