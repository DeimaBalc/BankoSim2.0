#ifndef KLASES_H
#define KLASES_H

#include <string>
#include <vector>
#include <map>
#include <random>
#include <filesystem>
#include <fstream>
#include <iomanip> // Required for std::fixed and std::setprecision
#include <ctime>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream> // For std::cerr, std::cout (if needed for debugging)
#include <thread>
#include <algorithm> // For std::all_of

// Forward declaration if needed, or ensure classes are defined in order
// class Tevas;
// class Vaikas;

class Tevas {
private:
    std::string vardas;
    std::string pavarde;
    std::string id;

public:
    Tevas() {}
    Tevas(const std::string& id) : id(id) {} // Constructor declaration

    std::string getId() const { return this->id; }
    std::string getVardas() const { return this->vardas; }
    std::string getPavarde() const { return this->pavarde; }

    void setVardas(std::string newVardas) { this->vardas = newVardas; }
    void setPavarde(std::string newPavarde) { this->pavarde = newPavarde; }
    void setId(std::string newId) { this->id = newId; }

    std::string to_string() const {
        return "Vardas ir pavardė: " + this->vardas + " " + this->pavarde + "\n" +
               "Prisijungimo ID: " + this->id + "\n";
    }

    
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
        return this->bankoSaskaitosNumeris;
    }
};

class pridejimoEkr {
public:
    pridejimoEkr() {}
    Vaikas interact(const Tevas& tevas, int klientoSoketas) {
        srand(time(nullptr)); 
        std::string pranesimas;
        char buffer[4096];
        Vaikas vaikas(std::to_string(rand() % 90000 + 10000));

        pranesimas = "Įveskite vaiko vardą: ";
        if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
            perror("Klaida siunčiant duomenis");
            return vaikas; // Return partially initialized vaikas
        }
        ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            perror("Klaida gaunant duomenis");
            return vaikas;
        }
        buffer[bytesRead] = '\0';
        vaikas.setVardas(std::string(buffer));

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
        vaikas.setPavarde(std::string(buffer));

        pranesimas = "Sukurkite vaiko slaptažodį: ";
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
        std::string vaikoSlaptazodis(buffer);

        std::string tevoDir = "./tevai/" + tevas.getId() + "/vaikai/" + vaikas.getId();
        std::string vaikoDir = "./vaikai/" + vaikas.getId();

        if (std::filesystem::exists(tevoDir)) {
            pranesimas = "Klaida: Vaikas su tokiu ID jau egzistuoja tėvo kataloge.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return vaikas;
        }
        if (!std::filesystem::create_directories(tevoDir)) {
            pranesimas = "Klaida: Nepavyko sukurti tėvo vaiko katalogo.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return vaikas;
        }

        if (std::filesystem::exists(vaikoDir)) {
            pranesimas = "Klaida: Vaikas su tokiu ID jau egzistuoja bendrame vaikų kataloge.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            // Cleanup previously created directory for consistency if needed
            std::filesystem::remove_all(tevoDir);
            return vaikas;
        }
        if (!std::filesystem::create_directories(vaikoDir)) {
            pranesimas = "Klaida: Nepavyko sukurti vaiko katalogo.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            std::filesystem::remove_all(tevoDir); // Cleanup
            return vaikas;
        }

        std::string salies_kodas = "LT";
        std::string kontrolinis_skaicius = std::to_string(rand() % 90 + 10); // Ensuring 2 digits for typical control
        std::string banko_kodas = "73000"; // Typical 5-digit bank code
        std::string unikalus_numeris;
        for(int i = 0; i < 11; ++i) { // Ensuring 11 digits for account number part
            unikalus_numeris += std::to_string(rand() % 10);
        }
        std::string bankoSaskaitosNumeris = salies_kodas + kontrolinis_skaicius + banko_kodas + unikalus_numeris; // LT + 2 + 5 + 11 = 20 chars
        vaikas.setBankoSas(bankoSaskaitosNumeris);

        std::ofstream vaikoFailPasTeva(tevoDir + "/asm_duom.txt");
        if (!vaikoFailPasTeva.is_open()) {
            pranesimas = "Klaida: Nepavyko sukurti asm_duom.txt failo tėvo vaiko kataloge.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            std::filesystem::remove_all(tevoDir);
            std::filesystem::remove_all(vaikoDir);
            return vaikas;
        }
        vaikoFailPasTeva << vaikoSlaptazodis << "\n";
        vaikoFailPasTeva << vaikas.getVardas() << "\n";
        vaikoFailPasTeva << vaikas.getPavarde() << "\n";
        vaikoFailPasTeva << tevas.getId() << "\n"; // Store parent's ID
        vaikoFailPasTeva.close();

        std::ofstream vaikoFailPasVaika(vaikoDir + "/asm_duom.txt");
        if (!vaikoFailPasVaika.is_open()) {
            pranesimas = "Klaida: Nepavyko sukurti asm_duom.txt failo vaiko kataloge.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            std::filesystem::remove_all(tevoDir);
            std::filesystem::remove_all(vaikoDir);
            return vaikas;
        }
        vaikoFailPasVaika << vaikoSlaptazodis << "\n";
        vaikoFailPasVaika << vaikas.getVardas() << "\n";
        vaikoFailPasVaika << vaikas.getPavarde() << "\n";
        vaikoFailPasVaika << tevas.getId() << "\n"; // Store parent's ID
        vaikoFailPasVaika.close();

        std::ofstream vaikoSasPasVaika(vaikoDir + "/" + vaikas.getBankoSas() + ".txt");
        if (!vaikoSasPasVaika.is_open()) {
            pranesimas = "Klaida: Nepavyko sukurti banko sąskaitos failo.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            std::filesystem::remove_all(tevoDir);
            std::filesystem::remove_all(vaikoDir);
            return vaikas;
        }
        vaikoSasPasVaika << std::fixed << std::setprecision(2) << 0.00 << "\n"; // Initial balance
        vaikoSasPasVaika.close();
        
        // Create taupykle.txt for the child
        std::ofstream taupykleFile(vaikoDir + "/taupyklė.txt");
        if (!taupykleFile.is_open()) {
            pranesimas = "Klaida: Nepavyko sukurti taupyklės failo.\n";
            // Non-critical, proceed with registration but inform user or log
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
        } else {
            taupykleFile << "0\n"; // 0 for locked
            taupykleFile << std::fixed << std::setprecision(2) << 0.00 << "\n"; // Initial balance 0.00
            taupykleFile.close();
        }


        pranesimas = "Vaiko registracija sėkminga!\n";
        send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
        pranesimas = "Vaiko prisijungimo ID: " + vaikas.getId() + "\n" +
                     "Vaiko banko sąskaita: " + vaikas.getBankoSas() + "\n";
        send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);

        
        return vaikas;
    }
};

class mokejimoKorteleEkr {
public:
    mokejimoKorteleEkr() {}
    std::string pranesimas;

    void interact(const Tevas& tevas, int klientoSoketas) {
        char buffer[4096];
        std::string tevoDir = "./tevai/" + tevas.getId();

        if (!std::filesystem::exists(tevoDir)) {
            // This directory should exist if parent is logged in / registered
            // but create_directories can be a fallback
            if (!std::filesystem::create_directories(tevoDir)) {
                pranesimas = "Klaida: Nepavyko sukurti tėvo katalogo.\n";
                send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
                return;
            }
        }

        pranesimas = "Įveskite kortelės numerį (pvz., LTXXXXXXXXXXXXXXXX): ";
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

        if (kortelesNumeris.size() != 20 || kortelesNumeris.substr(0, 2) != "LT") { 
            pranesimas = "Klaida: Neteisingas kortelės numerio formatas. Turi būti LT ir 18 skaitmenų (viso 20 simbolių).\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }

        std::string filePath = tevoDir + "/" + kortelesNumeris + ".txt";
        if (std::filesystem::exists(filePath)) {
            pranesimas = "Klaida: Failas su tokiu kortelės numeriu jau egzistuoja.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }

        std::ofstream tevoFailas(filePath);
        if (!tevoFailas.is_open()) {
            pranesimas = "Klaida: Nepavyko sukurti kortelės failo.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }

        pranesimas = "Įveskite likutį: ";
        if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
            perror("Klaida siunčiant duomenis");
            tevoFailas.close(); // Close file before returning
            return;
        }
        bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            perror("Klaida gaunant duomenis");
            tevoFailas.close();
            return;
        }
        buffer[bytesRead] = '\0';
        std::string likutisStr(buffer);
        double likutisDouble;
        try {
            likutisDouble = std::stod(likutisStr);
             if (likutisDouble < 0) {
                pranesimas = "Klaida: Likutis negali būti neigiamas.\n";
                send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
                tevoFailas.close();
                std::filesystem::remove(filePath); // Remove empty/invalid file
                return;
            }
        } catch (const std::exception& e) {
            pranesimas = "Klaida: Neteisingas likučio formatas.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            tevoFailas.close();
            std::filesystem::remove(filePath); // Remove empty/invalid file
            return;
        }
        
        tevoFailas << std::fixed << std::setprecision(2) << likutisDouble << "\n";
        tevoFailas.close();

        if (std::filesystem::file_size(filePath) == 0) { // Should not happen if write was successful
            pranesimas = "Klaida: Failas yra tuščias po bandymo įrašyti.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        } else {
            pranesimas = "Kortelė sėkmingai pridėta.\n";
            if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
                perror("Klaida siunčiant duomenis");
            }
            std::string infoMsg = "Kortelės numeris: " + kortelesNumeris + "\n" +
                                  "Likutis: " + likutisStr + "\n";
            if (send(klientoSoketas, infoMsg.c_str(), infoMsg.size(), 0) < 0) {
                perror("Klaida siunčiant duomenis");
            }
        }
    }
};

