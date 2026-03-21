#ifndef REVIEWER_H
#define REVIEWER_H
 
#include <string>
#include <iostream>
 
struct Reviewer {
    int id;                 
    std::string name;       
    std::string email;     
    int primaryExpertise;    
    int secondaryExpertise;  
 
   
    Reviewer(int id, const std::string& name, const std::string& email,
             int primaryExpertise, int secondaryExpertise = 0)
        : id(id), name(name), email(email),
          primaryExpertise(primaryExpertise), secondaryExpertise(secondaryExpertise) {}
 
  
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
 