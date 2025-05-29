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

// Inherit from Tevas
class Vaikas : public Tevas {
private:
    std::string bankoSaskaitosNumeris;

public:

    Vaikas() : Tevas() {}
    Vaikas(const std::string& id) : Tevas(id) {}

    void setBankoSas(const std::string& saskaitosNumeris) {
        this->bankoSaskaitosNumeris = saskaitosNumeris;
    }

    std::string getBankoSas() const {
        return bankoSaskaitosNumeris;
    }
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

class pridejimoEkr {
public:
    pridejimoEkr() {}
    Vaikas interact(const Tevas tevas, int klientoSoketas) {
        srand(time(nullptr));
        std::string pranesimas;
        char buffer[4096];
        Vaikas vaikas(std::to_string(rand() % 90000 + 10000));
        pranesimas = "Įveskite vaiko vardą: ";
        if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) { perror("Klaida siunčiant duomenis"); return vaikas; }
        ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) { perror("Klaida gaunant duomenis"); return vaikas; }
        buffer[bytesRead] = '\0';
        std::string vaikoVardas(buffer);
        vaikas.setVardas(vaikoVardas);
        pranesimas = "Įveskite vaiko pavardę: ";
        if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) { perror("Klaida siunčiant duomenis"); return vaikas; }
        bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) { perror("Klaida gaunant duomenis"); return vaikas; }
        buffer[bytesRead] = '\0';
        std::string vaikoPavarde(buffer);
        vaikas.setPavarde(vaikoPavarde);
        pranesimas = "Sukurkite vaiko slaptažodį: ";
        if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) { perror("Klaida siunčiant duomenis"); return vaikas; }
        bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) { perror("Klaida gaunant duomenis"); return vaikas; }
        buffer[bytesRead] = '\0';
        std::string vaikosSlaptazodis(buffer);
        std::string tevoDir = "./tevai/" + tevas.getId() + "/vaikai/" + vaikas.getId();
        std::string vaikoDir = "./vaikai/" + vaikas.getId();
        if (std::filesystem::exists(tevoDir)) {
            pranesimas = "Vaikas su tokiu ID jau egzistuoja.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return vaikas;
        }
        if (!std::filesystem::exists(tevoDir)) {
            if (!std::filesystem::create_directories(tevoDir)) {
                send(klientoSoketas, "Nepavyko sukurti tėvo vaikų aplanko.\n", 36, 0);
                return vaikas;
            }
        }
        if (std::filesystem::exists(vaikoDir)) {
            pranesimas = "Vaikas su tokiu ID jau egzistuoja.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return vaikas;
        }
        if (!std::filesystem::exists(vaikoDir)) {
            if (!std::filesystem::create_directories(vaikoDir)) {
                send(klientoSoketas, "Nepavyko sukurti vaiko aplanko.\n", 30, 0);
                return vaikas;
            }
        }
        std::string salies_kodas = "LT";
        std::string kontrolinis_skaicius = std::to_string(rand() % 9000 + 1000);
        std::string banko_kodas = "7300";
        std::string unikalus_numeris = std::to_string(rand() % 90000000 + 10000000);
        std::string bankoSaskaitosNumeris = salies_kodas + kontrolinis_skaicius + banko_kodas + unikalus_numeris;
        vaikas.setBankoSas(bankoSaskaitosNumeris);
        std::ofstream vaikoFailPasTeva(tevoDir + "/asm_duom.txt");
        if (!vaikoFailPasTeva.is_open()) {
            send(klientoSoketas, "Nepavyko sukurti asm_duom.txt faile.\n", 38, 0);
            return vaikas;
        }
        vaikoFailPasTeva << vaikosSlaptazodis << "\n";
        vaikoFailPasTeva << vaikoVardas << "\n";
        vaikoFailPasTeva << vaikoPavarde << "\n";
        vaikoFailPasTeva << tevas.getId() << "\n";
        vaikoFailPasTeva.close();
        std::ofstream vaikoFailPasVaika(vaikoDir + "/asm_duom.txt");
        std::ofstream vaikoSasPasVaika(vaikoDir + "/" + vaikas.getBankoSas() + ".txt");
        if (!vaikoFailPasVaika.is_open()) {
            send(klientoSoketas, "Nepavyko sukurti asm_duom.txt faile.\n", 38, 0);
            return vaikas;
        }
        if (!vaikoSasPasVaika.is_open()) {
            send(klientoSoketas, "Nepavyko sukurti banko sąskaitos faile.\n", 42, 0);
            return vaikas;
        }
        vaikoFailPasVaika << vaikosSlaptazodis << "\n";
        vaikoFailPasVaika << vaikoVardas << "\n";
        vaikoFailPasVaika << vaikoPavarde << "\n";
        vaikoFailPasVaika << tevas.getId() << "\n";
        vaikoSasPasVaika << "0\n";
        vaikoFailPasVaika.close();
        vaikoSasPasVaika.close();
        std::string successMessage = "Vaiko registracija sėkminga!\n";
        send(klientoSoketas, successMessage.c_str(), successMessage.size(), 0);
        std::string infoMsg = "Vaiko prisijungimo ID: " + vaikas.getId() + "\nVaiko banko sąskaita: " + vaikas.getBankoSas() + "\n";
        send(klientoSoketas, infoMsg.c_str(), infoMsg.size(), 0);
        return vaikas;
    }
};
class mokejimoKorteleEkr{

public:

    mokejimoKorteleEkr(){};
    std::string pranesimas;

    void interact(const Tevas tevas, int klientoSoketas) {
        
        
        std::string pranesimas;
        char buffer[4096];

        std::string tevoDir = "./tevai/" + tevas.getId();

        if (!std::filesystem::exists(tevoDir)) {
            if (!std::filesystem::create_directories(tevoDir)) {
                send(klientoSoketas, "Nepavyko sukurti direktorijos.\n", 32, 0);
                return;
            }
        }

        

        pranesimas = "Įveskite kortelės numerį: ";
        if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
            perror("Klaida siunčiant duomenis");
            return;
        }

        ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            perror("Klaida gaunant duomenis");
            return;
        }
        buffer[bytesRead] = '\0';
        std::string kortelesNumeris(buffer);

        // Check if the first two characters are "LT"
        if (kortelesNumeris.size() != 18 || kortelesNumeris.substr(0, 2) != "LT") {
            pranesimas = "Neteisingas numerio formatas (LT0000000000000000)\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }

        std::string filePath = tevoDir + "/" + kortelesNumeris + ".txt";

        // Check if file with the same name already exists
        if (std::filesystem::exists(filePath)) {
            pranesimas = "Failas su tokiu kortelės numeriu jau egzistuoja.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }

        std::ofstream tevoFailas(filePath);

        if (!tevoFailas.is_open()) {
            send(klientoSoketas, "Nepavyko sukurti failo.\n", 26, 0);
            return;
        }

        pranesimas = "Įveskite likutį: ";
        if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
            perror("Klaida siunčiant duomenis");
            return;
        }
        // Remove duplicate send
        // send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);

        bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            perror("Klaida gaunant duomenis");
            return;
        }
        buffer[bytesRead] = '\0';
        tevoFailas << buffer << "\n";
        tevoFailas.close();

        // Check if file is empty using std::filesystem::file_size with a path, not a stream
        if (std::filesystem::file_size(filePath) == 0) {
            send(klientoSoketas, "Failas yra tusčias\n", 26, 0);
            return;
        }else{
            pranesimas = "Kortelė sėkmingai pridėta.\n";
            if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
                perror("Klaida siunčiant duomenis");
            }

            if (send(klientoSoketas, ("Kortelės numeris: " + kortelesNumeris + "\n" 
                                    + "Likutis: " + std::string(buffer) + "\n").c_str(), 50, 0) < 0) {
            perror("Klaida siunčiant duomenis");
            return;
            }

        }

        return;
    }



};
class papildymoEkr{
public:

