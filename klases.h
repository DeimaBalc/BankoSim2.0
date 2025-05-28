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

class pridejimoEkr{
public:
    pridejimoEkr(){};

    Vaikas interact(const Tevas tevas, int klientoSoketas) {
        srand(time(nullptr));
        std::string pranesimas;
        char buffer[4096];
        Vaikas vaikas(std::to_string(rand() % 90000 + 10000));

        pranesimas = "Įveskite vaiko vardą: ";
        if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
            perror("Klaida siunčiant duomenis");
            return vaikas;
        }
        ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            perror("Klaida gaunant duomenis");
            return vaikas;
        }
        buffer[bytesRead] = '\0';
        std::string vaikoVardas(buffer);
        vaikas.setVardas(vaikoVardas);

        pranesimas = "Įveskite vaiko pavardę: ";
        if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
            perror("Klaida siunčiant duomenis");
            return vaikas;
        }
        bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            perror("Klaida gaunant duomenis");
            return vaikas;
        }
        buffer[bytesRead] = '\0';
        std::string vaikoPavarde(buffer);
        vaikas.setPavarde(vaikoPavarde);

        pranesimas = "Sukurkite vaiko slaptažodį: ";
        if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
            perror("Klaida siunčiant duomenis");
            return  vaikas;
        }
        bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            perror("Klaida gaunant duomenis");
            return vaikas;
        }
        buffer[bytesRead] = '\0';
        std::string vaikosSlaptazodis(buffer);

        std::string tevoDir = "./tevai/" + tevas.getId() + "/vaikai/" + vaikas.getId();
        std::string vaikoDir = "./vaikai/" + vaikas.getId();

        if (std::filesystem::exists(tevoDir)) {
            pranesimas = "Failas su tokiu vaiko ID jau egzistuoja.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return vaikas;
        }
        if (!std::filesystem::exists(tevoDir)) {
            if (!std::filesystem::create_directories(tevoDir)) {
                send(klientoSoketas, "Nepavyko sukurti direktorijos.\n", 32, 0);
                return vaikas;
            }
        }
        if (std::filesystem::exists(vaikoDir)) {
            pranesimas = "Failas su tokiu vaiko ID jau egzistuoja.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return vaikas;
        }
        if (!std::filesystem::exists(vaikoDir)) {
            if (!std::filesystem::create_directories(vaikoDir)) {
                send(klientoSoketas, "Nepavyko sukurti vaiko direktorijos.\n", 32, 0);
                return vaikas;
            }
        }

        std::string salies_kodas = "LT";
        std::string kontrolinis_skaicius = std::to_string(rand() % 9000 + 1000);
        std::string banko_kodas = "7300";
        std::string unikalus_numeris = std::to_string(rand() % 90000000 + 10000000);
        std::string bankoSaskaitosNumeris = salies_kodas + kontrolinis_skaicius + banko_kodas + unikalus_numeris;
        vaikas.setBankoSas(bankoSaskaitosNumeris);

        // Only write asm_duom.txt in parent's directory
        std::ofstream vaikoFailPasTeva(tevoDir + "/asm_duom.txt");
        if (!vaikoFailPasTeva.is_open()) {
            send(klientoSoketas, "Nepavyko sukurti failo.\n", 26, 0);
            return vaikas;
        }
        vaikoFailPasTeva << vaikosSlaptazodis << "\n";
        vaikoFailPasTeva << vaikoVardas << "\n";
        vaikoFailPasTeva << vaikoPavarde << "\n";
        vaikoFailPasTeva << tevas.getId() << "\n";
        vaikoFailPasTeva.close();

        // Write asm_duom.txt and bank account file in global vaikai dir
        std::ofstream vaikoFailPasVaika(vaikoDir + "/asm_duom.txt");
        std::ofstream vaikoSasPasVaika(vaikoDir + "/" + vaikas.getBankoSas() + ".txt");
        if (!vaikoFailPasVaika.is_open()) {
            send(klientoSoketas, "Nepavyko sukurti failo.(1)\n", 26, 0);
            return vaikas;
        }
        if (!vaikoSasPasVaika.is_open()) {
            send(klientoSoketas, "Nepavyko sukurti failo.(2)\n", 26, 0);
            return vaikas;
        }
        vaikoFailPasVaika << vaikosSlaptazodis << "\n";
        vaikoFailPasVaika << vaikoVardas << "\n";
        vaikoFailPasVaika << vaikoPavarde << "\n";
        vaikoFailPasVaika << tevas.getId() << "\n";
        vaikoSasPasVaika << "0\n";
        vaikoFailPasVaika.close();
        vaikoSasPasVaika.close();

        std::string successMessage = "Vaiko registracija sėkminga.\n";
        if (send(klientoSoketas, successMessage.c_str(), successMessage.size(), 0) < 0) {
            perror("Klaida siunčiant registracijos pranešimą");
        }
        send(klientoSoketas, ("Prisijungimo ID: " + vaikas.getId() + "\n" + "Vaiko banko s1skaita: " + vaikas.getBankoSas() + "/n").c_str(), 26, 0);
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

        std::cout << "DEBUG: Tevo ID: " << tevas.getId() << "\n"; // DEBUG
        std::cout << "DEBUG: Tevo direktorija: " << tevoDir << "\n"; // DEBUG

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

        std::cout << "DEBUG: Kortelės numeris gautas: " << kortelesNumeris << "\n"; // DEBUG

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

    // Update balances in both parent and both child directories, add debug and success message
    void updateBalansas(const std::string& tevoSasPath, double naujasTevoBalansas,
                        const std::string& vaikoGlobalSasPath, double naujasVaikoBalansas,
                        const std::string& vaikoPasTevaSasPath) {
        std::cout << "DEBUG: updateBalansas called." << std::endl;
        std::cout << "DEBUG: tevoSasPath = " << tevoSasPath << ", naujasTevoBalansas = " << std::fixed << std::setprecision(2) << naujasTevoBalansas << std::endl;
        std::cout << "DEBUG: vaikoGlobalSasPath = " << vaikoGlobalSasPath << ", naujasVaikoBalansas = " << std::fixed << std::setprecision(2) << naujasVaikoBalansas << std::endl;
        std::cout << "DEBUG: vaikoPasTevaSasPath = " << vaikoPasTevaSasPath << std::endl;

        // Update parent's balance
        std::ofstream tevoSasFile(tevoSasPath, std::ios::trunc);
        if (tevoSasFile.is_open()) {
            tevoSasFile << std::fixed << std::setprecision(2) << naujasTevoBalansas << "\n";
            tevoSasFile.close();
            std::cout << "DEBUG: Parent's balance updated successfully." << std::endl;
        } else {
            std::cout << "DEBUG: Failed to open parent's account file for writing: " << tevoSasPath << std::endl;
        }
        // Update child's balance in ./vaikai
        std::ofstream vaikoGlobalSasFile(vaikoGlobalSasPath, std::ios::trunc);
        if (vaikoGlobalSasFile.is_open()) {
            vaikoGlobalSasFile << std::fixed << std::setprecision(2) << naujasVaikoBalansas << "\n";
            vaikoGlobalSasFile.close();
            std::cout << "DEBUG: Child's balance (./vaikai) updated successfully." << std::endl;
        } else {
            std::cout << "DEBUG: Failed to open child's account file for writing: " << vaikoGlobalSasPath << std::endl;
        }
        // Update child's balance in ./tevai/{tevasId}/vaikai/{vaikoId}
        std::ofstream vaikoPasTevaSasFileStream(vaikoPasTevaSasPath, std::ios::trunc);
        if (vaikoPasTevaSasFileStream.is_open()) {
            vaikoPasTevaSasFileStream << std::fixed << std::setprecision(2) << naujasVaikoBalansas << "\n";
            vaikoPasTevaSasFileStream.close();
            std::cout << "DEBUG: Child's balance (pas teva) updated successfully." << std::endl;
        } else {
            std::cout << "DEBUG: Failed to open child's account file for writing (pas teva): " << vaikoPasTevaSasPath << std::endl;
        }
    }

    void interact(const Tevas tevas, int klientoSoketas) {
        std::string pranesimas;
        char buffer[4096];
        ssize_t bytesRead;

        pranesimas = "Įveskite vaiko ID: "; // Changed from %veskite to Įveskite for consistency
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

        std::string tevoVaikoDir = "./tevai/" + tevas.getId() + "/vaikai/" + vaikoID;
        std::string vaikoGlobalDir = "./vaikai/" + vaikoID; // Renamed for clarity

        if (!std::filesystem::exists(tevoVaikoDir)) {
            pranesimas = "Vaikas su tokiu ID nerastas.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }

        std::string vaikoBankoSaskaitosNumeris = "Nerasta";
        std::string vaikoBalansasValue;
        bool vaikoBankoFileRastas = false;
        std::string vaikoGlobalSasPath; // Path to child's bank file in ./vaikai/{vaikoID}
        std::string vaikoPasTevaSasPath; // Path to child's bank file in ./tevai/{tevasID}/vaikai/{vaikoID}

        // --- Step 1: Find child's account file and read balance from the global vaikai directory ---
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
                            vaikoBalansasValue = "0.00"; // Default if file empty or balance not read
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

        // --- Step 2: Find child's account file within the parent's directory ---
        // This path is needed for updating the copy of the child's balance in the parent's structure
        if (std::filesystem::exists(tevoVaikoDir) && std::filesystem::is_directory(tevoVaikoDir)) {
            for (const auto& fileEntry : std::filesystem::directory_iterator(tevoVaikoDir)) {
                if (!fileEntry.is_regular_file()) continue;
                std::string filename = fileEntry.path().filename().string();
                if (filename.find("LT") != std::string::npos && filename.length() > 4 && filename.rfind(".txt") == filename.length() - 4) {
                    vaikoPasTevaSasPath = fileEntry.path().string();
                    break; // Found the file
                }
            }
        }
        if (vaikoPasTevaSasPath.empty()) {
            pranesimas = "Vaiko banko sąskaitos kopija tėvo kataloge nerasta.\n";
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

        // --- Step 3: Find parent's account file and read parent's balance ---
        std::string tevoSaskaitosNumeris = "Nerasta";
        std::string tevoBalansasValue;
        bool tevoBankoFileRastas = false;
        std::string tevoSasPath;

        std::string tevoRootDir = "./tevai/" + tevas.getId(); // Root directory for the parent
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
                            tevoBalansasValue = "0.00"; // Default if file empty
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

        // --- Step 4: Call the update function ---
        updateBalansas(tevoSasPath, naujasTevoBalansas, vaikoGlobalSasPath, naujasVaikoBalansas, vaikoPasTevaSasPath);
        
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
class veiklosEkr{};
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
    }

    void interact(const Vaikas vaikas, int klientoSoketas) {
        int vaikoPasirinkimas = 0;
        bool atgal = false;
        char buffer[4096];

        while (!atgal) {
            std::cout << "DEBUG: Displaying TAUPYKLES menu.\n"; // DEBUG
        
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
                    std::cout << "DEBUG: Exiting VAIKO BANKO VALDYMAS menu.\n"; // DEBUG
                    break; // Break from switch to re-display parent menu
                
            }
        }
        std::cout << "DEBUG: Returned from VAIKO BANKO VALDYMAS menu. Looping back to TĖVŲ PRISIJUNGIMAS menu.\n"; // DEBUG
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
            std::cout << "DEBUG: Displaying VAIKO BANKO VALDYMAS menu.\n"; // DEBUG
        
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
                    valdymoEkranas.setTevas(tevas);
                    valdymoEkranas.interact(klientoSoketas);
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
                    
                    valdymoEkranas.setTevas(tevas); 
                    valdymoEkranas.interact(klientoSoketas);
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
class vaikoPervedimoEkr{};
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
        std::cout << "DEBUG: Sent balance information to client: '" << pranesimas << "'\n"; // DEBUG

    }

};
class vaikoEkranas {

private:
    Vaikas vaikas;
    taupyklesEkr taupyklesEkranas;
    likutisEkr likutisEkranas;

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
            std::cout << "DEBUG: Displaying VAIKO BANKO VALDYMAS menu.\n"; // DEBUG
        
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
                    // vaikoPervedimoEkr.interact(vaikas, klientoSoketas); // Uncomment and implement when ready
                    send(klientoSoketas, "Funkcionalumas dar neįgyvendintas.\n", 36, 0);
                    break;
                case 2:
                    likutisEkranas.interact(vaikas, klientoSoketas); // Uncomment and implement when ready
            
