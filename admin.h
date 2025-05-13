#ifndef ADMIN_H
#define ADMIN_H

#include <string>
#include <vector>

class Admin {
private:
    std::string id; // Administratoriaus ID

public:
    Admin(); // Default constructor declaration
    Admin(const std::string& id): id(id) {}; // Constructor declaration

    std::string getId() const { return id; } // Gauti ID
};

#endif // ADMIN_H