    papildymoEkr(){};

    // Update balances in both parent and child global directories only
    void updateBalansas(const std::string& tevoSasPath, double naujasTevoBalansas,
                        const std::string& vaikoGlobalSasPath, double naujasVaikoBalansas) {
        // Update parent's balance
        std::ofstream tevoSasFile(tevoSasPath, std::ios::trunc);
        if (tevoSasFile.is_open()) {
            tevoSasFile << std::fixed << std::setprecision(2) << naujasTevoBalansas << "\n";
            tevoSasFile.close();
        } 
        // Update child's balance in ./vaikai
        std::ofstream vaikoGlobalSasFile(vaikoGlobalSasPath, std::ios::trunc);
        if (vaikoGlobalSasFile.is_open()) {
            vaikoGlobalSasFile << std::fixed << std::setprecision(2) << naujasVaikoBalansas << "\n";
            vaikoGlobalSasFile.close();
        }
    }

    void interact(const Tevas tevas, int klientoSoketas) {
        std::string pranesimas;
        char buffer[4096];
        ssize_t bytesRead;

        pranesimas = "Įveskite vaiko ID: ";
        if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
            perror("Klaida siunčiant duomenis");
            return;
        }
        bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            perror("Klaida gaunant duomenis");
            return;
        }
        buffer[bytesRead] = '\0';
        std::string vaikoID(buffer);

        std::string vaikoGlobalDir = "./vaikai/" + vaikoID;
        if (!std::filesystem::exists(vaikoGlobalDir)) {
            pranesimas = "Vaikas su tokiu ID nerastas.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }

        std::string vaikoBankoSaskaitosNumeris = "Nerasta";
        std::string vaikoBalansasValue;
        bool vaikoBankoFileRastas = false;
        std::string vaikoGlobalSasPath;
        // Find child's account file and read balance from the global vaikai directory
        if (std::filesystem::exists(vaikoGlobalDir) && std::filesystem::is_directory(vaikoGlobalDir)) {
            for (const auto& fileEntry : std::filesystem::directory_iterator(vaikoGlobalDir)) {
                if (!fileEntry.is_regular_file()) continue;
                std::string filename = fileEntry.path().filename().string();
                if (filename.find("LT") != std::string::npos && filename.length() > 4 && filename.rfind(".txt") == filename.length() - 4) {
                    vaikoBankoSaskaitosNumeris = filename.substr(0, filename.length() - 4);
                    vaikoGlobalSasPath = fileEntry.path().string();
                    std::ifstream saskaitaFile(vaikoGlobalSasPath);
                    if (saskaitaFile.is_open()) {
                        if (std::getline(saskaitaFile, vaikoBalansasValue) && !vaikoBalansasValue.empty()) {
                            // Balance read successfully
                        } else {
                            vaikoBalansasValue = "0.00";
                        }
                        saskaitaFile.close();
                        vaikoBankoFileRastas = true;
                        break;
                    }
                }
            }
        }
        if (!vaikoBankoFileRastas) {
            pranesimas = "Vaiko banko sąskaita nerasta arba nepavyko jos atidaryti.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }

        pranesimas = "Įveskite suma, kurią norite pridėti: ";
        if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
            perror("Klaida siunčiant duomenis");
            return;
        }
        bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            perror("Klaida gaunant duomenis");
            return;
        }
        buffer[bytesRead] = '\0';
        std::string sumaValue(buffer);
        double suma;
        try {
            suma = std::stod(sumaValue);
        } catch (const std::invalid_argument& e) {
            pranesimas = "Neteisingas sumos formatas.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        } catch (const std::out_of_range& e) {
            pranesimas = "Suma per didelė arba per maža.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }
        if (suma <= 0) {
            pranesimas = "Negalima pridėti neigiamos arba nulio sumos.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }

        // Find parent's account file and read parent's balance
        std::string tevoRootDir = "./tevai/" + tevas.getId();
        std::string tevoSaskaitosNumeris = "Nerasta";
        std::string tevoBalansasValue;
        bool tevoBankoFileRastas = false;
        std::string tevoSasPath;
        if (std::filesystem::exists(tevoRootDir) && std::filesystem::is_directory(tevoRootDir)) {
            for (const auto& fileEntry : std::filesystem::directory_iterator(tevoRootDir)) {
                if (!fileEntry.is_regular_file()) continue;
                std::string filename = fileEntry.path().filename().string();
                if (filename.find("LT") != std::string::npos && filename.length() > 4 && filename.rfind(".txt") == filename.length() - 4) {
                    tevoSaskaitosNumeris = filename.substr(0, filename.length() - 4);
                    tevoSasPath = fileEntry.path().string();
                    std::ifstream saskaitaFile(tevoSasPath);
                    if (saskaitaFile.is_open()) {
                        if (std::getline(saskaitaFile, tevoBalansasValue) && !tevoBalansasValue.empty()) {
                            // Balance read successfully
                        } else {
                            tevoBalansasValue = "0.00";
                        }
                        saskaitaFile.close();
                        tevoBankoFileRastas = true;
                        break;
                    }
                }
            }
        }
        if (!tevoBankoFileRastas) {
            pranesimas = "Tėvo banko sąskaita nerasta arba nepavyko jos atidaryti.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }
        double tevoBalansas;
        try {
            tevoBalansas = std::stod(tevoBalansasValue);
        } catch (const std::invalid_argument& e) {
            pranesimas = "Klaida skaitant tėvo balansą.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        } catch (const std::out_of_range& e) {
            pranesimas = "Tėvo balansas per didelis arba per maža.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }
        if (tevoBalansas < suma) {
            pranesimas = "Nepakanka lėšų tėvo sąskaitoje.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }
        double vaikoBalansas;
        try {
            vaikoBalansas = std::stod(vaikoBalansasValue);
        } catch (const std::invalid_argument& e) {
            pranesimas = "Klaida skaitant vaiko balansą.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        } catch (const std::out_of_range& e) {
            pranesimas = "Vaiko balansas per didelis arba per maža.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }
        double naujasVaikoBalansas = vaikoBalansas + suma;
        double naujasTevoBalansas = tevoBalansas - suma;
        // Call the update function (no parent's child dir update)
        updateBalansas(tevoSasPath, naujasTevoBalansas, vaikoGlobalSasPath, naujasVaikoBalansas);
        pranesimas = "Suma sėkmingai pridėta vaikui. Naujas vaiko balansas: " + std::to_string(naujasVaikoBalansas) + ", naujas tėvo balansas: " + std::to_string(naujasTevoBalansas) + "\n";
        send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
        return;
    }
};
class pasalinimoEkr{

public:

    pasalinimoEkr(){};

    void interact(const Tevas tevas, int klientoSoketas) {
        // Placeholder for the pridejimoEkr functionality
       
        
        std::string pranesimas;
        char buffer[4096];

        pranesimas = "Įveskite vaiko id: ";
        if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
            perror("Klaida siunčiant duomenis");
            return;
        }

        ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            perror("Klaida gaunant duomenis");
            return;
        }
        buffer[bytesRead] = '\0';
        std::string vaikoID(buffer);

        


        std::string tevoDir = "./tevai/" + tevas.getId() + "/vaikai";
        std::string vaikoDir = "./vaikai/" + vaikoID;

        

        std::string filePathPasTeva = tevoDir + "/" + vaikoID;

        std::filesystem::remove_all (filePathPasTeva); // Remove the file for the parent
        std::filesystem::remove_all (vaikoDir); // Remove the file for the child
        


        std::string successMessage = "Vaikas pašalintas sėkmingai.\n";
            if (send(klientoSoketas, successMessage.c_str(), successMessage.size(), 0) < 0) {
                perror("Klaida siunčiant ištrinimo pranešimą");
            }


        return;
    }
};
class perziurosEkr {
public:
    perziurosEkr() {}

    void interact(const Tevas& tevas, int klientoSoketas) {
        std::string tevoVaikaiDir = "./tevai/" + tevas.getId() + "/vaikai";
        std::string pranesimas;

        if (!std::filesystem::exists(tevoVaikaiDir) || !std::filesystem::is_directory(tevoVaikaiDir)) {
            pranesimas = "Tėvo vaikų aplankas nerastas arba tai ne aplankas.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }

        bool foundChildren = false;
        for (const auto& vaikoDirEntry : std::filesystem::directory_iterator(tevoVaikaiDir)) {
            if (!vaikoDirEntry.is_directory()) {
                continue;
            }
            std::string vaikoId = vaikoDirEntry.path().filename().string();
            std::string vaikoGlobalDir = "./vaikai/" + vaikoId;

            // Read personal data from asm_duom.txt in global vaikai dir
            std::string asmDuomPath = vaikoGlobalDir + "/asm_duom.txt";
            std::ifstream asmDuomFile(asmDuomPath);
            std::string vardas = "Nenustatytas";
            std::string pavarde = "Nenustatyta";
            if (asmDuomFile.is_open()) {
                std::string tempSlaptazodis;
                std::getline(asmDuomFile, tempSlaptazodis);
                std::getline(asmDuomFile, vardas);
                std::getline(asmDuomFile, pavarde);
                asmDuomFile.close();
            }

            // Find bank account file in global vaikai dir
            std::string bankoSaskaitosNumeris = "Nerasta";
            std::string balansasText = "Balansas nerastas";
            bool bankoFileRastas = false;
            if (std::filesystem::exists(vaikoGlobalDir) && std::filesystem::is_directory(vaikoGlobalDir)) {
                for (const auto& fileEntry : std::filesystem::directory_iterator(vaikoGlobalDir)) {
                    if (!fileEntry.is_regular_file()) continue;
                    std::string filename = fileEntry.path().filename().string();
                    if (filename.find("LT") != std::string::npos && filename.length() > 4 && filename.rfind(".txt") == filename.length() - 4) {
                        bankoSaskaitosNumeris = filename.substr(0, filename.length() - 4);
                        std::ifstream saskaitaFile(fileEntry.path().string());
                        if (saskaitaFile.is_open()) {
                            std::string balansasValue;
                            if (std::getline(saskaitaFile, balansasValue) && !balansasValue.empty()) {
                                balansasText = "Balansas: " + balansasValue;
                            } else {
                                balansasText = "Balansas: 0.00 (arba failas tuščias)";
                            }
                            saskaitaFile.close();
                        } else {
                            balansasText = "Nepavyko atidaryti banko sąskaitos failo.";
                        }
                        bankoFileRastas = true;
                        break;
                    }
                }
            }
            if (!bankoFileRastas && bankoSaskaitosNumeris == "Nerasta") {
                balansasText = "Banko sąskaita nerasta.";
            }

            // Taupyklė info (new structure: first line = status, second line = balance)
            std::string taupykleInfo = "Taupyklė: nėra informacijos";
            std::string taupyklePath = vaikoGlobalDir + "/taupyklė.txt";
            std::ifstream taupykleFile(taupyklePath);
            if (taupykleFile.is_open()) {
                std::string taupykleStatus, taupykleBalansas;
                if (std::getline(taupykleFile, taupykleStatus)) {
                    if (std::getline(taupykleFile, taupykleBalansas)) {
                        if (taupykleStatus == "0") {
                            taupykleInfo = "Taupyklė: užrakinta, balansas: " + taupykleBalansas;
                        } else if (taupykleStatus == "1") {
                            taupykleInfo = "Taupyklė: atrakinta, balansas: " + taupykleBalansas;
                        } else {
                            taupykleInfo = "Taupyklė: būsena nežinoma, balansas: " + taupykleBalansas;
                        }
                    } else {
                        taupykleInfo = "Taupyklė: klaida skaitant balansą";
                    }
                }
                taupykleFile.close();
            }

            pranesimas += "Vaiko ID: " + vaikoId + "\n";
            pranesimas += "Vardas: " + vardas + "\n";
            pranesimas += "Pavardė: " + pavarde + "\n";
            pranesimas += "Banko sąskaitos numeris: " + bankoSaskaitosNumeris + "\n";
            pranesimas += balansasText + "\n";
            pranesimas += taupykleInfo + "\n\n";
            foundChildren = true;
        }

        if (!foundChildren) {
            pranesimas = "Šis tėvas vaikų neturi arba nepavyko nuskaityti jų duomenų.\n";
        }
        send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
    }
};
class veiklosPerziurosEkr {
public:
    veiklosPerziurosEkr() {};

    void interact(const Tevas& tevas, int klientoSoketas) {
        std::string pranesimas;
        char buffer[4096];
        ssize_t bytesRead;

        pranesimas = "Įveskite vaiko ID, kurio veiklą norite peržiūrėti: ";
        if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
            perror("Klaida siunčiant duomenis");
            return;
        }
        bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            perror("Klaida gaunant duomenis");
            return;
        }
        buffer[bytesRead] = '\0';
        std::string vaikoID(buffer);

        std::string veiklaPath = "./tevai/" + tevas.getId() + "/vaikai/" + vaikoID + "/veikla.txt";
        std::ifstream veiklaFile(veiklaPath);
        if (!veiklaFile.is_open()) {
            pranesimas = "Veiklos failas nerastas arba tuščias.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }
        std::string line, veiklaContent;
        while (std::getline(veiklaFile, line)) {
            veiklaContent += line + "\n";
        }
        veiklaFile.close();
        if (veiklaContent.empty()) {
            veiklaContent = "Veiklos įrašų nėra.\n";
        }
        send(klientoSoketas, veiklaContent.c_str(), veiklaContent.size(), 0);
    }
};
class authScreen {

    public:

        authScreen(){};
        std::string pranesimas;

        bool authVaikas(Vaikas& vaikas, int klientoSoketas) {
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
            std::string vaikoId(buffer);
            vaikas.setId(vaikoId); // Set the ID in the Vaikas object

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

            std::string vaikoDir = "./vaikai/" + vaikoId + "/asm_duom.txt";
            std::ifstream vaikoFailas(vaikoDir);

            if (!vaikoFailas.is_open()) {
                send(klientoSoketas, "Nepavyko atidaryti failo.\n", 26, 0);
                return false;
            }

            std::string storedPassword;
            std::getline(vaikoFailas, storedPassword);
            vaikoFailas.close();

            if (atsakymas == storedPassword) {
                send(klientoSoketas, "Autentifikacija sėkminga.\n", 26, 0);
                return true;
            } else {
                send(klientoSoketas, "Neteisingas slaptažodis.\n", 26, 0);
                return false;
            }
        }

        bool authTevas(Tevas& tevas, int klientoSoketas) {
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
            tevas.setId(tevoId); // Set the ID in the Tevas object

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
class taupyklesEkr{
public:

    taupyklesEkr(){};

    void uzrakintiAtrakinti(const Tevas tevas, int klientoSoketas) {
        std::string pranesimas;
        char buffer[4096];
        ssize_t bytesRead;

        pranesimas = "Įveskite vaiko ID: ";
        if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
            perror("Klaida siunčiant duomenis");
            return;
        }
        bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            perror("Klaida gaunant duomenis");
            return;
        }
        buffer[bytesRead] = '\0';
        std::string vaikoID(buffer);

        std::string vaikoDir = "./vaikai/" + vaikoID;
        std::string taupyklėsFailas = vaikoDir + "/taupyklė.txt";

        if (!std::filesystem::exists(taupyklėsFailas)) {
            pranesimas = "Taupyklės failas nerastas.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }

        std::ifstream taupyklėsFile(taupyklėsFailas);
        if (!taupyklėsFile.is_open()) {
            pranesimas = "Nepavyko atidaryti taupyklės failo.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }

        std::string status, balansas;
        if (std::getline(taupyklėsFile, status) && std::getline(taupyklėsFile, balansas)) {
            taupyklėsFile.close();
            if (status == "0") {
                status = "1"; // Atrakinti
                pranesimas = "Taupyklė buvo užrakinta. Dabar ji bus atrakinta.\n";
            } else if (status == "1") {
                status = "0"; // Užrakinti
                pranesimas = "Taupyklė buvo atrakinta. Dabar ji bus užrakinta.\n";
            } else {
                pranesimas = "Taupyklės būsena nežinoma.\n";
                send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
                return;
            }
            // Write back the new status
            std::ofstream taupyklėsFileOut(taupyklėsFailas, std::ios::trunc);
            if (taupyklėsFileOut.is_open()) {
                taupyklėsFileOut << status << "\n" << balansas << "\n";
                taupyklėsFileOut.close();
                pranesimas += "Taupyklė sėkmingai atnaujinta.\n";
            } else {
                pranesimas = "Nepavyko atnaujinti taupyklės failo.\n";
            }
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
        } else {
            taupyklėsFile.close();
            pranesimas = "Taupyklė tuščia arba failas tuščias.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
        }
    }

    void perziuretiLikuti(const Vaikas& vaikas, int klientoSoketas) {
        std::string pranesimas;
        std::string vaikoDir = "./vaikai/" + vaikas.getId();
        std::string taupyklėsFailas = vaikoDir + "/taupyklė.txt";

        if (!std::filesystem::exists(taupyklėsFailas)) {
            pranesimas = "Taupyklės failas nerastas.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }

        std::ifstream taupyklėsFile(taupyklėsFailas);
        if (!taupyklėsFile.is_open()) {
            pranesimas = "Nepavyko atidaryti taupyklės failo.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }
        std::string status, balansas;
        if (std::getline(taupyklėsFile, status) && std::getline(taupyklėsFile, balansas)) {
            pranesimas = "Taupyklės likutis: " + balansas + "\n";
            if (status == "0") {
                pranesimas += "Taupyklė užrakinta (negalima išimti pinigų).\n";
            } else if (status == "1") {
                pranesimas += "Taupyklė atrakinta (galima išimti pinigus).\n";
            } else {
                pranesimas += "Taupyklės būsena nežinoma.\n";
            }
        } else {
            pranesimas = "Taupyklė tuščia arba failas tuščias.\n";
        }
        taupyklėsFile.close();
        send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);

        std::string asmDuom = vaikoDir + "/asm_duom.txt";
        // Read parent ID from the fourth line of asm_duom.txt
        std::ifstream asmDuomFile(asmDuom);
        std::string line, parentId;
        int lineNum = 0;
        while (std::getline(asmDuomFile, line)) {
            ++lineNum;
            if (lineNum == 4) {
                parentId = line;
                break;
            }
        }
        asmDuomFile.close();
        if (!parentId.empty()) {
            // Compose veikla.txt path
            std::string veiklaPath = "./tevai/" + parentId + "/vaikai/" + vaikas.getId() + "/veikla.txt";
            // Get timestamp
            std::time_t now = std::time(nullptr);
            char timeStr[32];
            std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
            // Compose log entry
            std::string logEntry = std::string(timeStr) + "Peržiūrėtas taupyklės likutis\n";
            // Append to veikla.txt
            std::ofstream veiklaFile(veiklaPath, std::ios::app);
            veiklaFile << logEntry;
            veiklaFile.close();
        }
            return;
    }

    void idetiPinigus(const Vaikas vaikas, int klientoSoketas){
        std::string pranesimas;
        char buffer[4096];
        if (send(klientoSoketas, "Įveskite sumą, kurią norite įdėti į taupyklę: ", 45, 0) < 0) {
            perror("Klaida siunčiant duomenis");
            return;
        }
        ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            perror("Klaida gaunant duomenis");
            return;
        }
        buffer[bytesRead] = '\0';
        std::string suma(buffer);
        double sumaDouble;
        try {
            sumaDouble = std::stod(suma);
        } catch (...) {
            send(klientoSoketas, "Neteisinga suma. Bandykite dar kartą.\n", 37, 0);
            return;
        }
        if (sumaDouble <= 0) {
            send(klientoSoketas, "Suma turi būti teigiama.\n", 26, 0);
            return;
        }
        std::string vaikoDir = "./vaikai/" + vaikas.getId();
        std::string taupyklėsFailas = vaikoDir + "/taupyklė.txt";
        std::string status = "0", balansas = "0.00";
        // Read current status and balance
        if (std::filesystem::exists(taupyklėsFailas)) {
            std::ifstream taupyklėsFile(taupyklėsFailas);
            if (taupyklėsFile.is_open()) {
                std::getline(taupyklėsFile, status);
                std::getline(taupyklėsFile, balansas);
                taupyklėsFile.close();
            }
        }
        double balansasDouble = 0.0;
        try { balansasDouble = std::stod(balansas); } catch (...) {}
        balansasDouble += sumaDouble;
        // Write back
        std::ofstream taupyklėsFileOut(taupyklėsFailas, std::ios::trunc);
        if (taupyklėsFileOut.is_open()) {
            taupyklėsFileOut << status << "\n" << std::fixed << std::setprecision(2) << balansasDouble << "\n";
            taupyklėsFileOut.close();
            pranesimas = "Pinigai sėkmingai įdėti į taupyklę. Naujas likutis: " + std::to_string(balansasDouble) + "\n";
        } else {
            pranesimas = "Nepavyko atnaujinti taupyklės failo.\n";
        }
        send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);

        std::string asmDuom = vaikoDir + "/asm_duom.txt";
        // Read parent ID from the fourth line of asm_duom.txt
        std::ifstream asmDuomFile(asmDuom);
        std::string line, parentId;
        int lineNum = 0;
        while (std::getline(asmDuomFile, line)) {
            ++lineNum;
            if (lineNum == 4) {
                parentId = line;
                break;
            }
        }
        asmDuomFile.close();
        if (!parentId.empty()) {
            // Compose veikla.txt path
            std::string veiklaPath = "./tevai/" + parentId + "/vaikai/" + vaikas.getId() + "/veikla.txt";
            // Only create the file if it does not exist
            if (!std::filesystem::exists(veiklaPath)) {
                std::ofstream veiklaFileInit(veiklaPath); // creates the file
                veiklaFileInit.close();
            }
            // Get timestamp
            std::time_t now = std::time(nullptr);
            char timeStr[32];
            std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
            // Compose log entry
            std::string logEntry = std::string(timeStr) + "Įdėta " + std::to_string(sumaDouble) + " į taupyklę\n";
            // Append to veikla.txt
            std::ofstream veiklaFile(veiklaPath, std::ios::app);
            veiklaFile << logEntry;
            veiklaFile.close();
        }
            return;
    }

    void isimtiPinigus(const Vaikas vaikas, int klientoSoketas){
        std::string pranesimas;
        char buffer[4096];
        if (send(klientoSoketas, "Įveskite sumą, kurią norite išimti iš taupyklės: ", 47, 0) < 0) {
            perror("Klaida siunčiant duomenis");
            return;
        }
        ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            perror("Klaida gaunant duomenis");
            return;
        }
        buffer[bytesRead] = '\0';
        std::string suma(buffer);
        double sumaDouble;
        try {
            sumaDouble = std::stod(suma);
        } catch (...) {
            send(klientoSoketas, "Neteisinga suma. Bandykite dar kartą.\n", 37, 0);
            return;
        }
        if (sumaDouble <= 0) {
            send(klientoSoketas, "Suma turi būti teigiama.\n", 26, 0);
            return;
        }
        std::string vaikoDir = "./vaikai/" + vaikas.getId();
        std::string taupyklėsFailas = vaikoDir + "/taupyklė.txt";
        std::string status = "0", balansas = "0.00";
        // Read current status and balance
        if (std::filesystem::exists(taupyklėsFailas)) {
            std::ifstream taupyklėsFile(taupyklėsFailas);
            if (taupyklėsFile.is_open()) {
                std::getline(taupyklėsFile, status);
                std::getline(taupyklėsFile, balansas);
                taupyklėsFile.close();
            }
        }
        if (status == "0") {
            send(klientoSoketas, "Taupyklė užrakinta. Negalite išimti pinigų.\n", 42, 0);

            std::string asmDuom = vaikoDir + "/asm_duom.txt";
            // Read parent ID from the fourth line of asm_duom.txt
            std::ifstream asmDuomFile(asmDuom);
            std::string line, parentId;
            int lineNum = 0;
            while (std::getline(asmDuomFile, line)) {
                ++lineNum;
                if (lineNum == 4) {
                    parentId = line;
                    break;
                }
            }
            asmDuomFile.close();
            if (!parentId.empty()) {
                // Compose veikla.txt path
                std::string veiklaPath = "./tevai/" + parentId + "/vaikai/" + vaikas.getId() + "/veikla.txt";
                // Get timestamp
                std::time_t now = std::time(nullptr);
                char timeStr[32];
                std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
                // Compose log entry
                std::string logEntry = std::string(timeStr) + "Bandyta išimti " + std::to_string(sumaDouble) + " iš užrakintos taupyklės\n";
                // Append to veikla.txt
                std::ofstream veiklaFile(veiklaPath, std::ios::app);
                veiklaFile << logEntry;
                veiklaFile.close();
            }
            
            return;
        }
        double balansasDouble = 0.0;
        try { balansasDouble = std::stod(balansas); } catch (...) {}
        if (balansasDouble < sumaDouble) {
            send(klientoSoketas, "Nepakanka lėšų taupyklėje.\n", 29, 0);
            return;
        }
        balansasDouble -= sumaDouble;
        // Write back
        std::ofstream taupyklėsFileOut(taupyklėsFailas, std::ios::trunc);
        if (taupyklėsFileOut.is_open()) {
            taupyklėsFileOut << status << "\n" << std::fixed << std::setprecision(2) << balansasDouble << "\n";
            taupyklėsFileOut.close();
            pranesimas = "Pinigai sėkmingai išimti iš taupyklės. Naujas likutis: " + std::to_string(balansasDouble) + "\n";
        } else {
            pranesimas = "Nepavyko atnaujinti taupyklės failo.\n";
        }
        send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);

        std::string asmDuom = vaikoDir + "/asm_duom.txt";
        // Read parent ID from the fourth line of asm_duom.txt
        std::ifstream asmDuomFile(asmDuom);
        std::string line, parentId;
        int lineNum = 0;
        while (std::getline(asmDuomFile, line)) {
            ++lineNum;
            if (lineNum == 4) {
                parentId = line;
                break;
            }
        }
        asmDuomFile.close();
        if (!parentId.empty()) {
            // Compose veikla.txt path
            std::string veiklaPath = "./tevai/" + parentId + "/vaikai/" + vaikas.getId() + "/veikla.txt";
            // Only create the file if it does not exist
            if (!std::filesystem::exists(veiklaPath)) {
                std::ofstream veiklaFileInit(veiklaPath); // creates the file
                veiklaFileInit.close();
            }
            // Get timestamp
            std::time_t now = std::time(nullptr);
            char timeStr[32];
            std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
            // Compose log entry
            std::string logEntry = std::string(timeStr) + " Išimta " + std::to_string(sumaDouble) + " iš taupyklės\n";
            // Append to veikla.txt
            std::ofstream veiklaFile(veiklaPath, std::ios::app);
            veiklaFile << logEntry;
            veiklaFile.close();
        }
            return;
    }

    void interact(const Vaikas vaikas, int klientoSoketas) {
        int vaikoPasirinkimas = 0;
        bool atgal = false;
        char buffer[4096];

        while (!atgal) {
            vaikoPasirinkimas = 0; // Reset vaikoPasirinkimas each time to ensure menu is shown again

            std::string pranesimas = (
                "\n\n*** TAUPYKLĖS VALDYMAS ***\n\n"
                "1. Įdėti pinigus\n"
                "2. Išimti pinigus\n"
                "3. Peržiūrėti taupyklės likutį\n"
                "4. Atgal\n"
                "Pasirinkite veiksmą (1-4): "
            );
            if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
                perror("Klaida siunčiant duomenis");
                break;
            }
            ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead <= 0) {
                perror("Klaida gaunant duomenis");
                break;
            }
            buffer[bytesRead] = '\0';
            std::string atsakymas(buffer);
            if (atsakymas.empty() || !std::all_of(atsakymas.begin(), atsakymas.end(), ::isdigit) ||
                std::stoi(atsakymas) < 1 || std::stoi(atsakymas) > 4) {
                send(klientoSoketas, "Neteisingas pasirinkimas. Bandykite dar kartą.\n", 42, 0);
                continue;
            }
            vaikoPasirinkimas = std::stoi(atsakymas);
            switch (vaikoPasirinkimas) {
                case 1:
                    idetiPinigus(vaikas, klientoSoketas);
                    break;
                case 2:
                    isimtiPinigus(vaikas, klientoSoketas);
                    break;
                case 3:
                    perziuretiLikuti(vaikas, klientoSoketas);
                    break;
                case 4:
                    send(klientoSoketas, "Grįžtama atgal į tėvų meniu.\n", 36, 0);
                    atgal = true; // This will break out of the inner while loop
                    break; // Break from switch to re-display parent menu
                
            }
        }
        return;
    }

};
class valdymoEkr{

private:

    Tevas tevas;
    Vaikas vaikas;
    mokejimoKorteleEkr mokejimoKortEkr;
    pridejimoEkr vaikoPridejimoEkr;
    pasalinimoEkr vaikoPasalinimoEkr;
    perziurosEkr perziurosEkranas;
    papildymoEkr papildymoEkranas;
    taupyklesEkr taupyklesEkranas;
    veiklosPerziurosEkr veiklosPerziurosEkranas;

public:

    valdymoEkr(){};

    void setTevas(const Tevas& tevas) {
        this->tevas = tevas;
    }

    void interact(int klientoSoketas) {
        int vaikoPasirinkimas = 0;
        bool atgal = false;
        char buffer[4096];

        while (!atgal) {
            vaikoPasirinkimas = 0; // Reset vaikoPasirinkimas each time to ensure menu is shown again

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
                break;
            }
            ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead <= 0) {
                perror("Klaida gaunant duomenis");
                break;
            }
            buffer[bytesRead] = '\0';
            std::string atsakymas(buffer);
            if (atsakymas.empty() || !std::all_of(atsakymas.begin(), atsakymas.end(), ::isdigit) ||
                std::stoi(atsakymas) < 1 || std::stoi(atsakymas) > 7) {
                send(klientoSoketas, "Neteisingas pasirinkimas. Bandykite dar kartą.\n", 42, 0);
                continue;
            }
            vaikoPasirinkimas = std::stoi(atsakymas);
            switch (vaikoPasirinkimas) {
                case 1:
                    mokejimoKortEkr.interact(tevas, klientoSoketas);
                    break;
                case 2:
                    vaikas = vaikoPridejimoEkr.interact(tevas, klientoSoketas);
                    break;
                case 3:
                    vaikoPasalinimoEkr.interact(tevas, klientoSoketas);
                    break;
                case 4:
                    perziurosEkranas.interact(tevas, klientoSoketas); // Uncomment and implement when ready
                    break;
                case 5:
                    papildymoEkranas.interact(tevas, klientoSoketas); // Uncomment and implement when ready
                    
                    break;
                case 6:
                    taupyklesEkranas.uzrakintiAtrakinti(tevas, klientoSoketas); // Uncomment and implement when ready
                    break;
                case 7:
                    veiklosPerziurosEkranas.interact(tevas, klientoSoketas);
                    break;
                case 8:
                    send(klientoSoketas, "Grįžtama atgal į tėvų meniu.\n", 36, 0);
                    atgal = true; // This will break out of the inner while loop
                    break; // Break from switch to re-display parent menu
            }
        }
        return;
    }
};
class tevuPrisijungimoEkr {
private:

