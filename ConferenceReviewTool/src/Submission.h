
#ifndef SUBMISSION_H
#define SUBMISSION_H

#include <string>
#include <iostream>

/**
 * @brief Representa uma submissão (artigo) da conferência.
 */
struct Submission {
    int id;                  ///< Identificador único da submissão
    std::string title;       ///< Título do artigo
    std::string authors;     ///< Autores do artigo
    std::string email;       ///< Email de contacto
    int primaryTopic;        ///< Tópico primário da submissão
    int secondaryTopic;      ///< Tópico secundário da submissão (0 se não definido)

    /**
     * @brief Constrói uma Submission com os dados fornecidos.
     * @param id Identificador único
     * @param title Título do artigo
     * @param authors Autores do artigo
     * @param email Email de contacto
     * @param primaryTopic Tópico primário
     * @param secondaryTopic Tópico secundário (opcional, default 0)
     */
    Submission(int id, const std::string& title, const std::string& authors,
               const std::string& email, int primaryTopic, int secondaryTopic = 0)
        : id(id), title(title), authors(authors), email(email),
          primaryTopic(primaryTopic), secondaryTopic(secondaryTopic) {}

    /**
     * @brief Imprime os dados da submissão no stdout.
     */
    void print() const {
        std::cout << "  [" << id << "] " << title
                  << " | Authors: " << authors
                  << " | Topic: " << primaryTopic;
        if (secondaryTopic != 0)
            std::cout << "/" << secondaryTopic;
        std::cout << "\n";
    }
};

#endif // SUBMISSION_H