                    break;
                case 3:
                
                    taupyklesEkranas.interact(vaikas, klientoSoketas); // Uncomment and implement when ready
                    std::cout << "DEBUG: taupyklesEkranas.interact() called.\n"; // DEBUG
                    break;
                case 4:
                    send(klientoSoketas, "Grįžtama atgal į tėvų meniu.\n", 36, 0);
                    atgal = true; // This will break out of the inner while loop
                    std::cout << "DEBUG: Exiting VAIKO BANKO VALDYMAS menu.\n"; // DEBUG
                    break; // Break from switch to re-display parent menu
                
            }
        }
        std::cout << "DEBUG: Returned from VAIKO BANKO VALDYMAS menu. Looping back to TĖVŲ PRISIJUNGIMAS menu.\n"; // DEBUG
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
            std::cout << "DEBUG: Displaying Vaiku PRISIJUNGIMAS menu.\n"; // DEBUG
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
                std::cout << "DEBUG: Failed to send Vaiku PRISIJUNGIMAS menu.\n"; // DEBUG
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
                std::stoi(atsakymas) < 1 || std::stoi(atsakymas) > 2) {
                send(klientoSoketas, "Neteisingas pasirinkimas. Bandykite dar kartą.\n", 42, 0);
                std::cout << "DEBUG: Invalid input for TĖVŲ PRISIJUNGIMAS menu.\n"; // DEBUG
                continue;
            }

            pasirinkimas = std::stoi(atsakymas);
            std::cout << "DEBUG: Parsed vaiku PRISIJUNGIMAS menu choice: " << pasirinkimas << "\n"; // DEBUG

            switch (pasirinkimas) {
                case 1: {
                    std::cout << "DEBUG: Entering authentication flow.\n"; // DEBUG
                    bool isAuthenticated = auth.authVaikas(vaikas, klientoSoketas);
                    // Removed redundant send here, as auth.authTevas already sends success/failure messages.
                    if (!isAuthenticated) {
                        send(klientoSoketas, "Bandykite dar kartą.\n", 20, 0);
                        std::cout << "DEBUG: Authentication failed. Looping back to vaiku PRISIJUNGIMAS menu.\n"; // DEBUG
                        continue;
                    }
                    std::cout << "DEBUG: Authentication successful. Entering VAIKO BANKO VALDYMAS menu.\n"; // DEBUG
                    // Show child menu only if authenticated
                    vaikoEkr.setVaikas(vaikas);
                    vaikoEkr.interact(klientoSoketas);
                    std::cout << "DEBUG: Returned from vaikoEkranas.interact(). Looping back to TĖVŲ PRISIJUNGIMAS menu.\n"; // DEBUG
                    break; // Break from outer switch (case 1) to re-display parent menu
                }
                case 2: {
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
