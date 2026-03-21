#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <string>

struct Parameters {
    int minReviewsPerSubmission   = 1;
    int maxReviewsPerReviewer     = 1;
    int primaryReviewerExpertise  = 1;
    int secondaryReviewerExpertise= 0;
    int primarySubmissionDomain   = 1;
    int secondarySubmissionDomain = 1;
    int generateAssignments       = 1; // 0-3
    int riskAnalysis              = 0; // 0, 1, K
    std::string outputFileName    = "output.csv";
};

#endif // PARAMETERS_H
