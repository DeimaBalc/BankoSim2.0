#ifndef AUTHSCREEN_H
#define AUTHSCREEN_H

#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <fstream> 
#include <cstdlib>
#include <ctime>
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

    Admin registerAdmin(int klientoSoketas){

        std::srand(std::time(0));
        int adminId = rand() % 90000 + 10000;

        Admin admin(std::to_string(adminId));

        std::string adminDir = "./admin/" + admin.getId() + ".txt";
        std::ofstream adminFile(adminDir); // Using std::ofstream
        if (!adminFile.is_open()) {
            std::cerr << "Nepavyko sukurti administratoriaus failo: " << adminDir << std::endl;
            return admin;
        }

        std::string password;

        std::string pranesimas = "Įveskite slaptažodį: ";
        send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
        char buffer[4096];
        ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            std::cerr << "Nepavyko gauti atsakymo iš kliento." << std::endl;
            return admin;
        }

        buffer[bytesRead] = '\0'; // Null-terminate the string

        password = buffer;

        adminFile << password; // Write the password to the file
        adminFile.close();

        std::string successMessage = "Administratoriaus ID: " + admin.getId() + "\n";
        send(klientoSoketas, successMessage.c_str(), successMessage.size(), 0);
        std::string passwordMessage = "Slaptažodis: " + password + "\n";
        send(klientoSoketas, passwordMessage.c_str(), passwordMessage.size(), 0);

        return admin;

    }

    //bool authUser(const User user);
    //User registerUser();
};

#endif // AUTHSCREEN_H