    Tevas tevas;
    Vaikas vaikas;
    authScreen auth;
    valdymoEkr valdymoEkranas;
    

public:

    tevuPrisijungimoEkr(){};

    void interact(int klientoSoketas) {
        int pasirinkimas = 0;

        while (true) {
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
                continue;
            }
            ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead <= 0) {
                perror("Klaida gaunant duomenis");
                return; // Exit if client disconnected
            }
            buffer[bytesRead] = '\0';
            std::string atsakymas(buffer);

            if (atsakymas.empty() || !std::all_of(atsakymas.begin(), atsakymas.end(), ::isdigit) || 
                std::stoi(atsakymas) < 1 || std::stoi(atsakymas) > 3) {
                send(klientoSoketas, "Neteisingas pasirinkimas. Bandykite dar kartą.\n", 42, 0);
                continue;
            }

            pasirinkimas = std::stoi(atsakymas);

            switch (pasirinkimas) {
                case 1: {
                    bool isAuthenticated = auth.authTevas(tevas, klientoSoketas);
                    if (!isAuthenticated) {
                        send(klientoSoketas, "Bandykite dar kartą.\n", 20, 0);
                        continue;
                    }
                    // Show child menu only if authenticated
                    valdymoEkranas.setTevas(tevas);
                    valdymoEkranas.interact(klientoSoketas);
                    break; // Break from outer switch (case 1) to re-display parent menu
                }
                case 2: {
                    tevas = auth.registerTevas(klientoSoketas);
                    if (tevas.getId().empty()) {
                        send(klientoSoketas, "Tėvo registracija nesėkminga. Bandykite dar kartą.\n", 40, 0);
                        continue;
                    }
                    send(klientoSoketas, "Tėvo registracija sėkminga.\n", 26, 0);
                    valdymoEkranas.setTevas(tevas); 
                    valdymoEkranas.interact(klientoSoketas);
                    break;
                }
                case 3: {
                    send(klientoSoketas, "Grįžtama atgal į pagrindinį meniu.\n", 40, 0);
                    return; // Exit to the previous menu (userInterface::pradzia)
                }
                default: {
                    send(klientoSoketas, "Neteisingas pasirinkimas. Bandykite dar kartą.\n", 42, 0);
                    break;
                }
            }
        }
    }
};
class vaikoPervedimoEkr{
public:

