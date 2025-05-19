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
class Vaikas{};
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

        bool authTevas(const Tevas tevas, int klientoSoketas) {

            std::string pranesimas = "Įveskite ID ";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);

            char buffer[4096];
            ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead <= 0) {
                std::cerr << "Nepavyko gauti atsakymo iš kliento." << std::endl;
                return false;
            }

            buffer[bytesRead] = '\0'; 
            std::string tevoId(buffer);

            std::cout << tevoId << std::endl;
            
            pranesimas = "Įveskite slaptažodį: ";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);

            buffer[4096];
            bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead <= 0) {
                std::cerr << "Nepavyko gauti atsakymo iš kliento." << std::endl;
                return false;
            }

            buffer[bytesRead] = '\0'; // Null-terminate the string
            std::string atsakymas(buffer);

            std::string tevoDir = "./tevai/" + tevoId + "/asm_duom.txt";
            std::ifstream tevoFailas(tevoDir); // Using std::ifstream

            if (!tevoFailas.is_open()) {
                std::cerr << "Nepavyko atidaryti Tevasistratoriaus failo: " << tevoDir << std::endl;
                return false;
            }

            std::string storedPassword;
            std::getline(tevoFailas, storedPassword); // Read the password from the file
            tevoFailas.close();

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

        Tevas registerTevas(int klientoSoketas){

            std::srand(std::time(0));
            int tevoId = rand() % 90000 + 10000;

            Tevas tevas(std::to_string(tevoId));

            std::string tevoDir = "./tevai/" + tevas.getId() + "/asm_duom.txt";
            std::ofstream tevoFailas(tevoDir); // Using std::ofstream
            if (!tevoFailas.is_open()) {
                std::cerr << "Nepavyko sukurti Tevo failo: " << tevoDir << std::endl;
                return tevas;
            }

            std::string password;
            std::string vardas;
            std::string pavarde;

            std::string pranesimas = "Įveskite slaptažodį: ";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            char buffer[4096];
            ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead <= 0) {
                std::cerr << "Nepavyko gauti atsakymo iš kliento." << std::endl;
                return tevas;
            }

            buffer[bytesRead] = '\0'; // Null-terminate the string
            password = buffer;
            tevoFailas << password; // Write the password to the file

            std::string pranesimas = "Įveskite vardą: ";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            char buffer[4096];
            ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead <= 0) {
                std::cerr << "Nepavyko gauti atsakymo iš kliento." << std::endl;
                return tevas;
            }

            buffer[bytesRead] = '\0'; // Null-terminate the string
            vardas = buffer;
            tevoFailas << vardas; // Write the password to the file

            std::string pranesimas = "Įveskite pavarde: ";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            char buffer[4096];
            ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead <= 0) {
                std::cerr << "Nepavyko gauti atsakymo iš kliento." << std::endl;
                return tevas;
            }

            buffer[bytesRead] = '\0'; // Null-terminate the string
            pavarde = buffer;
            tevoFailas << pavarde; // Write the password to the file


            tevoFailas.close();

            std::string successMessage = "Prisijungimo ID: " + tevas.getId() + "\n";
            send(klientoSoketas, successMessage.c_str(), successMessage.size(), 0);
            std::string passwordMessage = "Slaptažodis: " + password + "\n";
            send(klientoSoketas, passwordMessage.c_str(), passwordMessage.size(), 0);

            return tevas;

        }

        //bool authVaikas(const Vaikas vaikas);
        //Vaikas registerVaikas();


};
class tevuPrisujungimoEkr {
private:

    Tevas tevas;
    authScreen authScreen;

public:

    tevuPrisujungimoEkr(){};

    void interact(int klientoSoketas) {

        int pasirinkimas = 0;
        
        while (!pasirinkimas) {
                std::string pranesimas = (
                    "\n\n*** TĖVŲ PRISIJUNGIMAS ***\n\n"
                    "1. Prisijungti\n"
                    "2. Registruotis\n\n"
                    "Pasirinkite veiksmą (1-2)"
                );

                send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);

                char buffer[4096];
                ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
                if (bytesRead <= 0) {
                    throw std::runtime_error("Nepavyko gauti atsakymo iš kliento.");
                }
                
                //Gautas atsakymas tikrinamas
                buffer[bytesRead] = '\0'; // Null-terminate the string
                std::string atsakymas(buffer);

                // Check if the response is valid
                if (atsakymas.empty() || !std::all_of(atsakymas.begin(), atsakymas.end(), ::isdigit) || 
                    std::stoi(atsakymas) < 1 || std::stoi(atsakymas) > 2) {
                    std::string klaida = "Neteisingas pasirinkimas. Bandykite dar kartą.\n";
                    send(klientoSoketas, klaida.c_str(), klaida.size(), 0);
                    continue;
                }else {

                    pasirinkimas = std::stoi(atsakymas);

                }
                
            }

