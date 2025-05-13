#ifndef AUTHSCREEN_H
#define AUTHSCREEN_H

#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <fstream> // Added for std::ifstream
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "admin.h"
//#include <user.h>

class AuthScreen {    // Administratoriaus ID

public:

    AuthScreen(){};

    bool authAdmin(const Admin admin, int klientoSoketas) {
        std::string adminId = admin.getId();

        std::cout <<adminId << std::endl;
        
        std::string pranesimas = "Įveskite slaptažodį: ";
        send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);

        char buffer[4096];
        ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            std::cerr << "Nepavyko gauti atsakymo iš kliento." << std::endl;
            return false;
        }

        buffer[bytesRead] = '\0'; // Null-terminate the string
        std::string atsakymas(buffer);

        std::string adminDir = "./admin/" + adminId + ".txt";
        std::ifstream adminFile(adminDir); // Using std::ifstream

        if (!adminFile.is_open()) {
            std::cerr << "Nepavyko atidaryti administratoriaus failo: " << adminDir << std::endl;
            return false;
        }

        std::string storedPassword;
        std::getline(adminFile, storedPassword); // Read the password from the file
        adminFile.close();

        if (atsakymas == storedPassword) {
            std::string successMessage = "Autentifikacija sėkminga.\n";
            send(klientoSoketas, successMessage.c_str(), successMessage.size(), 0);
            return true;
        } else {
            std::string failureMessage = "Neteisingas slaptažodis.\n";
            send(klientoSoketas, failureMessage.c_str(), failureMessage.size(), 0);
            return false;
        }
    }

    //bool authUser(const User user);
};

#endif // AUTHSCREEN_H