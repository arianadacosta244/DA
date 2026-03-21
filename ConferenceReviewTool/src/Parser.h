#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include "Reviewer.h"
#include "Submission.h"
#include "Parameters.h"

/**
 * @brief Result of parsing a CSV input file.
 */
struct ConferenceData {
    std::vector<Submission> submissions;
    std::vector<Reviewer>   reviewers;
    Parameters              params;
    bool                    valid = false;
};

/**
 * @brief Reads and parses a conference CSV input file.
 * Time Complexity: O(S + R) where S = submissions, R = reviewers.
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