            switch (pasirinkimas)
            {
            case 1: {
                bool isAuthenticated = authScreen.authTevas(tevas, klientoSoketas);
                if (isAuthenticated) {
                    std::string successMessage = "Tėvo autentifikacija sėkminga.\n";
                    send(klientoSoketas, successMessage.c_str(), successMessage.size(), 0);
                } else {
                    std::string failureMessage = "Tėvo autentifikacija nesėkminga.\n";
                    send(klientoSoketas, failureMessage.c_str(), failureMessage.size(), 0);
                    return;
                }
                break;
            }

            case 2: {
                tevas = authScreen.registerTevas(klientoSoketas);
                std::cout << tevas.to_string() << std::endl;
                std::string successMessage = "Tėvo registracija sėkminga.\n";
                send(klientoSoketas, successMessage.c_str(), successMessage.size(), 0);
                break;
            }

            default:
                break;
            }

    }

};
class userInterface {
private:


    tevuPrisujungimoEkr tevuEkranas;
    vaikuPrisijungimoEkr vaikuEkranas;

public:

    userInterface(){};

    void pradzia(int klientoSoketas){

        int pasirinkimas = 0;
        bool atsijungti = false;

        try {
            //vartotojas renkasi ka daryti
            while (!pasirinkimas) {
                std::string pranesimas = (
                    "\n\n*** VAIKŲ BANKAS ***\n\n"
                    "1. Tėvų prisijungimas\n"
                    "2. Vaikų prisijungimas\n\n"
                    "Pasirinkite veiksmą (1-2)"
                );

                send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);

                char buffer[4096];
                ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
                if (bytesRead <= 0) {
                    throw std::runtime_error("Nepavyko gauti atsakymo iš kliento.");
                }
                
                //Gautas atsakymas tikrinamas
                buffer[bytesRead] = '\0'; // Null-terminate the string
                std::string atsakymas(buffer);

                // Check if the response is valid
                if (atsakymas.empty() || !std::all_of(atsakymas.begin(), atsakymas.end(), ::isdigit) || 
                    std::stoi(atsakymas) < 1 || std::stoi(atsakymas) > 2) {
                    std::string klaida = "Neteisingas pasirinkimas. Bandykite dar kartą.\n";
                    send(klientoSoketas, klaida.c_str(), klaida.size(), 0);
                    continue;
                }else {

                    pasirinkimas = std::stoi(atsakymas);

                }
                
            }

            switch (pasirinkimas)
            {
            case 1:
                tevuEkranas.interact(klientoSoketas);
                break;
            
            case 2: 
                vaikuEkranas.interact(klientoSoketas);
                break;
            default:
                break;
            }

            //realizuojami variantai
            if(pasirinkimas == 1 || pasirinkimas == 2){

                while(!atsijungti){

                    if(pasirinkimas == 1) {
                        // Prisijungti
                        std::string pranesimas = "Pasirinkote prisijungti.\n";
                        send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
                        // Handle login logic here
                    } else if (pasirinkimas == 2) {
                        // Registruotis
                        std::string pranesimas = "Pasirinkote registruotis.\n";
                        send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
                        // Handle registration logic here
                    } 

                }

            } else { 
                //jei pasirinktas adminas, paleidziamas kitas ekranas
                valdykAdmin(klientoSoketas);
                //sugrizus is admino, atsijungiama nuo soketo
                std::string pranesimas = "ATSIJUNGTA: Sėkmingai atsijungta. Iki pasimatymo!\n\n";
                send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            }
        

        } catch (const std::exception& e) {
            std::cerr << "Klaida tvarkant klientą: " << e.what() << std::endl;
            std::string klaida = "Įvyko klaida: " + std::string(e.what()) + "\n";
            send(klientoSoketas, klaida.c_str(), klaida.size(), 0);
        }

        close(klientoSoketas); // Close client socket after handling
    }



};







#endif // KLASES_H