#include <iostream>
#include <string>

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
        std::cin >> option;

        switch (option) {
            case 1:
                std::cout << "Enter filename: ";
                // TODO: chamar a função de parsing
                break;
            case 2:
                // TODO: chamar generateAssignments()
                break;
            case 3:
                // TODO: chamar riskAnalysis()
                break;
            case 4:
                // TODO: mostrar resultados
                break;
            case 5:
                // TODO: exportar ficheiro
                break;
            case 0:
                std::cout << "Exiting..." << std::endl;
                break;
            default:
                std::cerr << "Error: invalid option." << std::endl;
        }
    } while (option != 0);
}

void batchMode(const std::string& inputFile, const std::string& riskFile) {
    std::cout << "Running in batch mode..." << std::endl;
    std::cout << "Input: " << inputFile << std::endl;
    // TODO: carregar ficheiro, gerar assignments, risk analysis, exportar
}

int main(int argc, char* argv[]) {
    // Batch mode: ./myProg -b input.csv risk.csv
    //pode nao ter risk.csv
    if (argc >= 3 && std::string(argv[1]) == "-b") {
        std::string inputFile = argv[2];  //atribuir ficheiro de input
        std::string riskFile  = (argc >= 4) ? argv[3] : "";  //atribuir riskFile
        batchMode(inputFile, riskFile);  //executar
        return 0;
    }

    // Interactive mode
    interactiveMode();
    return 0;

}    