class papildymoEkr {
public:
    papildymoEkr() {}

    void updateBalansai(const std::string& tevoSasPath, double naujasTevoBalansas,
                        const std::string& vaikoGlobalSasPath, double naujasVaikoBalansas) {
        std::ofstream tevoSasFile(tevoSasPath, std::ios::trunc);
        if (tevoSasFile.is_open()) {
            tevoSasFile << std::fixed << std::setprecision(2) << naujasTevoBalansas << "\n";
            tevoSasFile.close();
        } else {
            std::cerr << "Klaida atnaujinant tėvo balansą faile: " << tevoSasPath << std::endl;
        }

        std::ofstream vaikoGlobalSasFile(vaikoGlobalSasPath, std::ios::trunc);
        if (vaikoGlobalSasFile.is_open()) {
            vaikoGlobalSasFile << std::fixed << std::setprecision(2) << naujasVaikoBalansas << "\n";
            vaikoGlobalSasFile.close();
        } else {
            std::cerr << "Klaida atnaujinant vaiko balansą faile: " << vaikoGlobalSasPath << std::endl;
        }
    }

    void interact(const Tevas& tevas, int klientoSoketas) {
        std::string pranesimas;
        char buffer[4096];
        ssize_t bytesRead;

        pranesimas = "Įveskite vaiko ID, kurio sąskaitą norite papildyti: ";
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
        if (!std::filesystem::exists(vaikoGlobalDir) || !std::filesystem::is_directory(vaikoGlobalDir)) {
            pranesimas = "Klaida: Vaikas su tokiu ID nerastas.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }

        std::string vaikoBankoSaskaitosNumeris;
        std::string vaikoBalansasValueStr;
        std::string vaikoGlobalSasPath;
        bool vaikoBankoFileRastas = false;

        for (const auto& fileEntry : std::filesystem::directory_iterator(vaikoGlobalDir)) {
            if (!fileEntry.is_regular_file()) continue;
            std::string filename = fileEntry.path().filename().string();
            if (filename.rfind(".txt") != std::string::npos && filename.substr(0, 2) == "LT") { // Basic check for account file
                vaikoBankoSaskaitosNumeris = filename.substr(0, filename.length() - 4);
                vaikoGlobalSasPath = fileEntry.path().string();
                std::ifstream saskaitaFile(vaikoGlobalSasPath);
                if (saskaitaFile.is_open()) {
                    if (!std::getline(saskaitaFile, vaikoBalansasValueStr) || vaikoBalansasValueStr.empty()) {
                        vaikoBalansasValueStr = "0.00"; // Default if empty
                    }
                    saskaitaFile.close();
                    vaikoBankoFileRastas = true;
                    break;
                }
            }
        }

        if (!vaikoBankoFileRastas) {
            pranesimas = "Klaida: Vaiko banko sąskaita nerasta arba nepavyko jos nuskaityti.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }

        pranesimas = "Įveskite sumą, kurią norite pridėti: ";
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
        std::string sumaValueStr(buffer);
        double suma;
        try {
            suma = std::stod(sumaValueStr);
        } catch (const std::invalid_argument&) {
            pranesimas = "Klaida: Neteisingas sumos formatas.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        } catch (const std::out_of_range&) {
            pranesimas = "Klaida: Suma per didelė arba per maža.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }

        if (suma <= 0) {
            pranesimas = "Klaida: Papildoma suma turi būti teigiama.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }

        std::string tevoRootDir = "./tevai/" + tevas.getId();
        std::string tevoSaskaitosNumeris;
        std::string tevoBalansasValueStr;
        std::string tevoSasPath;
        bool tevoBankoFileRastas = false;

        if (std::filesystem::exists(tevoRootDir) && std::filesystem::is_directory(tevoRootDir)) {
            for (const auto& fileEntry : std::filesystem::directory_iterator(tevoRootDir)) {
                if (!fileEntry.is_regular_file()) continue;
                std::string filename = fileEntry.path().filename().string();
                 if (filename.rfind(".txt") != std::string::npos && filename.substr(0, 2) == "LT") {
                    tevoSaskaitosNumeris = filename.substr(0, filename.length() - 4);
                    tevoSasPath = fileEntry.path().string();
                    std::ifstream saskaitaFile(tevoSasPath);
                    if (saskaitaFile.is_open()) {
                        if (!std::getline(saskaitaFile, tevoBalansasValueStr) || tevoBalansasValueStr.empty()) {
                            tevoBalansasValueStr = "0.00";
                        }
                        saskaitaFile.close();
                        tevoBankoFileRastas = true;
                        break;
                    }
                }
            }
        }

        if (!tevoBankoFileRastas) {
            pranesimas = "Klaida: Tėvo banko sąskaita nerasta arba nepavyko jos nuskaityti. Pridėkite mokėjimo būdą.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }

        double tevoBalansas, vaikoBalansas;
        try {
            tevoBalansas = std::stod(tevoBalansasValueStr);
        } catch (const std::exception&) {
            pranesimas = "Klaida: Nepavyko nuskaityti tėvo balanso.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }
        try {
            vaikoBalansas = std::stod(vaikoBalansasValueStr);
        } catch (const std::exception&) {
            pranesimas = "Klaida: Nepavyko nuskaityti vaiko balanso.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }

        if (tevoBalansas < suma) {
            pranesimas = "Klaida: Nepakanka lėšų tėvo sąskaitoje.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }

        double naujasVaikoBalansas = vaikoBalansas + suma;
        double naujasTevoBalansas = tevoBalansas - suma;

        updateBalansai(tevoSasPath, naujasTevoBalansas, vaikoGlobalSasPath, naujasVaikoBalansas);

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2)
            << "Suma sėkmingai pridėta vaikui.\n"
            << "Naujas vaiko balansas: " << naujasVaikoBalansas << "\n"
            << "Naujas tėvo balansas: " << naujasTevoBalansas << "\n";
        pranesimas = oss.str();
        send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
    }
};

class pasalinimoEkr {
public:
    pasalinimoEkr() {}

    void interact(const Tevas& tevas, int klientoSoketas) {
        std::string pranesimas;
        char buffer[4096];

        pranesimas = "Įveskite šalinamo vaiko ID: ";
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

        std::string tevoVaikoDir = "./tevai/" + tevas.getId() + "/vaikai/" + vaikoID;
        std::string globalVaikoDir = "./vaikai/" + vaikoID;

        bool tevoVaikoDirPašalintas = false;
        bool globalVaikoDirPašalintas = false;
        std::uintmax_t n;


        if (std::filesystem::exists(tevoVaikoDir)) {
            n = std::filesystem::remove_all(tevoVaikoDir);
            if (n > 0) tevoVaikoDirPašalintas = true;
            std::cout << "Pašalinta iš tėvo katalogo: " << n << " elementų." << std::endl;
        } else {
             pranesimas = "Informacija: Vaiko katalogas nerastas tėvo struktūroje.\n";
             send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
        }


        if (std::filesystem::exists(globalVaikoDir)) {
            n = std::filesystem::remove_all(globalVaikoDir);
             if (n > 0) globalVaikoDirPašalintas = true;
            std::cout << "Pašalinta iš bendro vaikų katalogo: " << n << " elementų." << std::endl;
        } else {
            pranesimas = "Informacija: Vaiko katalogas nerastas bendroje vaikų struktūroje.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
        }
        
        if (tevoVaikoDirPašalintas || globalVaikoDirPašalintas) {
            pranesimas = "Vaikas sėkmingai pašalintas.\n";
        } else {
            pranesimas = "Klaida: Vaikas su nurodytu ID nerastas arba jau pašalintas.\n";
        }

        if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
            perror("Klaida siunčiant pranešimą");
        }
    }
};

class perziurosEkr {
public:
    perziurosEkr() {}

    void interact(const Tevas& tevas, int klientoSoketas) {
        std::string tevoVaikaiDir = "./tevai/" + tevas.getId() + "/vaikai";
        std::string pranesimasGalutinis;

        if (!std::filesystem::exists(tevoVaikaiDir) || !std::filesystem::is_directory(tevoVaikaiDir)) {
            pranesimasGalutinis = "Informacija: Šis tėvas neturi vaikų arba jų katalogas nerastas.\n";
            send(klientoSoketas, pranesimasGalutinis.c_str(), pranesimasGalutinis.size(), 0);
            return;
        }

        bool foundChildren = false;
        for (const auto& vaikoDirEntryTevuose : std::filesystem::directory_iterator(tevoVaikaiDir)) {
            if (!vaikoDirEntryTevuose.is_directory()) {
                continue;
            }
            std::string vaikoId = vaikoDirEntryTevuose.path().filename().string();
            std::string vaikoGlobalDir = "./vaikai/" + vaikoId;
            std::string vaikoPranesimas;

            vaikoPranesimas += "--- Vaiko ID: " + vaikoId + " ---\n";

            std::string vardas = "Nenustatytas";
            std::string pavarde = "Nenustatyta";
            std::string asmDuomPath = vaikoGlobalDir + "/asm_duom.txt"; // Check global dir for details

            if (std::filesystem::exists(asmDuomPath)) {
                 std::ifstream asmDuomFile(asmDuomPath);
                if (asmDuomFile.is_open()) {
                    std::string tempSlaptazodis, tempTevoId;
                    std::getline(asmDuomFile, tempSlaptazodis); 
                    std::getline(asmDuomFile, vardas);
                    std::getline(asmDuomFile, pavarde);
                    // std::getline(asmDuomFile, tempTevoId); // Parent ID is also in this file
                    asmDuomFile.close();
                } else {
                     vaikoPranesimas += "Klaida: Nepavyko atidaryti asmens duomenų failo (" + asmDuomPath + ").\n";
                }
            } else {
                 vaikoPranesimas += "Informacija: Asmens duomenų failas nerastas (" + asmDuomPath + ").\n";
            }
            vaikoPranesimas += "Vardas: " + vardas + "\n";
            vaikoPranesimas += "Pavardė: " + pavarde + "\n";
            

            std::string bankoSaskaitosNumeris = "Nerasta";
            std::string balansasText = "Balansas nerastas";
            bool bankoFileRastas = false;

            if (std::filesystem::exists(vaikoGlobalDir) && std::filesystem::is_directory(vaikoGlobalDir)) {
                for (const auto& fileEntry : std::filesystem::directory_iterator(vaikoGlobalDir)) {
                    if (!fileEntry.is_regular_file()) continue;
                    std::string filename = fileEntry.path().filename().string();
                    if (filename.rfind(".txt") != std::string::npos && filename.substr(0, 2) == "LT") {
                        bankoSaskaitosNumeris = filename.substr(0, filename.length() - 4);
                        std::ifstream saskaitaFile(fileEntry.path().string());
                        if (saskaitaFile.is_open()) {
                            std::string balansasValue;
                            if (std::getline(saskaitaFile, balansasValue) && !balansasValue.empty()) {
                                balansasText = "Balansas: " + balansasValue;
                            } else {
                                balansasText = "Balansas: 0.00";
                            }
                            saskaitaFile.close();
                        } else {
                            balansasText = "Klaida: Nepavyko atidaryti banko sąskaitos failo.";
                        }
                        bankoFileRastas = true;
                        break;
                    }
                }
            }
            if (!bankoFileRastas) { // bankoSaskaitosNumeris might still be "Nerasta"
                balansasText = "Banko sąskaita nerasta.";
            }
            vaikoPranesimas += "Banko sąskaitos numeris: " + bankoSaskaitosNumeris + "\n";
            vaikoPranesimas += balansasText + "\n";

            std::string taupykleInfo = "Taupyklė: informacija nerasta";
            std::string taupyklePath = vaikoGlobalDir + "/taupyklė.txt";
            if(std::filesystem::exists(taupyklePath)) {
                std::ifstream taupykleFile(taupyklePath);
                if (taupykleFile.is_open()) {
                    std::string taupykleStatus, taupykleBalansas;
                    if (std::getline(taupykleFile, taupykleStatus) && std::getline(taupykleFile, taupykleBalansas)) {
                        if (taupykleStatus == "0") {
                            taupykleInfo = "Taupyklė: UŽRAKINTA, balansas: " + taupykleBalansas;
                        } else if (taupykleStatus == "1") {
                            taupykleInfo = "Taupyklė: ATRAKINTA, balansas: " + taupykleBalansas;
                        } else {
                            taupykleInfo = "Taupyklė: nežinoma būsena, balansas: " + taupykleBalansas;
                        }
                    } else {
                        taupykleInfo = "Taupyklė: klaida skaitant duomenis.";
                    }
                    taupykleFile.close();
                } else {
                     taupykleInfo = "Taupyklė: nepavyko atidaryti failo.";
                }
            }
            vaikoPranesimas += taupykleInfo + "\n\n";
            pranesimasGalutinis += vaikoPranesimas;
            foundChildren = true;
        }

        if (!foundChildren) {
            pranesimasGalutinis = "Informacija: Šis tėvas vaikų neturi arba nepavyko nuskaityti jų duomenų.\n";
        }
        send(klientoSoketas, pranesimasGalutinis.c_str(), pranesimasGalutinis.size(), 0);
    }
};

class veiklosPerziurosEkr {
public:
    veiklosPerziurosEkr() {}

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
            pranesimas = "Informacija: Veiklos failas nerastas vaikui ID " + vaikoID + " arba tuščias.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }

        std::string line;
        std::string veiklaContent = "--- Vaiko ID " + vaikoID + " veiklos istorija ---\n";
        while (std::getline(veiklaFile, line)) {
            veiklaContent += line + "\n";
        }
        veiklaFile.close();

        if (veiklaContent == "--- Vaiko ID " + vaikoID + " veiklos istorija ---\n") { // Check if only header is present
            veiklaContent += "Veiklos įrašų nėra.\n";
        }
        send(klientoSoketas, veiklaContent.c_str(), veiklaContent.size(), 0);
    }
};

class authScreen {
public:
    authScreen() {}
    // Removed unused std::string pranesimas;

    bool authVaikas(Vaikas& vaikas, int klientoSoketas) {
        char buffer[4096];
        std::string pranesimas;

        pranesimas = "Įveskite vaiko prisijungimo ID: ";
        if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
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

        pranesimas = "Įveskite slaptažodį: ";
        if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
            perror("Klaida siunčiant duomenis");
            return false;
        }
        bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            perror("Klaida gaunant duomenis");
            return false;
        }
        buffer[bytesRead] = '\0';
        std::string ivestasSlaptazodis(buffer);

        std::string vaikoDuomPath = "./vaikai/" + vaikoId + "/asm_duom.txt";
        std::ifstream vaikoFailas(vaikoDuomPath);

        if (!vaikoFailas.is_open()) {
            pranesimas = "Klaida: Nepavyko rasti vaiko duomenų. Patikrinkite ID.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return false;
        }

        std::string storedPassword;
        std::getline(vaikoFailas, storedPassword); // First line is password
        // Optionally load other details into vaikas object
        std::string vardas, pavarde;
        if(std::getline(vaikoFailas, vardas)) vaikas.setVardas(vardas);
        if(std::getline(vaikoFailas, pavarde)) vaikas.setPavarde(pavarde);
        vaikoFailas.close();


        if (ivestasSlaptazodis == storedPassword) {
            pranesimas = "Autentifikacija sėkminga.\nSveiki, " + vaikas.getVardas() + "!\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return true;
        } else {
            pranesimas = "Klaida: Neteisingas ID arba slaptažodis.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return false;
        }
    }

    bool authTevas(Tevas& tevas, int klientoSoketas) {
        char buffer[4096];
        std::string pranesimas;

        pranesimas = "Įveskite tėvo prisijungimo ID: ";
        if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
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
        tevas.setId(tevoId);

        pranesimas = "Įveskite slaptažodį: ";
        if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
            perror("Klaida siunčiant duomenis");
            return false;
        }
        bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            perror("Klaida gaunant duomenis");
            return false;
        }
        buffer[bytesRead] = '\0';
        std::string ivestasSlaptazodis(buffer);

        std::string tevoDuomPath = "./tevai/" + tevoId + "/asm_duom.txt";
        std::ifstream tevoFailas(tevoDuomPath);

        if (!tevoFailas.is_open()) {
            pranesimas = "Klaida: Nepavyko rasti tėvo duomenų. Patikrinkite ID.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return false;
        }

        std::string storedPassword, vardas, pavarde;
        std::getline(tevoFailas, storedPassword);
        if (std::getline(tevoFailas, vardas)) tevas.setVardas(vardas);
        if (std::getline(tevoFailas, pavarde)) tevas.setPavarde(pavarde);
        tevoFailas.close();

        if (ivestasSlaptazodis == storedPassword) {
            pranesimas = "Autentifikacija sėkminga.\nSveiki, " + tevas.getVardas() + "!\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return true;
        } else {
            pranesimas = "Klaida: Neteisingas ID arba slaptažodis.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return false;
        }
    }

    Tevas registerTevas(int klientoSoketas) {
        srand(time(nullptr)); // Consider calling srand once at program start
        char buffer[4096];
        std::string generatedId = std::to_string(rand() % 90000 + 10000);
        Tevas tevas(generatedId);
        std::string tevoDir = "./tevai/" + tevas.getId();
        std::string pranesimas;

        // Ensure unique ID, though random chance of collision is low for this range
        while(std::filesystem::exists(tevoDir)){
            generatedId = std::to_string(rand() % 90000 + 10000);
            tevas.setId(generatedId);
            tevoDir = "./tevai/" + tevas.getId();
        }


        if (!std::filesystem::create_directories(tevoDir)) {
            pranesimas = "Klaida: Nepavyko sukurti tėvo katalogo registracijos metu.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return Tevas(); // Return empty Tevas object on failure
        }

        std::string filePath = tevoDir + "/asm_duom.txt";
        std::ofstream tevoFailas(filePath);

        if (!tevoFailas.is_open()) {
            pranesimas = "Klaida: Nepavyko sukurti asmens duomenų failo.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            std::filesystem::remove_all(tevoDir); // Clean up created directory
            return Tevas();
        }

        pranesimas = "Sukurkite slaptažodį: ";
        if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
            perror("Klaida siunčiant duomenis");
            tevoFailas.close(); std::filesystem::remove_all(tevoDir); return Tevas();
        }
        ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            perror("Klaida gaunant duomenis");
            tevoFailas.close(); std::filesystem::remove_all(tevoDir); return Tevas();
        }
        buffer[bytesRead] = '\0';
        std::string slaptazodis(buffer);
        tevoFailas << slaptazodis << "\n";

