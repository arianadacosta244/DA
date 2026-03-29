
#ifndef SUBMISSION_H
#define SUBMISSION_H
 
#include <string>
#include <iostream>
 
struct Submission {
    int id;                  
    std::string title;       
    std::string authors;     
    std::string email;       
    int primaryTopic;      
    int secondaryTopic;      
 
    
    Submission(int id, const std::string& title, const std::string& authors,
               const std::string& email, int primaryTopic, int secondaryTopic = 0)
        : id(id), title(title), authors(authors), email(email),
          primaryTopic(primaryTopic), secondaryTopic(secondaryTopic) {}
 
   
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
 