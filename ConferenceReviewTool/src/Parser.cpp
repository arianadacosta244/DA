#include "Parser.h"
#include <fstream>
#include <iostream>
#include <set>

std::string Parser::trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    size_t b = s.find_last_not_of(" \t\r\n");
    return (a == std::string::npos) ? "" : s.substr(a, b - a + 1);
}

std::string Parser::stripInlineComment(const std::string& s) {
    bool inQ = false;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '"') inQ = !inQ;
        if (!inQ && s[i] == '#') return s.substr(0, i);
    }
    return s;
}

std::string Parser::unquote(const std::string& s) {
    std::string t = trim(s);
    if (t.size() >= 2 && t.front() == '"' && t.back() == '"')
        return t.substr(1, t.size() - 2);
    return t;
}

std::vector<std::string> Parser::splitCSV(const std::string& line) {
    std::vector<std::string> tokens;
    std::string cur;
    bool inQ = false;
    for (char c : line) {
        if (c == '"') { inQ = !inQ; cur += c; }
        else if (c == ',' && !inQ) { tokens.push_back(trim(cur)); cur.clear(); }
        else cur += c;
    }
    tokens.push_back(trim(cur));
    return tokens;
}

ConferenceData Parser::parse(const std::string& filename) {
    ConferenceData data;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Cannot open file: " << filename << "\n";
        return data;
    }

    enum Section { NONE, SUBMISSIONS, REVIEWERS, PARAMETERS, CONTROL };
    Section section = NONE;
    std::string line;
    int lineNum = 0;
    bool hasErrors = false;
    std::set<int> subIds, revIds;

    while (std::getline(file, line)) {
        lineNum++;
        std::string raw = trim(line);
        if (raw.empty()) continue;

        // Section headers — check BEFORE stripping comments
        if (raw == "#Submissions") { section = SUBMISSIONS; continue; }
        if (raw == "#Reviewers")   { section = REVIEWERS;   continue; }
        if (raw == "#Parameters")  { section = PARAMETERS;  continue; }
        if (raw == "#Control")     { section = CONTROL;     continue; }
        if (raw == "#")            { section = NONE;         continue; }
        if (raw[0] == '#')         { continue; } // column headers like #Id, Title...

        // Strip inline comment from data lines
        line = trim(stripInlineComment(raw));
        if (line.empty()) continue;

        auto tok = splitCSV(line);

        if (section == SUBMISSIONS) {
            if (tok.size() < 5) {
                std::cerr << "[ERROR] Line " << lineNum << ": submission needs at least 5 fields.\n";
                hasErrors = true; continue;
            }
            try {
                int id  = std::stoi(tok[0]);
                if (subIds.count(id)) {
                    std::cerr << "[ERROR] Duplicate submission ID: " << id << "\n";
                    hasErrors = true; continue;
                }
                int primary = std::stoi(tok[4]);
                if (primary <= 0) throw std::invalid_argument("bad topic");
                int secondary = (tok.size() > 5 && !tok[5].empty()) ? std::stoi(tok[5]) : 0;
                subIds.insert(id);
                data.submissions.emplace_back(id, unquote(tok[1]), unquote(tok[2]),
                                              unquote(tok[3]), primary, secondary);
            } catch (...) {
                std::cerr << "[ERROR] Line " << lineNum << ": invalid submission data.\n";
                hasErrors = true;
            }

        } else if (section == REVIEWERS) {
            if (tok.size() < 4) {
                std::cerr << "[ERROR] Line " << lineNum << ": reviewer needs at least 4 fields.\n";
                hasErrors = true; continue;
            }
            try {
                int id = std::stoi(tok[0]);
                if (revIds.count(id)) {
                    std::cerr << "[ERROR] Duplicate reviewer ID: " << id << "\n";
                    hasErrors = true; continue;
                }
                int primary = std::stoi(tok[3]);
                if (primary <= 0) throw std::invalid_argument("bad expertise");
                int secondary = (tok.size() > 4 && !tok[4].empty()) ? std::stoi(tok[4]) : 0;
                revIds.insert(id);
                data.reviewers.emplace_back(id, unquote(tok[1]), unquote(tok[2]),
                                            primary, secondary);
            } catch (...) {
                std::cerr << "[ERROR] Line " << lineNum << ": invalid reviewer data.\n";
                hasErrors = true;
            }

        } else if (section == PARAMETERS) {
            if (tok.size() < 2) continue;
            std::string key = trim(tok[0]), val = trim(tok[1]);
            try {
                if      (key == "MinReviewsPerSubmission")    data.params.minReviewsPerSubmission   = std::stoi(val);
                else if (key == "MaxReviewsPerReviewer")      data.params.maxReviewsPerReviewer     = std::stoi(val);
                else if (key == "PrimaryReviewerExpertise")   data.params.primaryReviewerExpertise  = std::stoi(val);
                else if (key == "SecondaryReviewerExpertise") data.params.secondaryReviewerExpertise= std::stoi(val);
                else if (key == "PrimarySubmissionDomain")    data.params.primarySubmissionDomain   = std::stoi(val);
                else if (key == "SecondarySubmissionDomain")  data.params.secondarySubmissionDomain = std::stoi(val);
            } catch (...) {
                std::cerr << "[ERROR] Line " << lineNum << ": bad parameter '" << key << "'.\n";
                hasErrors = true;
            }

        } else if (section == CONTROL) {
            if (tok.size() < 2) continue;
            std::string key = trim(tok[0]), val = trim(tok[1]);
            try {
                if      (key == "GenerateAssignments") data.params.generateAssignments = std::stoi(val);
                else if (key == "RiskAnalysis")        data.params.riskAnalysis        = std::stoi(val);
                else if (key == "OutputFileName")      data.params.outputFileName      = unquote(val);
            } catch (...) {
                std::cerr << "[ERROR] Line " << lineNum << ": bad control '" << key << "'.\n";
                hasErrors = true;
            }
        }
    }

    if (data.submissions.empty()) std::cerr << "[WARNING] No submissions loaded.\n";
    if (data.reviewers.empty())   std::cerr << "[WARNING] No reviewers loaded.\n";
    data.valid = !hasErrors;
    return data;
}