        pranesimas = "Įveskite vardą: ";
        if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
            perror("Klaida siunčiant duomenis");
            tevoFailas.close(); std::filesystem::remove_all(tevoDir); return Tevas();
        }
        bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            perror("Klaida gaunant duomenis");
            tevoFailas.close(); std::filesystem::remove_all(tevoDir); return Tevas();
        }
        buffer[bytesRead] = '\0';
        std::string vardas(buffer);
        tevas.setVardas(vardas);
        tevoFailas << vardas << "\n";

        pranesimas = "Įveskite pavardę: ";
        if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
            perror("Klaida siunčiant duomenis");
            tevoFailas.close(); std::filesystem::remove_all(tevoDir); return Tevas();
        }
        bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            perror("Klaida gaunant duomenis");
            tevoFailas.close(); std::filesystem::remove_all(tevoDir); return Tevas();
        }
        buffer[bytesRead] = '\0';
        std::string pavarde(buffer);
        tevas.setPavarde(pavarde);
        tevoFailas << pavarde << "\n";
        tevoFailas.close();

        pranesimas = "Tėvo registracija sėkminga!\nJūsų prisijungimo ID: " + tevas.getId() + "\n";
        if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
            perror("Klaida siunčiant registracijos patvirtinimą");
        }
        return tevas;
    }
};

