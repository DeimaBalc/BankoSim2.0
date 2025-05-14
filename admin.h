#ifndef ADMIN_H
#define ADMIN_H

#include <string>
#include <vector>
#include <iostream>

class Admin {
private:
    std::string id; // Administratoriaus ID

public:
    Admin(); // Default constructor declaration
    Admin(const std::string& id): id(id) {}; // Constructor declaration

    std::string getId() const { return id; } // Gauti ID

    void setId(std::string newId) {this->id = newId;}

    std::string to_string() const {

        return "Administratoriaus ID: " + id;

    } // Gauti ID kaip string
   

};

//std::ostream operator<<(std::ostream& os, const Admin admin);

#endif // ADMIN_H