    vaikoPervedimoEkr(){};

    void interact(const Vaikas& vaikas, int klientoSoketas) {
        std::string pranesimas;
        char buffer[4096];
        ssize_t bytesRead;

        pranesimas = "Įveskite gavėjo ID: ";
        if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
            perror("Klaida siunčiant duomenis");
            return;
        }
        bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            perror("Klaida gaunant duomenis");
            return;
        }
        buffer[bytesRead] = '\0';
        std::string gavėjoID(buffer);

        pranesimas = "Įveskite sumą, kurią norite pervesti: ";
        if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
            perror("Klaida siunčiant duomenis");
            return;
        }
        bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            perror("Klaida gaunant duomenis");
            return;
        }
        buffer[bytesRead] = '\0';
        std::string suma(buffer);

        double sumaDouble;
        try {
            sumaDouble = std::stod(suma);
        } catch (...) {
            send(klientoSoketas, "Neteisinga suma. Bandykite dar kartą.\n", 37, 0);
            return;
        }
        if (sumaDouble <= 0) {
            send(klientoSoketas, "Suma turi būti teigiama.\n", 26, 0);
            return;
        }


        std::string vaikoDir = "./vaikai/" + vaikas.getId();
        std::string bankoSasPath;
        for (const auto& fileEntry : std::filesystem::directory_iterator(vaikoDir)) {
            if (!fileEntry.is_regular_file()) {
                continue; // Only process files
            }

            std::string filename = fileEntry.path().filename().string();
            // Check if filename contains "LT" and ends with ".txt"
            if (filename.find("LT") != std::string::npos && 
                filename.length() > 4 && filename.rfind(".txt") == filename.length() - 4) {
                
                // Found a potential bank account file
                bankoSasPath = fileEntry.path().string();
                break; // Assuming one bank account file per child directory, take the first one found
            }
        }
        if (bankoSasPath.empty()) {
            pranesimas = "Banko sąskaita nerasta.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }
        std::ifstream saskaitaFile(bankoSasPath);
        if (!saskaitaFile.is_open()) {
            pranesimas = "Nepavyko atidaryti banko sąskaitos failo.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }
        std::string balansasValue;
        if (std::getline(saskaitaFile, balansasValue) && !balansasValue.empty()) {
            saskaitaFile.close();
        } else {
            pranesimas = "Balansas: 0.00 (arba failas tuščias)\n"; // Default if file empty or balance not read
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }
        double balansasDouble = 0.0;
        try { balansasDouble = std::stod(balansasValue); } catch (...) {
            pranesimas = "Nepavyko perskaityti balanso.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }
        if (balansasDouble < sumaDouble) {
            pranesimas = "Nepakanka lėšų sąskaitoje.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }
        balansasDouble -= sumaDouble;
        // Write back the new balance

        std::ofstream saskaitaFileOut(bankoSasPath, std::ios::trunc);
        if (saskaitaFileOut.is_open()) {
            saskaitaFileOut << std::fixed << std::setprecision(2) << balansasDouble << "\n";
            saskaitaFileOut.close();
        } else {
            pranesimas = "Nepavyko atnaujinti banko sąskaitos failo.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }
        pranesimas = "Pervedimas sėkmingas. Naujas balansas: " + std::to_string(balansasDouble) + "\n";
        // Here you would typically log the transfer to the recipient's account as well
        std::string gavėjoDir = "./vaikai/" + gavėjoID;
        std::string gavėjoSasPath;
        for (const auto& fileEntry : std::filesystem::directory_iterator(gavėjoDir)) {
            if (!fileEntry.is_regular_file()) {
                continue; // Only process files
            }

            std::string filename = fileEntry.path().filename().string();
            // Check if filename contains "LT" and ends with ".txt"
            if (filename.find("LT") != std::string::npos && 
                filename.length() > 4 && filename.rfind(".txt") == filename.length() -  4) {
                
                // Found a potential bank account file
                gavėjoSasPath = fileEntry.path().string();
                break; // Assuming one bank account file per child directory, take the first one found
            }
        }
        if (gavėjoSasPath.empty()) {
            pranesimas = "Gavėjo banko sąskaita nerasta.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }
        std::ifstream gavėjoSaskaitaFile(gavėjoSasPath);
        if (!gavėjoSaskaitaFile.is_open()) {
            pranesimas = "Nepavyko atidaryti gavėjo banko sąskaitos failo.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }
        std::string gavėjoBalansasValue;
        if (std::getline(gavėjoSaskaitaFile, gavėjoBalansasValue) && !gavėjoBalansasValue.empty()) {
            gavėjoSaskaitaFile.close();
        } else {
            pranesimas = "Gavėjo balansas: 0.00 (arba failas tuščias)\n"; // Default if file empty or balance not read
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }
        double gavėjoBalansasDouble = 0.0;
        try { gavėjoBalansasDouble = std::stod(gavėjoBalansasValue); } catch (...) {
            pranesimas = "Nepavyko perskaityti gavėjo balanso.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }
        gavėjoBalansasDouble += sumaDouble;
        // Write back the new balance to the recipient's account
        std::ofstream gavėjoSaskaitaFileOut(gavėjoSasPath, std::ios::trunc);
        if (gavėjoSaskaitaFileOut.is_open()) {
            gavėjoSaskaitaFileOut << std::fixed << std::setprecision(2) << gavėjoBalansasDouble << "\n";
            gavėjoSaskaitaFileOut.close();
        } else {
            pranesimas = "Nepavyko atnaujinti gavėjo banko sąskaitos failo.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }
        pranesimas = "Pervedimas sėkmingas gavėjui. Naujas gavėjo balansas: " + std::to_string(gavėjoBalansasDouble) + "\n";
               
        send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);

        std::string asmDuom = vaikoDir + "/asm_duom.txt";
        // Read parent ID from the fourth line of asm_duom.txt
        std::ifstream asmDuomFile(asmDuom);
        std::string line, parentId;
        int lineNum = 0;
        while (std::getline(asmDuomFile, line)) {
            ++lineNum;
            if (lineNum == 4) {
                parentId = line;
                break;
            }
        }
        asmDuomFile.close();
        if (!parentId.empty()) {
            // Compose veikla.txt path
            std::string veiklaPath = "./tevai/" + parentId + "/vaikai/" + vaikas.getId() + "/veikla.txt";
            // Only create the file if it does not exist
            if (!std::filesystem::exists(veiklaPath)) {
                std::ofstream veiklaFileInit(veiklaPath); // creates the file
                veiklaFileInit.close();
            }
            // Get timestamp
            std::time_t now = std::time(nullptr);
            char timeStr[32];
            std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
            // Compose log entry
            std::string logEntry = std::string(timeStr) + " Pervedė " + suma + " į " + gavėjoID + " sąskaitą\n";
            // Append to veikla.txt
            std::ofstream veiklaFile(veiklaPath, std::ios::app);
            veiklaFile << logEntry;
            veiklaFile.close();
        }

        return;
        
    }


};
class likutisEkr{

public:

    likutisEkr(){};

    void interact(const Vaikas& vaikas, int klientoSoketas) {
        std::string pranesimas;

        std::string vaikoDir = "./vaikai/" + vaikas.getId();
        std::string bankoSasPath;

        for (const auto& fileEntry : std::filesystem::directory_iterator(vaikoDir)) {
            if (!fileEntry.is_regular_file()) {
                continue; // Only process files
            }

            std::string filename = fileEntry.path().filename().string();
            // Check if filename contains "LT" and ends with ".txt"
            if (filename.find("LT") != std::string::npos && 
                filename.length() > 4 && filename.rfind(".txt") == filename.length() - 4) {
                
                // Found a potential bank account file
                bankoSasPath = fileEntry.path().string();
                break; // Assuming one bank account file per child directory, take the first one found
            }
        }

        if (bankoSasPath.empty()) {
            pranesimas = "Banko sąskaita nerasta.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }
        std::ifstream saskaitaFile(bankoSasPath);
        if (!saskaitaFile.is_open()) {
            pranesimas = "Nepavyko atidaryti banko sąskaitos failo.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }
        std::string balansasValue;
        if (std::getline(saskaitaFile, balansasValue) && !balansasValue.empty()) {
            pranesimas = "Balansas: " + balansasValue + "\n";
        } else {
            pranesimas = "Balansas: 0.00 (arba failas tuščias)\n"; // Default if file empty or balance not read
        }
        saskaitaFile.close();
        send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);

        std::string asmDuom = vaikoDir + "/asm_duom.txt";
        // Read parent ID from the fourth line of asm_duom.txt
        std::ifstream asmDuomFile(asmDuom);
        std::string line, parentId;
        int lineNum = 0;
        while (std::getline(asmDuomFile, line)) {
            ++lineNum;
            if (lineNum == 4) {
                parentId = line;
                break;
            }
        }
        asmDuomFile.close();
        if (!parentId.empty()) {
            // Compose veikla.txt path
            std::string veiklaPath = "./tevai/" + parentId + "/vaikai/" + vaikas.getId() + "/veikla.txt";
            // Get timestamp
            std::time_t now = std::time(nullptr);
            char timeStr[32];
            std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
            // Compose log entry
            std::string logEntry = std::string(timeStr) + "Peržiūrėtas likutis\n";
            // Append to veikla.txt
            std::ofstream veiklaFile(veiklaPath, std::ios::app);
            veiklaFile << logEntry;
            veiklaFile.close();
        }
            return;

    }

};
class vaikoEkranas {

private:
    Vaikas vaikas;
    taupyklesEkr taupyklesEkranas;
    likutisEkr likutisEkranas;
    vaikoPervedimoEkr vaikoPervedimoEkranas;

public:

    vaikoEkranas(){};

    void setVaikas(const Vaikas& vaikas) {
        this->vaikas = vaikas;
    }

    void interact(int klientoSoketas) {
        int vaikoPasirinkimas = 0;
        bool atgal = false;
        char buffer[4096];

        while (!atgal) {
            vaikoPasirinkimas = 0; // Reset vaikoPasirinkimas each time to ensure menu is shown again

            std::string pranesimas = (
                "\n\n*** VAIKO BANKO VALDYMAS ***\n\n"
                "1. Pervesti draugui\n"
                "2. Likutis\n"
                "3. Taupyklė\n"
                "4. Atgal\n"
                "Pasirinkite veiksmą (1-4): "
            );
            if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
                perror("Klaida siunčiant duomenis");
                break;
            }
            ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead <= 0) {
                perror("Klaida gaunant duomenis");
                break;
            }
            buffer[bytesRead] = '\0';
            std::string atsakymas(buffer);
            if (atsakymas.empty() || !std::all_of(atsakymas.begin(), atsakymas.end(), ::isdigit) ||
                std::stoi(atsakymas) < 1 || std::stoi(atsakymas) > 7) {
                send(klientoSoketas, "Neteisingas pasirinkimas. Bandykite dar kartą.\n", 42, 0);
                continue;
            }
            vaikoPasirinkimas = std::stoi(atsakymas);
            switch (vaikoPasirinkimas) {
                case 1:
                    vaikoPervedimoEkranas.interact(vaikas, klientoSoketas); // Uncomment and implement when ready
                    send(klientoSoketas, "Funkcionalumas dar neįgyvendintas.\n", 36, 0);
                    break;
                case 2:
                    likutisEkranas.interact(vaikas, klientoSoketas); // Uncomment and implement when ready
            
                    break;
                case 3:
                
                    taupyklesEkranas.interact(vaikas, klientoSoketas); // Uncomment and implement when ready
                    break;
                case 4:
                    send(klientoSoketas, "Grįžtama atgal į tėvų meniu.\n", 36, 0);
                    atgal = true; // This will break out of the inner while loop
                    break; // Break from switch to re-display parent menu
                
            }
        }
        return;
    }
};
class vaikuPrisijungimoEkr {
private:
    Vaikas vaikas;
    authScreen auth;
    vaikoEkranas vaikoEkr;

public:

    vaikuPrisijungimoEkr(){};

    void interact(int klientoSoketas) {
      int pasirinkimas = 0;

        while (true) {
            pasirinkimas = 0; // Reset pasirinkimas each time to ensure menu is shown again
            std::string pranesimas = (
                "\n\n*** VAIKŲ PRISIJUNGIMAS ***\n\n"
                "1. Prisijungti\n"
                "2. Atgal\n\n"
                "Pasirinkite veiksmą (1-2): "
            );

            char buffer[4096];
            if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
                perror("Klaida siunčiant duomenis");
                continue;
            }
            ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead <= 0) {
                perror("Klaida gaunant duomenis");
                return; // Exit if client disconnected
            }
            buffer[bytesRead] = '\0';
            std::string atsakymas(buffer);

            if (atsakymas.empty() || !std::all_of(atsakymas.begin(), atsakymas.end(), ::isdigit) || 
                std::stoi(atsakymas) < 1 || std::stoi(atsakymas) > 2) {
                send(klientoSoketas, "Neteisingas pasirinkimas. Bandykite dar kartą.\n", 42, 0);
                continue;
            }

            pasirinkimas = std::stoi(atsakymas);

            switch (pasirinkimas) {
                case 1: {
                    bool isAuthenticated = auth.authVaikas(vaikas, klientoSoketas);
                    if (!isAuthenticated) {
                        send(klientoSoketas, "Bandykite dar kartą.\n", 20, 0);
                        continue;
                    }
                    vaikoEkr.setVaikas(vaikas);
                    vaikoEkr.interact(klientoSoketas);
                    break; // Break from outer switch (case 1) to re-display parent menu
                }
                case 2: {
                    send(klientoSoketas, "Grįžtama atgal į pagrindinį meniu.\n", 40, 0);
                    return; // Exit to the previous menu (userInterface::pradzia)
                }
                default: {
                    send(klientoSoketas, "Neteisingas pasirinkimas. Bandykite dar kartą.\n", 42, 0);
                    break;
                }
            }
        }

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
                        continue;
                    }
                    ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
                    if (bytesRead <= 0) {
                        perror("Klaida gaunant duomenis");
                        break; // This break exits the inner while loop
                    }
                    buffer[bytesRead] = '\0';
                    std::string atsakymas(buffer);

                    if (atsakymas.empty() || !std::all_of(atsakymas.begin(), atsakymas.end(), ::isdigit) || 
                        std::stoi(atsakymas) < 1 || std::stoi(atsakymas) > 3) {
                        send(klientoSoketas, "Neteisingas pasirinkimas. Bandykite dar kartą.\n", 45, 0);
                        continue;
                    }

                    pasirinkimas = std::stoi(atsakymas);
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
                        close(klientoSoketas);
                        return;
                    default:    
                        send(klientoSoketas, "Neteisingas pasirinkimas. Bandykite dar kartą.\n", 45, 0);
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