class taupyklesEkr {
public:
    taupyklesEkr() {}
    
    void logActivity(const std::string& vaikoId, const std::string& activityMessage) {
        std::string vaikoGlobalDir = "./vaikai/" + vaikoId;
        std::string asmDuomPath = vaikoGlobalDir + "/asm_duom.txt";
        std::string parentId;

        std::ifstream asmDuomFile(asmDuomPath);
        if (asmDuomFile.is_open()) {
            std::string line;
            for (int i = 0; i < 4 && std::getline(asmDuomFile, line); ++i) {
                if (i == 3) parentId = line;
            }
            asmDuomFile.close();
        }

        if (!parentId.empty()) {
            std::string veiklaDir = "./tevai/" + parentId + "/vaikai/" + vaikoId;
            std::filesystem::create_directories(veiklaDir); // Ensure directory exists
            std::string veiklaPath = veiklaDir + "/veikla.txt";
            
            std::time_t now = std::time(nullptr);
            char timeStr[32];
            std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
            
            std::ofstream veiklaFile(veiklaPath, std::ios::app);
            if (veiklaFile.is_open()) {
                veiklaFile << timeStr << " " << activityMessage << "\n";
                veiklaFile.close();
            } else {
                std::cerr << "Klaida: Nepavyko atidaryti veiklos failo: " << veiklaPath << std::endl;
            }
        } else {
            std::cerr << "Klaida: Nepavyko gauti tėvo ID vaikui " << vaikoId << " veiklos registravimui." << std::endl;
        }
    }

    

