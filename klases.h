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
#include <iostream> // Added for std::cout debug statements
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


class bankoSas{

    private:
    std::string saskaitosNumeris;
    double balansas;
    std::string vartotojas;

public:

    // Konstruktorius
    bankoSas(const std::string& vartotojas);
    // Funkcijos
    void ideti(double suma);
    void isimti(double suma);
    void pervesti(bankoSas& kitaSaskaita, double suma);
    double gautiBalansa() const;
    std::string gautiSaskaitosNumeri() const;
    std::string gautiVartotoja() const;

    void setBalansa(double naujasBalansas);

    void irasytiIFaila() const;

    friend std::ostream& operator<<(std::ostream& os, const bankoSas& saskaita);


};

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

            if (send(klientoSoketas, "Įveskite ID ", 13, 0) < 0) {
                perror("Klaida siunčiant duomenis");
                return false;
            }
            ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead <= 0) {
                perror("Klaida gaunant duomenis");
                return false;
            }
            buffer[bytesRead] = '\0';
            std::string tevoId(buffer);

            if (send(klientoSoketas, "Įveskite slaptažodį: ", 23, 0) < 0) {
                perror("Klaida siunčiant duomenis");
                return false;
            }
            bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead <= 0) {
                perror("Klaida gaunant duomenis");
                return false;
            }
            buffer[bytesRead] = '\0';
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
            srand(time(nullptr));
            char buffer[4096];
            Tevas tevas(std::to_string(rand() % 90000 + 10000));
            std::string tevoDir = "./tevai/" + tevas.getId();

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

            if (send(klientoSoketas, "Įveskite slaptažodį: ", 23, 0) < 0) {
                perror("Klaida siunčiant duomenis");
                return tevas;
            }
            ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead <= 0) {
                perror("Klaida gaunant duomenis");
                return tevas;
            }
            buffer[bytesRead] = '\0';
            tevoFailas << buffer << "\n";

            if (send(klientoSoketas, "Įveskite vardą: ", 17, 0) < 0) {
                perror("Klaida siunčiant duomenis");
                return tevas;
            }
            bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead <= 0) {
                perror("Klaida gaunant duomenis");
                return tevas;
            }
            buffer[bytesRead] = '\0';
            tevas.setVardas(buffer); // Set the name in the Tevas object
            tevoFailas << buffer << "\n"; // Write to file after setting in object

            if (send(klientoSoketas, "Įveskite pavarde: ", 19, 0) < 0) {
                perror("Klaida siunčiant duomenis");
                return tevas;
            }
            bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead <= 0) {
                perror("Klaida gaunant duomenis");
                return tevas;
            }
            buffer[bytesRead] = '\0';
            tevas.setPavarde(buffer); // Set the surname in the Tevas object
            tevoFailas << buffer << "\n"; // Write to file after setting in object

            tevoFailas.close();

            std::string successMessage = "Tėvo registracija sėkminga.\n";
            if (send(klientoSoketas, successMessage.c_str(), successMessage.size(), 0) < 0) {
                perror("Klaida siunčiant registracijos pranešimą");
            }

            send(klientoSoketas, ("Prisijungimo ID: " + tevas.getId() + "\n").c_str(), 26, 0);
            return tevas;
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
            std::cout << "DEBUG: Displaying TĖVŲ PRISIJUNGIMAS menu.\n"; // DEBUG
            pasirinkimas = 0; // Reset pasirinkimas each time to ensure menu is shown again
            std::string pranesimas = (
                "\n\n*** TĖVŲ PRISIJUNGIMAS ***\n\n"
                "1. Prisijungti\n"
                "2. Registruotis\n"
                "3. Atgal\n\n"
                "Pasirinkite veiksmą (1-3): "
            );

            char buffer[4096];
            if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
                perror("Klaida siunčiant duomenis");
                std::cout << "DEBUG: Failed to send TĖVŲ PRISIJUNGIMAS menu.\n"; // DEBUG
                continue;
            }
            std::cout << "DEBUG: Sent TĖVŲ PRISIJUNGIMAS menu. Waiting for input.\n"; // DEBUG
            ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead <= 0) {
                perror("Klaida gaunant duomenis");
                std::cout << "DEBUG: Client disconnected or recv error in TĖVŲ PRISIJUNGIMAS menu.\n"; // DEBUG
                return; // Exit if client disconnected
            }
            buffer[bytesRead] = '\0';
            std::string atsakymas(buffer);
            std::cout << "DEBUG: Received input for TĖVŲ PRISIJUNGIMAS menu: '" << atsakymas << "'\n"; // DEBUG

            if (atsakymas.empty() || !std::all_of(atsakymas.begin(), atsakymas.end(), ::isdigit) || 
                std::stoi(atsakymas) < 1 || std::stoi(atsakymas) > 3) {
                send(klientoSoketas, "Neteisingas pasirinkimas. Bandykite dar kartą.\n", 42, 0);
                std::cout << "DEBUG: Invalid input for TĖVŲ PRISIJUNGIMAS menu.\n"; // DEBUG
                continue;
            }

            pasirinkimas = std::stoi(atsakymas);
            std::cout << "DEBUG: Parsed TĖVŲ PRISIJUNGIMAS menu choice: " << pasirinkimas << "\n"; // DEBUG

            switch (pasirinkimas) {
                case 1: {
                    std::cout << "DEBUG: Entering authentication flow.\n"; // DEBUG
                    bool isAuthenticated = auth.authTevas(tevas, klientoSoketas);
                    // Removed redundant send here, as auth.authTevas already sends success/failure messages.
                    if (!isAuthenticated) {
                        send(klientoSoketas, "Bandykite dar kartą.\n", 20, 0);
                        std::cout << "DEBUG: Authentication failed. Looping back to TĖVŲ PRISIJUNGIMAS menu.\n"; // DEBUG
                        continue;
                    }
                    std::cout << "DEBUG: Authentication successful. Entering VAIKO BANKO VALDYMAS menu.\n"; // DEBUG
                    // Show child menu only if authenticated
                    int vaikoPasirinkimas = 0;
                    bool atgal = false;
                    while (!atgal) {
                        std::string pranesimas = (
                            "\n\n*** VAIKO BANKO VALDYMAS ***\n\n"
                            "1. Pridėti mokėjimo būdą\n"
                            "2. Pridėti vaiką\n"
                            "3. Pašalinti vaiką\n"
                            "4. Peržiūrėti vaikus\n"
                            "5. Papildyti sąskaitą\n"
                            "6. Užrakinti/atrakinti taupyklę\n"
                            "7. Atgal\n\n"
                            "Pasirinkite veiksmą (1-7): "
                        );
                        if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
                            perror("Klaida siunčiant duomenis");
                            std::cout << "DEBUG: Failed to send VAIKO BANKO VALDYMAS menu.\n"; // DEBUG
                            break;
                        }
                        std::cout << "DEBUG: Sent VAIKO BANKO VALDYMAS menu. Waiting for input.\n"; // DEBUG
                        ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
                        if (bytesRead <= 0) {
                            perror("Klaida gaunant duomenis");
                            std::cout << "DEBUG: Client disconnected or recv error in VAIKO BANKO VALDYMAS menu.\n"; // DEBUG
                            break;
                        }
                        buffer[bytesRead] = '\0';
                        std::string atsakymas(buffer);
                        std::cout << "DEBUG: Received input for VAIKO BANKO VALDYMAS menu: '" << atsakymas << "'\n"; // DEBUG
                        if (atsakymas.empty() || !std::all_of(atsakymas.begin(), atsakymas.end(), ::isdigit) ||
                            std::stoi(atsakymas) < 1 || std::stoi(atsakymas) > 7) {
                            send(klientoSoketas, "Neteisingas pasirinkimas. Bandykite dar kartą.\n", 42, 0);
                            std::cout << "DEBUG: Invalid input for VAIKO BANKO VALDYMAS menu.\n"; // DEBUG
                            continue;
                        }
                        vaikoPasirinkimas = std::stoi(atsakymas);
                        std::cout << "DEBUG: Parsed VAIKO BANKO VALDYMAS menu choice: " << vaikoPasirinkimas << "\n"; // DEBUG
                        switch (vaikoPasirinkimas) {
                            case 1:
                                // pridejimoEkr.interact(klientoSoketas); // Uncomment and implement when ready
                                send(klientoSoketas, "Funkcionalumas dar neįgyvendintas.\n", 36, 0);
                                break;
                            case 2:
                                // vaikuEkranas.interact(klientoSoketas); // Uncomment and implement when ready
                                send(klientoSoketas, "Funkcionalumas dar neįgyvendintas.\n", 36, 0);
                                break;
                            case 3:
                                // pasalinimoEkr.interact(klientoSoketas); // Uncomment and implement when ready
                                send(klientoSoketas, "Funkcionalumas dar neįgyvendintas.\n", 36, 0);
                                break;
                            case 4:
                                // perziurosEkr.interact(klientoSoketas); // Uncomment and implement when ready
                                send(klientoSoketas, "Funkcionalumas dar neįgyvendintas.\n", 36, 0);
                                break;
                            case 5:
                                // papildymoEkr.interact(klientoSoketas); // Uncomment and implement when ready
                                send(klientoSoketas, "Funkcionalumas dar neįgyvendintas.\n", 36, 0);
                                break;
                            case 6:
                                // veiklosEkr.interact(klientoSoketas); // Uncomment and implement when ready
                                send(klientoSoketas, "Funkcionalumas dar neįgyvendintas.\n", 36, 0);
                                break;
                            case 7:
                                send(klientoSoketas, "Grįžtama atgal į tėvų meniu.\n", 36, 0);
                                atgal = true; // This will break out of the inner while loop
                                std::cout << "DEBUG: Exiting VAIKO BANKO VALDYMAS menu.\n"; // DEBUG
                                break; // Break from switch to re-display parent menu
                        }
                    }
                    std::cout << "DEBUG: Returned from VAIKO BANKO VALDYMAS menu. Looping back to TĖVŲ PRISIJUNGIMAS menu.\n"; // DEBUG
                    break; // Break from outer switch (case 1) to re-display parent menu
                }
                case 2: {
                    std::cout << "DEBUG: Entering registration flow.\n"; // DEBUG
                    tevas = auth.registerTevas(klientoSoketas);
                    if (tevas.getId().empty()) {
                        send(klientoSoketas, "Tėvo registracija nesėkminga. Bandykite dar kartą.\n", 40, 0);
                        std::cout << "DEBUG: Registration failed. Looping back to TĖVŲ PRISIJUNGIMAS menu.\n"; // DEBUG
                        continue;
                    }
                    send(klientoSoketas, "Tėvo registracija sėkminga.\n", 26, 0);
                    std::cout << tevas.to_string() << std::endl;
                    std::cout << "DEBUG: Registration successful. Looping back to TĖVŲ PRISIJUNGIMAS menu.\n"; // DEBUG
                    break;
                }
                case 3: {
                    send(klientoSoketas, "Grįžtama atgal į pagrindinį meniu.\n", 40, 0);
                    std::cout << "DEBUG: Exiting TĖVŲ PRISIJUNGIMAS menu to main menu.\n"; // DEBUG
                    return; // Exit to the previous menu (userInterface::pradzia)
                }
                default: {
                    send(klientoSoketas, "Neteisingas pasirinkimas. Bandykite dar kartą.\n", 42, 0);
                    std::cout << "DEBUG: Invalid input for TĖVŲ PRISIJUNGIMAS menu (default case).\n"; // DEBUG
                    break;
                }
            }
        }

        return;

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
                std::cout << "DEBUG: Displaying VAIKŲ BANKAS main menu.\n"; // DEBUG
                pasirinkimas = 0; // Reset pasirinkimas each time to ensure menu is shown again

                // Print menu and handle input until a valid selection is made
                while (pasirinkimas == 0) {
                    std::string pranesimas = (
                        "\n\n*** VAIKŲ BANKAS ***\n\n"
                        "1. Tėvų prisijungimas\n"
                        "2. Vaikų prisijungimas\n"
                        "3. Uždaryti programą\n\n"
                        "Pasirinkite veiksmą (1-3): "
                    );

                    char buffer[4096];
                    if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
                        perror("Klaida siunčiant duomenis");
                        std::cout << "DEBUG: Failed to send VAIKŲ BANKAS main menu.\n"; // DEBUG
                        continue;
                    }
                    std::cout << "DEBUG: Sent VAIKŲ BANKAS main menu. Waiting for input.\n"; // DEBUG
                    ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
                    if (bytesRead <= 0) {
                        perror("Klaida gaunant duomenis");
                        std::cout << "DEBUG: Client disconnected or recv error in VAIKŲ BANKAS main menu.\n"; // DEBUG
                        break; // This break exits the inner while loop
                    }
                    buffer[bytesRead] = '\0';
                    std::string atsakymas(buffer);
                    std::cout << "DEBUG: Received input for VAIKŲ BANKAS main menu: '" << atsakymas << "'\n"; // DEBUG

                    if (atsakymas.empty() || !std::all_of(atsakymas.begin(), atsakymas.end(), ::isdigit) || 
                        std::stoi(atsakymas) < 1 || std::stoi(atsakymas) > 3) {
                        send(klientoSoketas, "Neteisingas pasirinkimas. Bandykite dar kartą.\n", 45, 0);
                        std::cout << "DEBUG: Invalid input for VAIKŲ BANKAS main menu.\n"; // DEBUG
                        continue;
                    }

                    pasirinkimas = std::stoi(atsakymas);
                    std::cout << "DEBUG: Parsed VAIKŲ BANKAS main menu choice: " << pasirinkimas << "\n"; // DEBUG
                }

                switch (pasirinkimas) {
                    case 1:
                        std::cout << "DEBUG: Calling tevuEkranas.interact().\n"; // DEBUG
                        tevuEkranas.interact(klientoSoketas);
                        std::cout << "DEBUG: Returned from tevuEkranas.interact(). Looping back to VAIKŲ BANKAS main menu.\n"; // DEBUG
                        break;
                    case 2:
                        std::cout << "DEBUG: Calling vaikuEkranas.interact().\n"; // DEBUG
                        vaikuEkranas.interact(klientoSoketas);
                        std::cout << "DEBUG: Returned from vaikuEkranas.interact(). Looping back to VAIKŲ BANKAS main menu.\n"; // DEBUG
                        break;
                    case 3:
                        send(klientoSoketas, "Programa uždaroma. Iki pasimatymo!", 36, 0);
                        std::cout << "DEBUG: Closing program.\n"; // DEBUG
                        close(klientoSoketas);
                        return;
                    default:    
                        send(klientoSoketas, "Neteisingas pasirinkimas. Bandykite dar kartą.\n", 45, 0);
                        std::cout << "DEBUG: Invalid input for VAIKŲ BANKAS main menu (default case).\n"; // DEBUG
                        break;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Klaida tvarkant klientą: " << e.what() << std::endl;
            std::string klaida = "Įvyko klaida: " + std::string(e.what()) + "\n";
            send(klientoSoketas, klaida.c_str(), klaida.size(), 0);
        }

        close(klientoSoketas);
    }

};

#endif // KLASES_H
