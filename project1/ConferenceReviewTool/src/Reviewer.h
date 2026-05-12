#ifndef REVIEWER_H
#define REVIEWER_H
 
#include <string>
#include <iostream>
 
/**
 * @brief Representa um reviewer da conferência com as suas áreas de expertise.
 */
struct Reviewer {
    int id;                  ///< Identificador único do reviewer
    std::string name;        ///< Nome do reviewer
    std::string email;       ///< Email do reviewer
    int primaryExpertise;    ///< Tópico primário de expertise
    int secondaryExpertise;  ///< Tópico secundário de expertise (0 se não definido)

    /**
     * @brief Constrói um Reviewer com os dados fornecidos.
     */
    Reviewer(int id, const std::string& name, const std::string& email,
             int primaryExpertise, int secondaryExpertise = 0)
        : id(id), name(name), email(email),
          primaryExpertise(primaryExpertise), secondaryExpertise(secondaryExpertise) {}

    /**
     * @brief Imprime os dados do reviewer no stdout.
     */
    void print() const {
        std::cout << "  [" << id << "] " << name
                  << " | Email: " << email
                  << " | Expertise: " << primaryExpertise;
        if (secondaryExpertise != 0)
            std::cout << "/" << secondaryExpertise;
        std::cout << "\n";
    }
};
 
#endif // REVIEWER_H
 