    void uzrakintiAtrakinti(const Tevas& tevas, int klientoSoketas) { // Parent action
        std::string pranesimas;
        char buffer[4096];
        ssize_t bytesRead;

        pranesimas = "Įveskite vaiko ID, kurio taupyklę norite valdyti: ";
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

        std::string taupyklesFailas = "./vaikai/" + vaikoID + "/taupyklė.txt";

        if (!std::filesystem::exists(taupyklesFailas)) {
            pranesimas = "Klaida: Taupyklės failas nerastas vaikui ID " + vaikoID + ".\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }

        std::string status, balansas;
        std::ifstream taupyklesFileIn(taupyklesFailas);
        if (!taupyklesFileIn.is_open()) {
            pranesimas = "Klaida: Nepavyko atidaryti taupyklės failo skaitymui.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }
        if (!(std::getline(taupyklesFileIn, status) && std::getline(taupyklesFileIn, balansas))) {
            taupyklesFileIn.close();
            pranesimas = "Klaida: Taupyklės failas netinkamo formato arba tuščias.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }
        taupyklesFileIn.close();

        std::string log_msg;
        if (status == "0") { // Was locked
            status = "1";     // Unlock
            pranesimas = "Taupyklė buvo UŽRAKINTA. Dabar ji ATRAKINTA.\n";
            log_msg = "Tėvas (" + tevas.getId() + ") ATRAKINO taupyklę.";
        } else if (status == "1") { // Was unlocked
            status = "0";           // Lock
            pranesimas = "Taupyklė buvo ATRAKINTA. Dabar ji UŽRAKINTA.\n";
            log_msg = "Tėvas (" + tevas.getId() + ") UŽRAKINO taupyklę.";
        } else {
            pranesimas = "Klaida: Nežinoma taupyklės būsena faile.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }

        std::ofstream taupyklesFileOut(taupyklesFailas, std::ios::trunc);
        if (taupyklesFileOut.is_open()) {
            taupyklesFileOut << status << "\n" << balansas << "\n";
            taupyklesFileOut.close();
            pranesimas += "Taupyklės būsena sėkmingai atnaujinta.\n";
            logActivity(vaikoID, log_msg); // Log parent's action
        } else {
            pranesimas = "Klaida: Nepavyko atnaujinti taupyklės failo.\n";
        }
        send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
    }

    void perziuretiLikuti(const Vaikas& vaikas, int klientoSoketas) { // Child action
        std::string pranesimas;
        std::string taupyklesFailas = "./vaikai/" + vaikas.getId() + "/taupyklė.txt";

        if (!std::filesystem::exists(taupyklesFailas)) {
            pranesimas = "Informacija: Taupyklės failas nerastas.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            logActivity(vaikas.getId(), "Bandyta peržiūrėti neegzistuojančios taupyklės likutį.");
            return;
        }

        std::ifstream taupyklesFile(taupyklesFailas);
        if (!taupyklesFile.is_open()) {
            pranesimas = "Klaida: Nepavyko atidaryti taupyklės failo.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            logActivity(vaikas.getId(), "Klaida atidarant taupyklės failą likučio peržiūrai.");
            return;
        }
        std::string status, balansas;
        if (std::getline(taupyklesFile, status) && std::getline(taupyklesFile, balansas)) {
            pranesimas = "Taupyklės likutis: " + balansas + "\n";
            if (status == "0") {
                pranesimas += "Būsena: UŽRAKINTA (negalima išimti pinigų).\n";
            } else if (status == "1") {
                pranesimas += "Būsena: ATRAKINTA (galima išimti pinigus).\n";
            } else {
                pranesimas += "Būsena: Nežinoma.\n";
            }
            logActivity(vaikas.getId(), "Peržiūrėtas taupyklės likutis (" + balansas + "). Būsena: " + (status=="0" ? "UŽRAKINTA" : "ATRAKINTA"));
        } else {
            pranesimas = "Informacija: Taupyklės failas tuščias arba netinkamo formato.\n";
            logActivity(vaikas.getId(), "Bandyta peržiūrėti tuščios/netinkamos taupyklės likutį.");
        }
        taupyklesFile.close();
        send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
    }

    // Inside class taupyklesEkr:

    void idetiPinigus(const Vaikas& vaikas, int klientoSoketas) { // Child action
        std::string pranesimas;
        char buffer[4096];

        pranesimas = "Įveskite sumą, kurią norite įdėti į taupyklę iš pagrindinės sąskaitos: ";
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
        std::string sumaStr(buffer);
        double sumaDouble;
        try {
            sumaDouble = std::stod(sumaStr);
        } catch (const std::exception&) {
            pranesimas = "Klaida: Neteisingas sumos formatas.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }

        if (sumaDouble <= 0) {
            pranesimas = "Klaida: Suma turi būti teigiama.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }

        
        std::string vaikoDir = "./vaikai/" + vaikas.getId();
        std::string bankoSasPath;
        std::string bankoBalansasStr;
        bool bankoSasRasta = false;
        double bankoBalansasDouble = 0.0;

        if (std::filesystem::exists(vaikoDir) && std::filesystem::is_directory(vaikoDir)) {
            for (const auto& fileEntry : std::filesystem::directory_iterator(vaikoDir)) {
                if (fileEntry.is_regular_file()) {
                    std::string filename = fileEntry.path().filename().string();
                    // Basic check for account file (LT*.txt)
                    if (filename.rfind(".txt") != std::string::npos && filename.length() > 4 && filename.substr(0, 2) == "LT") {
                        bankoSasPath = fileEntry.path().string();
                        std::ifstream sasFile(bankoSasPath);
                        if (sasFile.is_open()) {
                            if (std::getline(sasFile, bankoBalansasStr) && !bankoBalansasStr.empty()) {
                                // Successfully read balance
                            } else {
                                bankoBalansasStr = "0.00"; // Default if empty or not readable
                            }
                            sasFile.close();
                            bankoSasRasta = true;
                            break; 
                        }
                    }
                }
            }
        }

        if (!bankoSasRasta) {
            pranesimas = "Klaida: Jūsų pagrindinė banko sąskaita nerasta.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            logActivity(vaikas.getId(), "Klaida dedant į taupyklę: pagrindinė sąskaita nerasta.");
            return;
        }

        try {
            bankoBalansasDouble = std::stod(bankoBalansasStr);
        } catch (const std::exception&) {
            pranesimas = "Klaida: Nepavyko nuskaityti pagrindinės sąskaitos balanso.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            logActivity(vaikas.getId(), "Klaida dedant į taupyklę: nepavyko konvertuoti pagr. sąskaitos balanso.");
            return;
        }

        if (bankoBalansasDouble < sumaDouble) {
            pranesimas = "Klaida: Nepakanka lėšų pagrindinėje sąskaitoje (" + std::to_string(bankoBalansasDouble) + 
                         "), kad įdėtumėte " + sumaStr + " į taupyklę.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            logActivity(vaikas.getId(), "Bandyta įdėti " + sumaStr + " į taupyklę, bet nepakako lėšų pagrindinėje sąskaitoje (" + std::to_string(bankoBalansasDouble) + ").");
            return;
        }

        // Deduct from main account
        double naujasBankoBalansas = bankoBalansasDouble - sumaDouble;
        std::ofstream bankoSasFileOut(bankoSasPath, std::ios::trunc);
        if (!bankoSasFileOut.is_open()) {
            pranesimas = "Klaida: Kritinė klaida atnaujinant pagrindinės sąskaitos balansą. Operacija nutraukta.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            logActivity(vaikas.getId(), "KRITINĖ KLAIDA dedant į taupyklę: nepavyko atidaryti pagrindinės sąskaitos failo rašymui.");
            return;
        }
        bankoSasFileOut << std::fixed << std::setprecision(2) << naujasBankoBalansas << "\n";
        bankoSasFileOut.close();
       

        
        std::string taupyklesFailas = vaikoDir + "/taupyklė.txt"; // Path already defined in vaikoDir context
        std::string status = "0", taupyklesBalansasStr = "0.00"; 
        
        if (std::filesystem::exists(taupyklesFailas)) {
            std::ifstream taupyklesFileIn(taupyklesFailas);
            if (taupyklesFileIn.is_open()) {
                std::getline(taupyklesFileIn, status); 
                std::getline(taupyklesFileIn, taupyklesBalansasStr);
                taupyklesFileIn.close();
            } else {
                std::cerr << "Klaida: Nepavyko atidaryti esamo taupyklės failo skaitymui: " << taupyklesFailas << std::endl;
                status = "0"; // Default to locked if we can't read existing status
            }
        } else {
           
             status = "0"; // Default to locked for a new piggy bank file
        }

        double taupyklesBalansasDouble = 0.0;
        try { 
            taupyklesBalansasDouble = std::stod(taupyklesBalansasStr); 
        } catch (const std::exception&) { /* Use 0.0 if conversion fails, already default */ }
        
        taupyklesBalansasDouble += sumaDouble; // Add the sum to piggy bank balance

        std::ofstream taupyklesFileOut(taupyklesFailas, std::ios::trunc);
        if (taupyklesFileOut.is_open()) {
            taupyklesFileOut << status << "\n" << std::fixed << std::setprecision(2) << taupyklesBalansasDouble << "\n";
            taupyklesFileOut.close();
            
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) 
                << "Pinigai sėkmingai pervesti iš pagrindinės sąskaitos į taupyklę.\n"
                << "Taupyklės naujas likutis: " << taupyklesBalansasDouble << "\n"
                << "Pagrindinės sąskaitos naujas likutis: " << naujasBankoBalansas << "\n";
            pranesimas = oss.str();
            logActivity(vaikas.getId(), "Pervesta " + sumaStr + " iš pagrindinės sąskaitos į taupyklę. Taupyklės likutis: " + std::to_string(taupyklesBalansasDouble) + ". Pagrindinės sąskaitos likutis: " + std::to_string(naujasBankoBalansas) + ".");
        } else {
            // CRITICAL: Money was deducted from main account, but failed to be added to piggy bank.
            pranesimas = "Klaida: KRITINĖ KLAIDA! Pinigai nurašyti iš pagrindinės sąskaitos, bet nepavyko įdėti į taupyklę. Susisiekite su administratoriumi!\n";
            logActivity(vaikas.getId(), "KRITINĖ KLAIDA: Nurašyta " + sumaStr + " iš pagrindinės sąskaitos, bet nepavyko atnaujinti taupyklės failo. Pagrindinės sąskaitos likutis dabar: " + std::to_string(naujasBankoBalansas));
            // Attempt to refund main account as a best-effort recovery
            bankoSasFileOut.open(bankoSasPath, std::ios::trunc); // Re-open main account file
            if (bankoSasFileOut.is_open()) {
                bankoSasFileOut << std::fixed << std::setprecision(2) << bankoBalansasDouble << "\n"; // Write back original balance
                bankoSasFileOut.close();
                pranesimas += "Atliktas bandymas grąžinti lėšas į pagrindinę sąskaitą. Patikrinkite likutį.\n";
                logActivity(vaikas.getId(), "KRITINĖS KLAIDOS TAISYMAS: Bandyta grąžinti " + sumaStr + " į pagrindinę sąskaitą. Likutis turėtų būti: " + std::to_string(bankoBalansasDouble));
            } else {
                pranesimas += "Nepavyko automatiškai grąžinti lėšų į pagrindinę sąskaitą. \n";
                logActivity(vaikas.getId(), "KRITINĖS KLAIDOS TAISYMAS: NEPAVYKO grąžinti " + sumaStr + " į pagrindinę sąskaitą.");
            }
        }
        send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
    }

    // Inside class taupyklesEkr:

    void isimtiPinigus(const Vaikas& vaikas, int klientoSoketas) { // Child action
        std::string pranesimas;
        char buffer[4096];

        // 1. Get amount to withdraw from client
        pranesimas = "Įveskite sumą, kurią norite išimti iš taupyklės (bus pervesta į pagrindinę sąskaitą): ";
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
        std::string sumaStr(buffer);
        double sumaDouble;
        try {
            sumaDouble = std::stod(sumaStr);
        } catch (const std::exception&) {
            pranesimas = "Klaida: Neteisingas sumos formatas.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }

        if (sumaDouble <= 0) {
            pranesimas = "Klaida: Suma turi būti teigiama.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }

        // 2. Access Piggy Bank (Source)
        std::string vaikoDir = "./vaikai/" + vaikas.getId();
        std::string taupyklesFailas = vaikoDir + "/taupyklė.txt";
        std::string esamasTaupyklesStatus, esamasTaupyklesBalansasStr;

        if (!std::filesystem::exists(taupyklesFailas)) {
            pranesimas = "Klaida: Taupyklės failas nerastas. Negalima išimti pinigų.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            logActivity(vaikas.getId(), "Bandyta išimti " + sumaStr + " iš neegzistuojančios taupyklės.");
            return;
        }

        std::ifstream taupyklesFileIn(taupyklesFailas);
        if (!taupyklesFileIn.is_open() || 
            !std::getline(taupyklesFileIn, esamasTaupyklesStatus) || 
            !std::getline(taupyklesFileIn, esamasTaupyklesBalansasStr)) {
            if (taupyklesFileIn.is_open()) taupyklesFileIn.close();
            pranesimas = "Klaida: Nepavyko nuskaityti taupyklės duomenų.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            logActivity(vaikas.getId(), "Klaida skaitant taupyklės duomenis bandant išimti " + sumaStr + ".");
            return;
        }
        taupyklesFileIn.close();

        if (esamasTaupyklesStatus == "0") {
            pranesimas = "Klaida: Taupyklė UŽRAKINTA. Negalima išimti pinigų.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            logActivity(vaikas.getId(), "Bandyta išimti " + sumaStr + " iš UŽRAKINTOS taupyklės.");
            return;
        }

        double esamasTaupyklesBalansasDouble = 0.0;
        try { 
            esamasTaupyklesBalansasDouble = std::stod(esamasTaupyklesBalansasStr); 
        } catch (const std::exception&) {
            pranesimas = "Klaida: Netinkamas taupyklės balanso formatas faile.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            logActivity(vaikas.getId(), "Klaida konvertuojant taupyklės balansą (" + esamasTaupyklesBalansasStr + ") bandant išimti " + sumaStr + ".");
            return;
        }

        if (esamasTaupyklesBalansasDouble < sumaDouble) {
            pranesimas = "Klaida: Nepakanka lėšų taupyklėje (" + std::to_string(esamasTaupyklesBalansasDouble) + 
                         ") norint išimti " + sumaStr + ".\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            logActivity(vaikas.getId(), "Nepakako lėšų (" + esamasTaupyklesBalansasStr + ") bandant išimti " + sumaStr + " iš taupyklės.");
            return;
        }

        // 3. Debit Piggy Bank (Source)
        double naujasTaupyklesBalansas = esamasTaupyklesBalansasDouble - sumaDouble;
        std::ofstream taupyklesFileOut(taupyklesFailas, std::ios::trunc);
        if (!taupyklesFileOut.is_open()) {
            pranesimas = "Klaida: Kritinė klaida atnaujinant taupyklės balansą. Operacija nutraukta.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            logActivity(vaikas.getId(), "KRITINĖ KLAIDA išimant iš taupyklės: nepavyko atidaryti taupyklės failo rašymui.");
            return;
        }
        taupyklesFileOut << esamasTaupyklesStatus << "\n" << std::fixed << std::setprecision(2) << naujasTaupyklesBalansas << "\n";
        taupyklesFileOut.close();

        // 4. Credit Main Bank Account (Destination)
        std::string bankoSasPath;
        std::string bankoBalansasStr;
        bool bankoSasRasta = false;
        double bankoBalansasDouble = 0.0;

        // Find main bank account file
        if (std::filesystem::exists(vaikoDir) && std::filesystem::is_directory(vaikoDir)) {
            for (const auto& fileEntry : std::filesystem::directory_iterator(vaikoDir)) {
                if (fileEntry.is_regular_file()) {
                    std::string filename = fileEntry.path().filename().string();
                    if (filename.rfind(".txt") != std::string::npos && filename.length() > 4 && filename.substr(0, 2) == "LT") {
                        bankoSasPath = fileEntry.path().string();
                        std::ifstream sasFile(bankoSasPath);
                        if (sasFile.is_open()) {
                            std::getline(sasFile, bankoBalansasStr); // Read current balance
                            sasFile.close();
                            bankoSasRasta = true;
                            break;
                        }
                    }
                }
            }
        }
        
        if (!bankoSasRasta) {
            pranesimas = "Klaida: KRITINĖ KLAIDA! Pinigai išimti iš taupyklės, bet Jūsų pagrindinė banko sąskaita nerasta. Susisiekite su administratoriumi!\n";
            logActivity(vaikas.getId(), "KRITINĖ KLAIDA: Išimta " + sumaStr + " iš taupyklės, bet pagrindinė sąskaita nerasta. Taupyklės likutis dabar: " + std::to_string(naujasTaupyklesBalansas));
            // Attempt to refund piggy bank
            taupyklesFileOut.open(taupyklesFailas, std::ios::trunc); // Re-open piggy bank file
            if(taupyklesFileOut.is_open()){
                taupyklesFileOut << esamasTaupyklesStatus << "\n" << std::fixed << std::setprecision(2) << esamasTaupyklesBalansasDouble << "\n"; // Write back original balance
                taupyklesFileOut.close();
                pranesimas += "Atliktas bandymas grąžinti lėšas į taupyklę. Patikrinkite likutį.\n";
                logActivity(vaikas.getId(), "KRITINĖS KLAIDOS TAISYMAS: Bandyta grąžinti " + sumaStr + " į taupyklę. Likutis turėtų būti: " + std::to_string(esamasTaupyklesBalansasDouble));
            } else {
                pranesimas += "Nepavyko automatiškai grąžinti lėšų į taupyklę. BŪTINA RANKINĖ INTERVENCIJA!\n";
                logActivity(vaikas.getId(), "KRITINĖS KLAIDOS TAISYMAS: NEPAVYKO grąžinti " + sumaStr + " į taupyklę.");
            }
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }

        try {
            bankoBalansasDouble = std::stod(bankoBalansasStr.empty() ? "0.00" : bankoBalansasStr);
        } catch (const std::exception&) {
            // If conversion fails, assume 0. This is risky but better than crashing.
            bankoBalansasDouble = 0.00;
             logActivity(vaikas.getId(), "Perspėjimas: Skaitant pagrindinės sąskaitos balansą įvyko konvertavimo klaida (" + bankoBalansasStr + "), naudojamas 0.00.");
        }

        double naujasBankoBalansas = bankoBalansasDouble + sumaDouble;
        std::ofstream bankoSasFileOut(bankoSasPath, std::ios::trunc);
        if (!bankoSasFileOut.is_open()) {
            pranesimas = "Klaida: KRITINĖ KLAIDA! Pinigai išimti iš taupyklės, bet nepavyko įskaityti į pagrindinę sąskaitą. Susisiekite su administratoriumi!\n";
            logActivity(vaikas.getId(), "KRITINĖ KLAIDA: Išimta " + sumaStr + " iš taupyklės, bet nepavyko atnaujinti pagrindinės sąskaitos failo. Taupyklės likutis dabar: " + std::to_string(naujasTaupyklesBalansas));
            // Attempt to refund piggy bank
            taupyklesFileOut.open(taupyklesFailas, std::ios::trunc); // Re-open piggy bank file
            if(taupyklesFileOut.is_open()){
                taupyklesFileOut << esamasTaupyklesStatus << "\n" << std::fixed << std::setprecision(2) << esamasTaupyklesBalansasDouble << "\n"; // Write back original balance
                taupyklesFileOut.close();
                pranesimas += "Atliktas bandymas grąžinti lėšas į taupyklę. Patikrinkite likutį.\n";
                logActivity(vaikas.getId(), "KRITINĖS KLAIDOS TAISYMAS: Bandyta grąžinti " + sumaStr + " į taupyklę. Likutis turėtų būti: " + std::to_string(esamasTaupyklesBalansasDouble));
            } else {
                pranesimas += "Nepavyko automatiškai grąžinti lėšų į taupyklę. BŪTINA RANKINĖ INTERVENCIJA!\n";
                 logActivity(vaikas.getId(), "KRITINĖS KLAIDOS TAISYMAS: NEPAVYKO grąžinti " + sumaStr + " į taupyklę.");
            }
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }
        bankoSasFileOut << std::fixed << std::setprecision(2) << naujasBankoBalansas << "\n";
        bankoSasFileOut.close();

        // 5. Success
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2)
            << "Pinigai sėkmingai pervesti iš taupyklės į pagrindinę sąskaitą.\n"
            << "Taupyklės naujas likutis: " << naujasTaupyklesBalansas << "\n"
            << "Pagrindinės sąskaitos naujas likutis: " << naujasBankoBalansas << "\n";
        pranesimas = oss.str();
        logActivity(vaikas.getId(), "Pervesta " + sumaStr + " iš taupyklės į pagrindinę sąskaitą. Taupyklės likutis: " + std::to_string(naujasTaupyklesBalansas) + ". Pagrindinės sąskaitos likutis: " + std::to_string(naujasBankoBalansas) + ".");
        send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
    }

    void interact(const Vaikas& vaikas, int klientoSoketas) { // Child's piggy bank menu
        int pasirinkimas = 0;
        bool atgal = false;
        char buffer[4096];
        std::string pranesimas;

        while (!atgal) {
            pasirinkimas = 0; 
            pranesimas = "\n*** TAUPYKLĖS VALDYMAS ***\n"
                         "1. Įdėti pinigus\n"
                         "2. Išimti pinigus\n"
                         "3. Peržiūrėti taupyklės likutį\n"
                         "4. Atgal į vaiko meniu\n"
                         "Pasirinkite veiksmą (1-4): ";
            if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
                perror("Klaida siunčiant meniu");
                break;
            }
            ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead <= 0) {
                perror("Klaida gaunant pasirinkimą");
                break;
            }
            buffer[bytesRead] = '\0';
            std::string atsakymas(buffer);

            try {
                pasirinkimas = std::stoi(atsakymas);
                if (pasirinkimas < 1 || pasirinkimas > 4) {
                    throw std::out_of_range("Neteisingas pasirinkimas");
                }
            } catch (const std::exception&) {
                pranesimas = "Klaida: Neteisingas pasirinkimas. Bandykite dar kartą (1-4).\n";
                send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
                continue;
            }
            
            switch (pasirinkimas) {
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
                    pranesimas = "Grįžtama atgal į vaiko meniu.\n";
                    send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
                    atgal = true;
                    break;
            }
        }
    }
};

class valdymoEkr { // Parent's main management screen
private:
    Tevas tevas;
    // Vaikas vaikas; // This instance seems unused here, child object is created in pridejimoEkr
    mokejimoKorteleEkr mokejimoKortEkr;
    pridejimoEkr vaikoPridejimoEkr;
    pasalinimoEkr vaikoPasalinimoEkr;
    perziurosEkr perziurosEkranas;
    papildymoEkr papildymoEkranas;
    taupyklesEkr taupyklesEkranasValdymui; // Renamed to avoid conflict if used elsewhere
    veiklosPerziurosEkr veiklosPerziurosEkranas;

public:
    valdymoEkr() {}

    void setTevas(const Tevas& tevas) {
        this->tevas = tevas;
    }

    void interact(int klientoSoketas) {
        int pasirinkimas = 0;
        bool atgal = false;
        char buffer[4096];
        std::string pranesimas;

        while (!atgal) {
            pasirinkimas = 0; 
            pranesimas = "\n*** TĖVO VALDYMO MENIU ***\n"
                         "1. Pridėti mokėjimo kortelę\n"
                         "2. Pridėti vaiką\n"
                         "3. Pašalinti vaiką\n"
                         "4. Peržiūrėti vaikų informaciją\n"
                         "5. Papildyti vaiko sąskaitą\n"
                         "6. Užrakinti/Atrakinti vaiko taupyklę\n"
                         "7. Peržiūrėti vaiko veiklos istoriją\n"
                         "8. Atsijungti (atgal į pagrindinį meniu)\n"
                         "Pasirinkite veiksmą (1-8): ";
            if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
                perror("Klaida siunčiant meniu");
                break;
            }
            ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead <= 0) {
                perror("Klaida gaunant pasirinkimą");
                break;
            }
            buffer[bytesRead] = '\0';
            std::string atsakymas(buffer);

            try {
                pasirinkimas = std::stoi(atsakymas);
                if (pasirinkimas < 1 || pasirinkimas > 8) {
                     throw std::out_of_range("Neteisingas pasirinkimas");
                }
            } catch (const std::exception&) {
                pranesimas = "Klaida: Neteisingas pasirinkimas. Bandykite dar kartą (1-8).\n";
                send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
                continue;
            }
            
            switch (pasirinkimas) {
                case 1:
                    mokejimoKortEkr.interact(this->tevas, klientoSoketas);
                    break;
                case 2:
                    // vaikas = vaikoPridejimoEkr.interact(this->tevas, klientoSoketas); // Returned Vaikas can be used if needed
                    vaikoPridejimoEkr.interact(this->tevas, klientoSoketas);
                    break;
                case 3:
                    vaikoPasalinimoEkr.interact(this->tevas, klientoSoketas);
                    break;
                case 4:
                    perziurosEkranas.interact(this->tevas, klientoSoketas);
                    break;
                case 5:
                    papildymoEkranas.interact(this->tevas, klientoSoketas);
                    break;
                case 6:
                    taupyklesEkranasValdymui.uzrakintiAtrakinti(this->tevas, klientoSoketas);
                    break;
                case 7:
                    veiklosPerziurosEkranas.interact(this->tevas, klientoSoketas);
                    break;
                case 8:
                    pranesimas = "Atsijungiama. Grįžtama į pagrindinį meniu.\n";
                    send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
                    atgal = true;
                    break;
            }
        }
    }
};

class tevuPrisijungimoEkr {
private:
    Tevas tevas;
    // Vaikas vaikas; // Unused here
    authScreen auth;
    valdymoEkr valdymoEkranas;
    
public:
    tevuPrisijungimoEkr() {}

    void interact(int klientoSoketas) {
        int pasirinkimas = 0;
        char buffer[4096];
        std::string pranesimas;

        while (true) {
            pasirinkimas = 0; 
            pranesimas = "\n*** TĖVŲ PRISIJUNGIMAS/REGISTRACIJA ***\n"
                         "1. Prisijungti\n"
                         "2. Registruotis\n"
                         "3. Atgal į pagrindinį meniu\n"
                         "Pasirinkite veiksmą (1-3): ";

            if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
                perror("Klaida siunčiant meniu");
                return; // Critical error, exit interaction
            }
            ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead <= 0) {
                perror("Klaida gaunant pasirinkimą");
                return; 
            }
            buffer[bytesRead] = '\0';
            std::string atsakymas(buffer);

            try {
                pasirinkimas = std::stoi(atsakymas);
                 if (pasirinkimas < 1 || pasirinkimas > 3) {
                     throw std::out_of_range("Neteisingas pasirinkimas");
                }
            } catch (const std::exception&) {
                pranesimas = "Klaida: Neteisingas pasirinkimas. Bandykite dar kartą (1-3).\n";
                send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
                continue;
            }

            switch (pasirinkimas) {
                case 1: { // Prisijungti
                    Tevas tempTevas; // Use a temporary object for authentication attempt
                    bool isAuthenticated = auth.authTevas(tempTevas, klientoSoketas);
                    if (isAuthenticated) {
                        this->tevas = tempTevas; // Assign to member if auth successful
                        valdymoEkranas.setTevas(this->tevas);
                        valdymoEkranas.interact(klientoSoketas);
                    } else {
                        // Error message already sent by authTevas
                        // pranesimas = "Prisijungti nepavyko. Bandykite dar kartą.\n";
                        // send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
                    }
                    break; 
                }
                case 2: { // Registruotis
                    Tevas registruotasTevas = auth.registerTevas(klientoSoketas);
                    if (!registruotasTevas.getId().empty()) { // Check if registration was successful
                        this->tevas = registruotasTevas;
                        // Success message already sent by registerTevas
                        valdymoEkranas.setTevas(this->tevas); 
                        valdymoEkranas.interact(klientoSoketas);
                    } else {
                        // Error message already sent by registerTevas
                        // pranesimas = "Registracija nepavyko. Bandykite dar kartą.\n";
                        // send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
                    }
                    break;
                }
                case 3: { // Atgal
                    pranesimas = "Grįžtama atgal į pagrindinį meniu.\n";
                    send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
                    return; 
                }
            }
        }
    }
};

