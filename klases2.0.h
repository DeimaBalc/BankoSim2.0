#ifndef KLASES2_H
#define KLASES2_H

#include <string>
#include <vector>
#include <map>
#include <random>
#include <filesystem>
#include <fstream>
#include <ctime>
#include <iostream>
#include <thread>
#include <algorithm>

class Tevas {
private:
    std::string vardas;
    std::string pavarde;
    std::string id;

public:
    Tevas() {};
    Tevas(const std::string& id) : id(id) {};

    std::string getId() const { return id; }
    std::string getVardas() const { return vardas; }
    std::string getPavarde() const { return pavarde; }

    void setVardas(std::string newVardas) { vardas = newVardas; }
    void setPavarde(std::string newPavarde) { pavarde = newPavarde; }
    void setId(std::string newId) { id = newId; }

    std::string to_string() const {
        return "Vardas ir pavardė: " + vardas + " " + pavarde + "\n" +
               "Prisijungimo ID: " + id;
    }
};

class Vaikas {
private:
    std::string vardas;
    std::string pavarde;
    std::string id;

public:
    Vaikas() {};
    Vaikas(const std::string& id) : id(id) {};

    std::string getId() const { return id; }
    std::string getVardas() const { return vardas; }
    std::string getPavarde() const { return pavarde; }

    void setVardas(std::string newVardas) { vardas = newVardas; }
    void setPavarde(std::string newPavarde) { pavarde = newPavarde; }
    void setId(std::string newId) { id = newId; }

    std::string to_string() const {
        return "Vardas ir pavardė: " + vardas + " " + pavarde + "\n" +
               "Prisijungimo ID: " + id;
    }
};

class bankoSas {
private:
    std::string saskaitosNumeris;
    double balansas;
    std::string vartotojas;

public:
    bankoSas(const std::string& vartotojas) : vartotojas(vartotojas), balansas(0.0) {
        saskaitosNumeris = "LT" + std::to_string(rand() % 9000000 + 1000000);
    }
    void ideti(double suma) { balansas += suma; }
    void isimti(double suma) { if (balansas >= suma) balansas -= suma; }
    void pervesti(bankoSas& kitaSaskaita, double suma) {
        if (balansas >= suma) {
            balansas -= suma;
            kitaSaskaita.ideti(suma);
        }
    }
    double gautiBalansa() const { return balansas; }
    std::string gautiSaskaitosNumeri() const { return saskaitosNumeris; }
    std::string gautiVartotoja() const { return vartotojas; }
    void setBalansa(double naujasBalansas) { balansas = naujasBalansas; }
    void irasytiIFaila() const {
        std::ofstream out("saskaita_" + vartotojas + ".txt");
        out << "Sąskaitos numeris: " << saskaitosNumeris << "\n";
        out << "Balansas: " << balansas << "\n";
        out << "Vartotojas: " << vartotojas << "\n";
        out.close();
    }
    friend std::ostream& operator<<(std::ostream& os, const bankoSas& saskaita) {
        os << "Sąskaitos numeris: " << saskaita.saskaitosNumeris << "\n";
        os << "Balansas: " << saskaita.balansas << "\n";
        os << "Vartotojas: " << saskaita.vartotojas << "\n";
        return os;
    }
};

class pridejimoEkr {};
class mokejimoKorteleEkr {};
class papildymoEkr {};
class pasalinimoEkr {};
class perziurosEkr {};
class veiklosEkr {};

class authScreen {
public:
    authScreen() {};
    std::string pranesimas;

    bool authTevas(Tevas& tevas) {
        std::string id, slaptazodis, failoSlaptazodis;
        std::cout << "Įveskite ID: ";
        std::cin >> id;
        std::cout << "Įveskite slaptažodį: ";
        std::cin >> slaptazodis;
        std::ifstream tevoFailas("./tevai/" + id + "/asm_duom.txt");
        if (!tevoFailas.is_open()) {
            std::cout << "Nepavyko atidaryti failo.\n";
            return false;
        }
        std::getline(tevoFailas, failoSlaptazodis);
        tevoFailas.close();
        if (slaptazodis == failoSlaptazodis) {
            std::cout << "Autentifikacija sėkminga.\n";
            tevas.setId(id);
            return true;
        } else {
            std::cout << "Neteisingas slaptažodis.\n";
            return false;
        }
    }

    Tevas registerTevas() {
        srand(time(nullptr));
        int tevoId = rand() % 90000 + 10000;
        Tevas tevas(std::to_string(tevoId));
        std::string tevoDir = "./tevai/" + tevas.getId();
        std::filesystem::create_directories(tevoDir);
        std::string filePath = tevoDir + "/asm_duom.txt";
        std::ofstream tevoFailas(filePath);
        if (!tevoFailas.is_open()) {
            std::cout << "Nepavyko sukurti failo.\n";
            return tevas;
        }
        std::string password, vardas, pavarde;
        std::cout << "Įveskite slaptažodį: ";
        std::cin >> password;
        tevoFailas << password << "\n";
        std::cout << "Įveskite vardą: ";
        std::cin >> vardas;
        tevoFailas << vardas << "\n";
        tevas.setVardas(vardas);
        std::cout << "Įveskite pavardę: ";
        std::cin >> pavarde;
        tevoFailas << pavarde << "\n";
        tevas.setPavarde(pavarde);
        tevoFailas.close();
        std::cout << "Tėvo registracija sėkminga.\n";
        std::cout << "Prisijungimo ID: " << tevas.getId() << "\n";
        return tevas;
    }
};

