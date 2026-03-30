#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <string>

/**
 * @brief Parâmetros de configuração do problema lidos do ficheiro de input.
 */
struct Parameters {
    int minReviewsPerSubmission    = 1;   ///< Número mínimo de reviews por submissão
    int maxReviewsPerReviewer      = 1;   ///< Número máximo de reviews por reviewer
    int primaryReviewerExpertise   = 1;   ///< Peso do tópico primário do reviewer
    int secondaryReviewerExpertise = 0;   ///< Peso do tópico secundário do reviewer
    int primarySubmissionDomain    = 1;   ///< Peso do tópico primário da submissão
    int secondarySubmissionDomain  = 1;   ///< Peso do tópico secundário da submissão
    int generateAssignments        = 1;   ///< Modo de geração de assignments (0-3)
    int riskAnalysis               = 0;   ///< Tipo de análise de risco (0, 1 ou K)
    std::string outputFileName     = "output.csv"; ///< Nome do ficheiro de output
};

#endif // PARAMETERS_H