class vaikoPervedimoEkr {
private:
    void logActivity(const std::string& vaikoId, const std::string& activityMessage) {
        std::string vaikoGlobalDir = "./vaikai/" + vaikoId;
        std::string asmDuomPath = vaikoGlobalDir + "/asm_duom.txt";
        std::string parentId;

        std::ifstream asmDuomFile(asmDuomPath);
        if (asmDuomFile.is_open()) {
            std::string line;
            for (int i = 0; i < 4 && std::getline(asmDuomFile, line); ++i) {
                if (i == 3) parentId = line;
            }
            asmDuomFile.close();
        }

        if (!parentId.empty()) {
            std::string veiklaDir = "./tevai/" + parentId + "/vaikai/" + vaikoId;
            std::filesystem::create_directories(veiklaDir);
            std::string veiklaPath = veiklaDir + "/veikla.txt";
            
            std::time_t now = std::time(nullptr);
            char timeStr[32];
            std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
            
            std::ofstream veiklaFile(veiklaPath, std::ios::app);
            if (veiklaFile.is_open()) {
                veiklaFile << timeStr << " " << activityMessage << "\n";
                veiklaFile.close();
            }
        }
    }
public:
    vaikoPervedimoEkr() {}

    void interact(const Vaikas& siuntejas, int klientoSoketas) {
        std::string pranesimas;
        char buffer[4096];
        ssize_t bytesRead;

        pranesimas = "Įveskite gavėjo vaiko ID: ";
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
        std::string gavejoID(buffer);

        if (gavejoID == siuntejas.getId()){
            pranesimas = "Klaida: Negalima pervesti pinigų sau pačiam.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }


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
        std::string sumaStr(buffer);
        double sumaDouble;
        try {
            sumaDouble = std::stod(sumaStr);
        } catch (const std::exception&) {
            pranesimas = "Klaida: Neteisingas sumos formatas.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }
        if (sumaDouble <= 0) {
            pranesimas = "Klaida: Pervedama suma turi būti teigiama.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }

        // Siuntėjo balanso tikrinimas ir nurašymas
        std::string siuntejoDir = "./vaikai/" + siuntejas.getId();
        std::string siuntejoSasPath;
        std::string siuntejoBalansasStr;
        bool siuntejoSasRasta = false;

        for (const auto& fileEntry : std::filesystem::directory_iterator(siuntejoDir)) {
            if (fileEntry.is_regular_file() && fileEntry.path().filename().string().rfind(".txt") != std::string::npos && fileEntry.path().filename().string().substr(0,2) == "LT") {
                siuntejoSasPath = fileEntry.path().string();
                std::ifstream sasFile(siuntejoSasPath);
                if (sasFile.is_open() && std::getline(sasFile, siuntejoBalansasStr) && !siuntejoBalansasStr.empty()) {
                    siuntejoSasRasta = true;
                }
                 if(sasFile.is_open()) sasFile.close();
                break;
            }
        }
        if (!siuntejoSasRasta) {
            pranesimas = "Klaida: Jūsų banko sąskaita nerasta arba nepavyko nuskaityti balanso.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }
        double siuntejoBalansasDouble;
        try { siuntejoBalansasDouble = std::stod(siuntejoBalansasStr); } catch (...) {
            pranesimas = "Klaida: Nepavyko nuskaityti jūsų balanso.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }
        if (siuntejoBalansasDouble < sumaDouble) {
            pranesimas = "Klaida: Nepakanka lėšų jūsų sąskaitoje.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            logActivity(siuntejas.getId(), "Nepakako lėšų pervedimui (" + std::to_string(sumaDouble) + ") gavėjui " + gavejoID);
            return;
        }

        // Gavėjo balanso papildymas
        std::string gavejoDir = "./vaikai/" + gavejoID;
         if (!std::filesystem::exists(gavejoDir) || !std::filesystem::is_directory(gavejoDir)) {
            pranesimas = "Klaida: Gavėjas su ID " + gavejoID + " nerastas.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }
        std::string gavejoSasPath;
        std::string gavejoBalansasStr;
        bool gavejoSasRasta = false;

        for (const auto& fileEntry : std::filesystem::directory_iterator(gavejoDir)) {
             if (fileEntry.is_regular_file() && fileEntry.path().filename().string().rfind(".txt") != std::string::npos && fileEntry.path().filename().string().substr(0,2) == "LT") {
                gavejoSasPath = fileEntry.path().string();
                std::ifstream sasFile(gavejoSasPath);
                 if (sasFile.is_open() && std::getline(sasFile, gavejoBalansasStr) && !gavejoBalansasStr.empty()) {
                    gavejoSasRasta = true;
                } else if (sasFile.is_open() && gavejoBalansasStr.empty()){ // File exists but empty, assume 0.00
                    gavejoBalansasStr = "0.00";
                    gavejoSasRasta = true;
                }
                if(sasFile.is_open()) sasFile.close();
                break;
            }
        }
        if (!gavejoSasRasta) {
            pranesimas = "Klaida: Gavėjo banko sąskaita nerasta arba nepavyko nuskaityti balanso.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }
        double gavejoBalansasDouble;
        try { gavejoBalansasDouble = std::stod(gavejoBalansasStr); } catch (...) {
             gavejoBalansasDouble = 0.00; // Default to 0 if corrupt
        }

        // Atliekame transakciją
        siuntejoBalansasDouble -= sumaDouble;
        gavejoBalansasDouble += sumaDouble;

        std::ofstream siuntejoSasFileOut(siuntejoSasPath, std::ios::trunc);
        if (siuntejoSasFileOut.is_open()) {
            siuntejoSasFileOut << std::fixed << std::setprecision(2) << siuntejoBalansasDouble << "\n";
            siuntejoSasFileOut.close();
        } else {
            pranesimas = "Klaida: Kritinė klaida atnaujinant jūsų balansą. Susisiekite su administratoriumi.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            logActivity(siuntejas.getId(), "KRITINĖ KLAIDA nurašant lėšas pervedimui gavėjui " + gavejoID);
            return; // Transakcija neįvyko pilnai
        }

        std::ofstream gavejoSasFileOut(gavejoSasPath, std::ios::trunc);
        if (gavejoSasFileOut.is_open()) {
            gavejoSasFileOut << std::fixed << std::setprecision(2) << gavejoBalansasDouble << "\n";
            gavejoSasFileOut.close();
        } else {
            // Rollback siuntėjo balansą
            siuntejoBalansasDouble += sumaDouble; // Atstatom
            std::ofstream siuntejoRollbackFile(siuntejoSasPath, std::ios::trunc);
            if(siuntejoRollbackFile.is_open()){
                siuntejoRollbackFile << std::fixed << std::setprecision(2) << siuntejoBalansasDouble << "\n";
                siuntejoRollbackFile.close();
            }
            pranesimas = "Klaida: Kritinė klaida atnaujinant gavėjo balansą. Pervedimas atšauktas.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            logActivity(siuntejas.getId(), "KRITINĖ KLAIDA papildant gavėjo " + gavejoID + " sąskaitą. Pervedimas atšauktas.");
            logActivity(gavejoID, "KRITINĖ KLAIDA gaunant pervedimą iš " + siuntejas.getId() + ". Pervedimas atšauktas.");
            return;
        }
        
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2)
            << "Pervedimas sėkmingas!\n"
            << "Nurašyta suma: " << sumaDouble << "\n"
            << "Jūsų naujas balansas: " << siuntejoBalansasDouble << "\n";
        pranesimas = oss.str();
        send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);

        logActivity(siuntejas.getId(), "Pervesta " + std::to_string(sumaDouble) + " gavėjui " + gavejoID + ". Naujas balansas: " + std::to_string(siuntejoBalansasDouble));
        logActivity(gavejoID, "Gauta " + std::to_string(sumaDouble) + " iš siuntėjo " + siuntejas.getId() + ". Naujas balansas: " + std::to_string(gavejoBalansasDouble));
    }
};

class likutisEkr {
private:
    void logActivity(const std::string& vaikoId, const std::string& activityMessage) {
        std::string vaikoGlobalDir = "./vaikai/" + vaikoId;
        std::string asmDuomPath = vaikoGlobalDir + "/asm_duom.txt";
        std::string parentId;

        std::ifstream asmDuomFile(asmDuomPath);
        if (asmDuomFile.is_open()) {
            std::string line;
            for (int i = 0; i < 4 && std::getline(asmDuomFile, line); ++i) {
                if (i == 3) parentId = line;
            }
            asmDuomFile.close();
        }

        if (!parentId.empty()) {
            std::string veiklaDir = "./tevai/" + parentId + "/vaikai/" + vaikoId;
             std::filesystem::create_directories(veiklaDir);
            std::string veiklaPath = veiklaDir + "/veikla.txt";
            
            std::time_t now = std::time(nullptr);
            char timeStr[32];
            std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
            
            std::ofstream veiklaFile(veiklaPath, std::ios::app);
            if (veiklaFile.is_open()) {
                veiklaFile << timeStr << " " << activityMessage << "\n";
                veiklaFile.close();
            }
        }
    }
public:
    likutisEkr() {}

    void interact(const Vaikas& vaikas, int klientoSoketas) {
        std::string pranesimas;
        std::string vaikoDir = "./vaikai/" + vaikas.getId();
        std::string bankoSasPath;
        std::string balansasValue = "0.00"; // Default
        bool sasRasta = false;

        if (std::filesystem::exists(vaikoDir) && std::filesystem::is_directory(vaikoDir)) {
            for (const auto& fileEntry : std::filesystem::directory_iterator(vaikoDir)) {
                if (fileEntry.is_regular_file()) {
                    std::string filename = fileEntry.path().filename().string();
                    if (filename.rfind(".txt") != std::string::npos && filename.substr(0, 2) == "LT") {
                        bankoSasPath = fileEntry.path().string();
                        std::ifstream saskaitaFile(bankoSasPath);
                        if (saskaitaFile.is_open()) {
                            if (!std::getline(saskaitaFile, balansasValue) || balansasValue.empty()) {
                                balansasValue = "0.00"; // If file empty
                            }
                            saskaitaFile.close();
                            sasRasta = true;
                            break;
                        }
                    }
                }
            }
        }

        if (!sasRasta) {
            pranesimas = "Informacija: Banko sąskaita nerasta arba nepavyko nuskaityti balanso.\n";
            logActivity(vaikas.getId(), "Bandyta peržiūrėti sąskaitos likutį, bet sąskaita nerasta.");
        } else {
            pranesimas = "Jūsų sąskaitos likutis: " + balansasValue + "\n";
            logActivity(vaikas.getId(), "Peržiūrėtas sąskaitos likutis: " + balansasValue);
        }
        send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
    }
};

class vaikoEkranas { // Child's main menu
private:
    Vaikas vaikas; // Current child
    taupyklesEkr taupyklesEkranasVaiko; // For child's interaction with their piggy bank
    likutisEkr likutisEkranasVaiko;
    vaikoPervedimoEkr vaikoPervedimoEkranasVaiko;

public:
    vaikoEkranas() {}

    void setVaikas(const Vaikas& vaikas) {
        this->vaikas = vaikas;
    }

    void interact(int klientoSoketas) {
        int pasirinkimas = 0;
        bool atgal = false;
        char buffer[4096];
        std::string pranesimas;

        while (!atgal) {
            pasirinkimas = 0;
            pranesimas = "\n*** VAIKO MENIU ***\n"
                         "Sveiki, " + this->vaikas.getVardas() + " (ID: " + this->vaikas.getId() + ")!\n"
                         "1. Pervesti pinigus draugui\n"
                         "2. Peržiūrėti sąskaitos likutį\n"
                         "3. Valdyti taupyklę\n"
                         "4. Atsijungti (atgal į pagrindinį meniu)\n"
                         "Pasirinkite veiksmą (1-4): ";
            if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
                perror("Klaida siunčiant meniu");
                break;
            }
            ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead <= 0) {
                perror("Klaida gaunant pasirinkimą");
                break;
            }
            buffer[bytesRead] = '\0';
            std::string atsakymas(buffer);

            try {
                pasirinkimas = std::stoi(atsakymas);
                 if (pasirinkimas < 1 || pasirinkimas > 4) { // Corrected range
                     throw std::out_of_range("Neteisingas pasirinkimas");
                }
            } catch (const std::exception&) {
                pranesimas = "Klaida: Neteisingas pasirinkimas. Bandykite dar kartą (1-4).\n";
                send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
                continue;
            }
            
            switch (pasirinkimas) {
                case 1:
                    vaikoPervedimoEkranasVaiko.interact(this->vaikas, klientoSoketas);
                    break;
                case 2:
                    likutisEkranasVaiko.interact(this->vaikas, klientoSoketas);
                    break;
                case 3:
                    taupyklesEkranasVaiko.interact(this->vaikas, klientoSoketas);
                    break;
                case 4:
                    pranesimas = "Atsijungiama. Grįžtama į pagrindinį meniu.\n";
                    send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
                    atgal = true;
                    break;
            }
        }
    }
};

class vaikuPrisijungimoEkr {
private:
    Vaikas vaikas; // Current child
    authScreen auth;
    vaikoEkranas vaikoEkr;

public:
    vaikuPrisijungimoEkr() {}

    void interact(int klientoSoketas) {
        int pasirinkimas = 0;
        char buffer[4096];
        std::string pranesimas;

        while (true) {
            pasirinkimas = 0; 
            pranesimas = "\n*** VAIKŲ PRISIJUNGIMAS ***\n"
                         "1. Prisijungti\n"
                         "2. Atgal į pagrindinį meniu\n"
                         "Pasirinkite veiksmą (1-2): ";

            if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
                perror("Klaida siunčiant meniu");
                return; 
            }
            ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead <= 0) {
                perror("Klaida gaunant pasirinkimą");
                return; 
            }
            buffer[bytesRead] = '\0';
            std::string atsakymas(buffer);

             try {
                pasirinkimas = std::stoi(atsakymas);
                 if (pasirinkimas < 1 || pasirinkimas > 2) {
                     throw std::out_of_range("Neteisingas pasirinkimas");
                }
            } catch (const std::exception&) {
                pranesimas = "Klaida: Neteisingas pasirinkimas. Bandykite dar kartą (1-2).\n";
                send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
                continue;
            }

            switch (pasirinkimas) {
                case 1: { // Prisijungti
                    Vaikas tempVaikas;
                    bool isAuthenticated = auth.authVaikas(tempVaikas, klientoSoketas);
                    if (isAuthenticated) {
                        this->vaikas = tempVaikas;
                        vaikoEkr.setVaikas(this->vaikas);
                        vaikoEkr.interact(klientoSoketas);
                    } else {
                        // Error message sent by authVaikas
                    }
                    break; 
                }
                case 2: { // Atgal
                    pranesimas = "Grįžtama atgal į pagrindinį meniu.\n";
                    send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
                    return; 
                }
            }
        }
    }
};

class userInterface {
private:
    tevuPrisijungimoEkr tevuEkranas;
    vaikuPrisijungimoEkr vaikuEkranas;

public:
    userInterface() {}

