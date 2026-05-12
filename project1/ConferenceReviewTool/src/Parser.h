#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include "Reviewer.h"
#include "Submission.h"
#include "Parameters.h"

/**
 * @brief Resultado do processamento (parsing) de um ficheiro CSV.
 */
struct ConferenceData {
    std::vector<Submission> submissions;
    std::vector<Reviewer>   reviewers;
    Parameters              params;
    bool                    valid = false;
};

/**
 * @brief Lê e processa um ficheiro CSV da conferência.
 * Complexidade Temporal: O(S + R) onde S = submissions, R = reviwers.
 */
class Parser {
public:
    static ConferenceData parse(const std::string& filename);

private:
    static std::string trim(const std::string& s);
    static std::string stripInlineComment(const std::string& s);
    static std::string unquote(const std::string& s);
    static std::vector<std::string> splitCSV(const std::string& line);
};

#endif // PARSER_H
