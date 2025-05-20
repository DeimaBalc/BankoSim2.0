#ifndef KLASES_H
#define KLASES_H

#include <string>
#include <vector>
#include <map>
#include <random>
#include <filesystem>
#include <fstream>
#include <ctime>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <algorithm>

class Tevas{
private:

    std::string vardas;
    std::string pavarde;
    std::string id;

public:

    Tevas(){};
    Tevas(const std::string& id): id(id) {}; // Constructor declaration

    std::string getId() const { return id; } // Gauti ID
    std::string getVardas() const { return vardas; } // Gauti vardą
    std::string getPavarde() const { return pavarde; } // Gauti pavardę

    void setVardas(std::string newVardas) {this->vardas = newVardas;}
    void setPavarde(std::string newPavarde) {this->pavarde = newPavarde;}
    void setId(std::string newId) {this->id = newId;}

    std::string to_string() const {

        return "Vardas ir pavardė: " + vardas + " " + pavarde + "\n" +
               "Prisijungimo ID: " + id;

    } // Gauti ID kaip string
   


};

class Vaikas{
private:

    std::string vardas;
    std::string pavarde;
    std::string id;

public:

    Vaikas(){};
    Vaikas(const std::string& id): id(id) {}; // Constructor declaration

    std::string getId() const { return id; } // Gauti ID
    std::string getVardas() const { return vardas; } // Gauti vardą
    std::string getPavarde() const { return pavarde; } // Gauti pavardę

    void setVardas(std::string newVardas) {this->vardas = newVardas;}
    void setPavarde(std::string newPavarde) {this->pavarde = newPavarde;}
    void setId(std::string newId) {this->id = newId;}

    std::string to_string() const {

        return "Vardas ir pavardė: " + vardas + " " + pavarde + "\n" +
               "Prisijungimo ID: " + id;

    } // Gauti ID kaip string



};


class bankoSas{};
class pridejimoEkr{};
class mokejimoKorteleEkr{};
class papildymoEkr{};
class pasalinimoEkr{};
class perziurosEkr{};
class veiklosEkr{};


class authScreen {

    public:

        authScreen(){};
        std::string pranesimas;

        bool authTevas(const Tevas tevas, int klientoSoketas) {
            char buffer[4096];

            if (!sendAndReceive(klientoSoketas, "Įveskite ID ", buffer, sizeof(buffer))) {
                return false;
            }
            std::string tevoId(buffer);

            if (!sendAndReceive(klientoSoketas, "Įveskite slaptažodį: ", buffer, sizeof(buffer))) {
                return false;
            }
            std::string atsakymas(buffer);

            std::string tevoDir = "./tevai/" + tevoId + "/asm_duom.txt";
            std::ifstream tevoFailas(tevoDir);

            if (!tevoFailas.is_open()) {
                send(klientoSoketas, "Nepavyko atidaryti failo.\n", 26, 0);
                return false;
            }

            std::string storedPassword;
            std::getline(tevoFailas, storedPassword);
            tevoFailas.close();

            if (atsakymas == storedPassword) {
                send(klientoSoketas, "Autentifikacija sėkminga.\n", 26, 0);
                return true;
            } else {
                send(klientoSoketas, "Neteisingas slaptažodis.\n", 26, 0);
                return false;
            }
        }

        Tevas registerTevas(int klientoSoketas) {
            
            srand(time(nullptr)); // Seed the random number generator
            
            char buffer[4096];
            Tevas tevas(std::to_string(rand() % 90000 + 10000));
            std::string tevoDir = "./tevai/" + tevas.getId();

            // Ensure the directory exists
            if (!std::filesystem::exists(tevoDir)) {
                if (!std::filesystem::create_directories(tevoDir)) {
                    send(klientoSoketas, "Nepavyko sukurti direktorijos.\n", 32, 0);
                    return tevas;
                }
            }

            std::string filePath = tevoDir + "/asm_duom.txt";
            std::ofstream tevoFailas(filePath);

            if (!tevoFailas.is_open()) {
                send(klientoSoketas, "Nepavyko sukurti failo.\n", 26, 0);
                return tevas;
            }

            if (!sendAndReceive(klientoSoketas, "Įveskite slaptažodį: ", buffer, sizeof(buffer))) {
                return tevas;
            }
            tevoFailas << buffer << "\n";

            if (!sendAndReceive(klientoSoketas, "Įveskite vardą: ", buffer, sizeof(buffer))) {
                return tevas;
            }
            tevoFailas << buffer << "\n";
            tevas.setVardas(buffer);

            if (!sendAndReceive(klientoSoketas, "Įveskite pavarde: ", buffer, sizeof(buffer))) {
                return tevas;
            }
            tevoFailas << buffer << "\n";
            tevas.setPavarde(buffer);

            tevoFailas.close();

            std::string successMessage = "Tėvo registracija sėkminga.\n";
            if (send(klientoSoketas, successMessage.c_str(), successMessage.size(), 0) < 0) {
                perror("Klaida siunčiant registracijos pranešimą");
            }

            send(klientoSoketas, ("Prisijungimo ID: " + tevas.getId() + "\n").c_str(), 26, 0);
            return tevas;
        }

    private:
        bool sendAndReceive(int soketas, const std::string& message, char* buffer, size_t bufferSize) {
            if (send(soketas, message.c_str(), message.size(), 0) < 0) {
                perror("Klaida siunčiant duomenis");
                return false;
            }
            ssize_t bytesRead = recv(soketas, buffer, bufferSize - 1, 0);
            if (bytesRead <= 0) {
                perror("Klaida gaunant duomenis");
                return false;
            }
            buffer[bytesRead] = '\0';
            return true;
        }
};

class tevuPrisijungimoEkr {
private:

    Tevas tevas;
    authScreen auth;

public:

    tevuPrisijungimoEkr(){};

    void interact(int klientoSoketas) {
        int pasirinkimas = 0;

        while (true) {
            std::string pranesimas = (
                "\n\n*** TĖVŲ PRISIJUNGIMAS ***\n\n"
                "1. Prisijungti\n"
                "2. Registruotis\n"
                "3. Atgal\n\n"
                "Pasirinkite veiksmą (1-3): "
            );

            if (!sendAndReceive(klientoSoketas, pranesimas, pasirinkimas)) {
                continue;
            }

            switch (pasirinkimas) {
                case 1: {
                    bool isAuthenticated = auth.authTevas(tevas, klientoSoketas);
                    send(klientoSoketas, isAuthenticated ? "Tėvo autentifikacija sėkminga.\n" : "Tėvo autentifikacija nesėkminga.\n", 30, 0);
                    if (!isAuthenticated) {
                        send(klientoSoketas, "Bandykite dar kartą.\n", 20, 0);
                        continue;
                    }
                    break;
                }
                case 2: {
                    tevas = auth.registerTevas(klientoSoketas);
                    if (tevas.getId().empty()) {
                        send(klientoSoketas, "Tėvo registracija nesėkminga. Bandykite dar kartą.\n", 40, 0);
                        continue;
                    }
                    send(klientoSoketas, "Tėvo registracija sėkminga.\n", 26, 0);
                    std::cout << tevas.to_string() << std::endl;
                    break;
                }
                case 3: {
                    send(klientoSoketas, "Grįžtama atgal į pagrindinį meniu.\n", 40, 0);
                    return; // Exit to the previous menu
                }
                default: {
                    send(klientoSoketas, "Neteisingas pasirinkimas. Bandykite dar kartą.\n", 42, 0);
                    continue;
                }
            }
        }
    }

private:
    bool sendAndReceive(int soketas, const std::string& message, int& pasirinkimas) {
        char buffer[4096];
        if (send(soketas, message.c_str(), message.size(), 0) < 0) {
            perror("Klaida siunčiant duomenis");
            return false;
        }
        ssize_t bytesRead = recv(soketas, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            perror("Klaida gaunant duomenis");
            return false;
        }
        buffer[bytesRead] = '\0';
        std::string atsakymas(buffer);

        if (atsakymas.empty() || !std::all_of(atsakymas.begin(), atsakymas.end(), ::isdigit) || 
            std::stoi(atsakymas) < 1 || std::stoi(atsakymas) > 3) {
            send(soketas, "Neteisingas pasirinkimas. Bandykite dar kartą.\n", 42, 0);
            return false;
        }

        pasirinkimas = std::stoi(atsakymas);
        return true;
    }
};

class vaikuPrisijungimoEkr {
private:
    Vaikas vaikas;
    authScreen auth;

public:

    vaikuPrisijungimoEkr(){};

    void interact(int klientoSoketas) {
        std::cout << "Placeholder: Vaikų prisijungimo ekranas dar nėra įgyvendintas.\n";
        std::string pranesimas = "Placeholder: Vaikų prisijungimo funkcionalumas dar kuriamas.\n";
        send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
        return;
    }
};

class userInterface {
private:

    tevuPrisijungimoEkr tevuEkranas;
    vaikuPrisijungimoEkr vaikuEkranas;

public:

    userInterface(){};

    void pradzia(int klientoSoketas){

        int pasirinkimas = 0;

        try {
            while (true) {
                std::string pranesimas = (
                    "\n\n*** VAIKŲ BANKAS ***\n\n"
                    "1. Tėvų prisijungimas\n"
                    "2. Vaikų prisijungimas\n"
                    "3. Uždaryti programą\n\n"
                    "Pasirinkite veiksmą (1-3): "
                );

                if (!sendAndReceive(klientoSoketas, pranesimas, pasirinkimas)) {
                    continue;
                }

                switch (pasirinkimas) {
                    case 1:
                        tevuEkranas.interact(klientoSoketas);
                        break;
                    case 2:
                        vaikuEkranas.interact(klientoSoketas);
                        break;
                    case 3:
                        send(klientoSoketas, "Programa uždaroma. Iki pasimatymo!", 36, 0);
                        close(klientoSoketas); // Close client socket
                        return;
                        //exit(0); // Terminate the program2
                    default:
                        send(klientoSoketas, "Neteisingas pasirinkimas. Bandykite dar kartą.\n", 45, 0);
                        continue;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Klaida tvarkant klientą: " << e.what() << std::endl;
            std::string klaida = "Įvyko klaida: " + std::string(e.what()) + "\n";
            send(klientoSoketas, klaida.c_str(), klaida.size(), 0);
        }

        close(klientoSoketas); // Close client socket after handling
    }

private:
    bool sendAndReceive(int soketas, const std::string& message, int& pasirinkimas) {
        char buffer[4096];
        if (send(soketas, message.c_str(), message.size(), 0) < 0) {
            perror("Klaida siunčiant duomenis");
            return false;
        }
        ssize_t bytesRead = recv(soketas, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            perror("Klaida gaunant duomenis");
            return false;
        }
        buffer[bytesRead] = '\0';
        std::string atsakymas(buffer);

        if (atsakymas.empty() || !std::all_of(atsakymas.begin(), atsakymas.end(), ::isdigit) || 
            std::stoi(atsakymas) < 1 || std::stoi(atsakymas) > 3) {
            send(soketas, "Neteisingas pasirinkimas. Bandykite dar kartą.\n", 42, 0);
            return false;
        }

        pasirinkimas = std::stoi(atsakymas);
        return true;
    }
};







#endif // KLASES_H