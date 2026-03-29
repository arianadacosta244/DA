#include <iostream>
#include <string>
#include "Parser.h"
#include "ReviewAssigner.h"

// Estado global da sessão
static ConferenceData gData;
static std::vector<Assignment> gAssignments;
static bool gDataLoaded = false;
static bool gAssigned   = false;

void showMenu() {
    std::cout << "\n=================================" << std::endl;
    std::cout << "   Assignment Tool" << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << "1. Load input file" << std::endl;
    std::cout << "2. Generate Assignments" << std::endl;
    std::cout << "3. Run Risk Analysis" << std::endl;
    std::cout << "4. Show Results" << std::endl;
    std::cout << "5. Export output file" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << "Option: ";
}

void interactiveMode() {
    int option = -1;
    do {
        showMenu();
        if (!(std::cin >> option)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }
        std::cin.ignore(10000, '\n');

        switch (option) {
            case 1: {
                std::cout << "Enter filename: ";
                std::string fname;
                std::getline(std::cin, fname);
                gData = Parser::parse(fname);
                if (!gData.submissions.empty() || !gData.reviewers.empty()) {
                    gDataLoaded = true;
                    gAssigned   = false;
                    std::cout << "[OK] Loaded " << gData.submissions.size()
                              << " submission(s) and " << gData.reviewers.size()
                              << " reviewer(s).\n";
                } else {
                    std::cerr << "[ERROR] Failed to load data.\n";
                }
                break;
            }
            case 2: {
                if (!gDataLoaded) { std::cerr << "[!] Load a file first.\n"; break; }
                if (gData.params.generateAssignments == 0) {
                    std::cout << "[INFO] GenerateAssignments = 0, skipping.\n"; break;
                }
                gAssignments = ReviewAssigner::assign(gData);
                gAssigned = true;
                std::cout << "[OK] " << gAssignments.size() << " assignment(s) generated.\n";
                break;
            }
            case 3: {
                if (!gDataLoaded) { std::cerr << "[!] Load a file first.\n"; break; }
                int K = gData.params.riskAnalysis;
                if (K == 0) { std::cout << "[INFO] RiskAnalysis = 0, skipping.\n"; break; }
                if (K == 1) {
                    auto risky = ReviewAssigner::riskAnalysisK1(gData);
                    ReviewAssigner::printRiskAnalysis(K, risky);
                } else {
                    std::cout << "[INFO] Risk Analysis K=" << K
                              << " (approach outlined in T2.3, not implemented).\n";
                }
                break;
            }
            case 4: {
                if (!gAssigned) { std::cerr << "[!] Generate assignments first.\n"; break; }
                ReviewAssigner::printSummary(gData, gAssignments);
                break;
            }
            case 5: {
                if (!gAssigned) { std::cerr << "[!] Generate assignments first.\n"; break; }
                ReviewAssigner::writeOutput(gData, gAssignments);
                break;
            }
            case 0:
                std::cout << "Exiting..." << std::endl;
                break;
            default:
                std::cerr << "Error: invalid option." << std::endl;
        }
    } while (option != 0);
}

void batchMode(const std::string& inputFile, const std::string& outputFile) {
    std::cout << "Running in batch mode..." << std::endl;
    std::cout << "Input: " << inputFile << std::endl;

    gData = Parser::parse(inputFile);
    if (gData.submissions.empty() && gData.reviewers.empty()) {
        std::cerr << "[ERROR] Failed to load input file.\n";
        return;
    }
    if (!outputFile.empty())
        gData.params.outputFileName = outputFile;

    if (gData.params.generateAssignments != 0) {
        gAssignments = ReviewAssigner::assign(gData);
        ReviewAssigner::printSummary(gData, gAssignments);
        ReviewAssigner::writeOutput(gData, gAssignments);
    }

    if (gData.params.riskAnalysis == 1) {
        auto risky = ReviewAssigner::riskAnalysisK1(gData);
        ReviewAssigner::printRiskAnalysis(1, risky);
        ReviewAssigner::appendRiskAnalysisToFile(gData.params.outputFileName, 1, risky);
    } else if (gData.params.riskAnalysis > 1) {
        std::cout << "[INFO] Risk Analysis K=" << gData.params.riskAnalysis
                  << " (T2.3, not implemented).\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc >= 3 && std::string(argv[1]) == "-b") {
        std::string inputFile  = argv[2];
        std::string outputFile = (argc >= 4) ? argv[3] : "";
        batchMode(inputFile, outputFile);
        return 0;
    }
    interactiveMode();
    return 0;
}