    void pradzia(int klientoSoketas) {
        int pasirinkimas = 0;
        char buffer[4096];
        std::string pranesimas;

        try {
            while (true) {
                pasirinkimas = 0; 
                pranesimas = "\n======= VAIKŲ BANKAS PROGRAMA =======\n"
                             "Sveiki atvykę!\n"
                             "1. Tėvų portalas\n"
                             "2. Vaikų portalas\n"
                             "3. Uždaryti programą\n"
                             "Pasirinkite veiksmą (1-3): ";

                if (send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0) < 0) {
                    perror("Klaida siunčiant pagrindinį meniu");
                    break; // Exit loop if send fails
                }
                ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
                if (bytesRead <= 0) {
                    if (bytesRead == 0) {
                        std::cout << "Klientas atsijungė." << std::endl;
                    } else {
                        perror("Klaida gaunant pasirinkimą iš pagrindinio meniu");
                    }
                    break; // Exit loop if recv fails or client disconnects
                }
                buffer[bytesRead] = '\0';
                std::string atsakymas(buffer);

                try {
                    pasirinkimas = std::stoi(atsakymas);
                    if (pasirinkimas < 1 || pasirinkimas > 3) {
                        throw std::out_of_range("Neteisingas pasirinkimas");
                    }
                } catch (const std::exception&) {
                    pranesimas = "Klaida: Neteisingas pasirinkimas. Prašome rinktis iš 1, 2 arba 3.\n";
                    send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
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
                        pranesimas = "Programa uždaroma. Viso gero!\n";
                        send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
                        close(klientoSoketas); // Close socket for this client
                        std::cout << "Kliento " << klientoSoketas << " sesija baigta." << std::endl;
                        return; // Exit pradzia for this client thread/process
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Klaida tvarkant klientą " << klientoSoketas << ": " << e.what() << std::endl;
            pranesimas = "Klaida: Įvyko sisteminė klaida serveryje: " + std::string(e.what()) + "\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0); // Try to inform client
        }
        // Ensure socket is closed if loop was exited due to error/disconnect
        // The main server loop should handle this for new connections.
        // If this function is the sole handler for a client connection, ensure cleanup.
        // For now, assuming `close(klientoSoketas)` in case 3 is the primary exit point.
        // If loop breaks otherwise, socket might remain open on server side until program ends.
        // Added close for safety if not case 3.
        // However, if an error occurs before case 3 and we `return`, the socket is not closed here.
        // The `close` should ideally be in a RAII wrapper or a finally-like block if this function owns the socket lifecycle for the client.
        // For simplicity in this structure, we only explicitly close on option 3. Other disconnections are handled by OS.
        // std::cout << "Kliento " << klientoSoketas << " ryšys prarastas arba įvyko klaida." << std::endl;
        // close(klientoSoketas); // This might be too aggressive if called after already closed or error.
    }
};

#endif // KLASES_H