class tevuPrisijungimoEkr {
private:
    Tevas tevas;
    authScreen auth;

public:
    tevuPrisijungimoEkr() {};

    void interact() {
        int pasirinkimas = 0;
        while (true) {
            std::cout << "\n\n*** TĖVŲ PRISIJUNGIMAS ***\n\n"
                      << "1. Prisijungti\n"
                      << "2. Registruotis\n"
                      << "3. Atgal\n\n"
                      << "Pasirinkite veiksmą (1-3): ";
            std::cin >> pasirinkimas;
            if (std::cin.fail() || pasirinkimas < 1 || pasirinkimas > 3) {
                std::cin.clear();
                std::cin.ignore(4096, '\n');
                std::cout << "Neteisingas pasirinkimas. Bandykite dar kartą.\n";
                continue;
            }
            switch (pasirinkimas) {
                case 1: {
                    bool isAuthenticated = auth.authTevas(tevas);
                    if (!isAuthenticated) {
                        std::cout << "Bandykite dar kartą.\n";
                        continue;
                    }
                    int vaikoPasirinkimas = 0;
                    bool atgal = false;
                    while (!atgal) {
                        std::cout << "\n\n*** VAIKO BANKO VALDYMAS ***\n\n"
                                  << "1. Pridėti mokėjimo būdą\n"
                                  << "2. Pridėti vaiką\n"
                                  << "3. Pašalinti vaiką\n"
                                  << "4. Peržiūrėti vaikus\n"
                                  << "5. Papildyti sąskaitą\n"
                                  << "6. Užrakinti/atrakinti taupyklę\n"
                                  << "7. Atgal\n\n"
                                  << "Pasirinkite veiksmą (1-7): ";
                        std::cin >> vaikoPasirinkimas;
                        if (std::cin.fail() || vaikoPasirinkimas < 1 || vaikoPasirinkimas > 7) {
                            std::cin.clear();
                            std::cin.ignore(4096, '\n');
                            std::cout << "Neteisingas pasirinkimas. Bandykite dar kartą.\n";
                            continue;
                        }
                        switch (vaikoPasirinkimas) {
                            case 1:
                                // pridejimoEkr.interact();
                                break;
                            case 2:
                                // vaikuEkranas.interact();
                                break;
                            case 3:
                                // pasalinimoEkr.interact();
                                break;
                            case 4:
                                // perziurosEkr.interact();
                                break;
                            case 5:
                                // papildymoEkr.interact();
                                break;
                            case 6:
                                // veiklosEkr.interact();
                                break;
                            case 7:
                                std::cout << "Grįžtama atgal į tėvų meniu.\n";
                                atgal = true;
                                break;
                            default:
                                std::cout << "Neteisingas pasirinkimas. Bandykite dar kartą.\n";
                                break;
                        }
                    }
                    break;
                }
                case 2: {
                    tevas = auth.registerTevas();
                    std::cout << tevas.to_string() << std::endl;
                    break;
                }
                case 3:
                    std::cout << "Grįžtama atgal į pagrindinį meniu.\n";
                    return;
                default:
                    std::cout << "Neteisingas pasirinkimas. Bandykite dar kartą.\n";
                    break;
            }
        }
    }
};

class vaikuPrisijungimoEkr {
private:
    Vaikas vaikas;
    authScreen auth;

public:
    vaikuPrisijungimoEkr() {};

    void interact() {
        std::cout << "Placeholder: Vaikų prisijungimo ekranas dar nėra įgyvendintas.\n";
        std::cout << "Placeholder: Vaikų prisijungimo funkcionalumas dar kuriamas.\n";
    }
};

class userInterface {
private:
    tevuPrisijungimoEkr tevuEkranas;
    vaikuPrisijungimoEkr vaikuEkranas;

public:
    userInterface() {};

    void pradzia() {
        int pasirinkimas = 0;
        while (true) {
            std::cout << "\n\n*** VAIKŲ BANKAS ***\n\n"
                      << "1. Tėvų prisijungimas\n"
                      << "2. Vaikų prisijungimas\n"
                      << "3. Uždaryti programą\n\n"
                      << "Pasirinkite veiksmą (1-3): ";
            std::cin >> pasirinkimas;
            if (std::cin.fail() || pasirinkimas < 1 || pasirinkimas > 3) {
                std::cin.clear();
                std::cin.ignore(4096, '\n');
                std::cout << "Neteisingas pasirinkimas. Bandykite dar kartą.\n";
                continue;
            }
            switch (pasirinkimas) {
                case 1:
                    tevuEkranas.interact();
                    break;
                case 2:
                    vaikuEkranas.interact();
                    break;
                case 3:
                    std::cout << "Programa uždaroma. Iki pasimatymo!\n";
                    return;
                default:
                    std::cout << "Neteisingas pasirinkimas. Bandykite dar kartą.\n";
                    break;
            }
        }
    }
};

#endif // KLASES2_H
