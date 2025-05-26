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
        // Placeholder for the pridejimoEkr functionality
       
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

        vaikas.setVardas(vaikoVardas); // Set the name in the Vaikas object


        std::cout << "DEBUG: Vaiko vardas gautas: " << vaikoVardas << "\n"; // DEBUG

        
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

        vaikas.setPavarde(vaikoPavarde); // Set the surname in the Vaikas object

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

        std::string tevoDir = "./tevai/" + tevas.getId() + "/vaikai" + "/" + vaikas.getId();
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

        vaikas.setBankoSas(bankoSaskaitosNumeris); // Set the bank account number in the Vaikas object


        std::ofstream vaikoFailPasTeva(tevoDir + "/asm_duom.txt");
        std::ofstream vaikoSasPasTeva(tevoDir + "/" + vaikas.getBankoSas() + ".txt");
        
        if (!vaikoFailPasTeva.is_open()) {
            send(klientoSoketas, "Nepavyko sukurti failo.\n", 26, 0);
            return vaikas;
        }

        if (!vaikoSasPasTeva.is_open()) {
            send(klientoSoketas, "Nepavyko sukurti failo.\n", 26, 0);
            return vaikas;
        }

        
        vaikoFailPasTeva << vaikosSlaptazodis << "\n"; // Write password to file
        vaikoFailPasTeva << vaikoVardas << "\n"; // Write name to file
        vaikoFailPasTeva << vaikoPavarde << "\n"; // Write surname to file
        vaikoSasPasTeva << "0\n"; // Write bank account number to file

        vaikoFailPasTeva.close();
        vaikoSasPasTeva.close();

        
        // Check if file with the same name already exists
       
        std::ofstream vaikoFailPasVaika(vaikoDir + "/asm_duom.txt");
        std::ofstream vaikoSasPasVaika(vaikoDir + "/" + vaikas.getBankoSas() + ".txt");

        if (!vaikoSasPasVaika.is_open()) {
            send(klientoSoketas, "Nepavyko sukurti failo.(2)\n", 26, 0);
            return vaikas;
        }

        if (!vaikoFailPasVaika.is_open()) {
            send(klientoSoketas, "Nepavyko sukurti failo.(1)\n", 26, 0);
            return vaikas;
        }

        vaikoFailPasVaika << vaikosSlaptazodis << "\n"; // Write password to file
        vaikoFailPasVaika << vaikoVardas << "\n"; // Write name to file
        vaikoFailPasVaika << vaikoPavarde << "\n"; // Write surname to file
        vaikoSasPasVaika <<"0\n";

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
class papildymoEkr{};
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

        if (!std::filesystem::exists(tevoVaikaiDir)) {
            pranesimas = "Direktorija nerasta.\n";
            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
            return;
        }

        bool found = false;
        for (const auto& entry : std::filesystem::directory_iterator(tevoVaikaiDir)) {
            if (!entry.is_directory()) continue;
            std::string vaikoId = entry.path().filename();
            std::string asmDuomPath = entry.path().string() + "/asm_duom.txt";
            std::ifstream duom(asmDuomPath);
            if (!duom.is_open()) continue;

            std::string vardas, pavarde;
        
            std::getline(duom, vardas);
            std::getline(duom, pavarde);

            // Try to find account and balance
            std::string saskaitaInfo = "Sąskaita nerasta\n";
            std::string vaikoSasDir = "./vaikai/" + vaikoId;
            std::string vaikoSasPath = vaikoSasDir + "/asm_duom.txt";
            std::string balansas = "Nerasta";

            if (std::filesystem::exists(vaikoSasPath)) {
                std::ifstream vaikoSas(vaikoSasPath);
                std::string skip;
                std::getline(vaikoSas, skip); // password
                std::getline(vaikoSas, skip); // vardas
                std::getline(vaikoSas, skip); // pavarde
                if (std::getline(vaikoSas, balansas)) {
                    saskaitaInfo = "Balansas: " + balansas + "\n";
                } else {
                    saskaitaInfo = "Balansas: nerastas\n";
                }
            }

            pranesimas += "Vaiko ID: " + vaikoId + "\n";
            pranesimas += "Vardas: " + vardas + "\n";
            pranesimas += "Pavardė: " + pavarde + "\n";
            pranesimas += saskaitaInfo + "\n";
            found = true;
        }

        if (!found) {
            pranesimas = "Vaikų nėra.\n";
        }

        send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);
    }
};
class veiklosEkr{};


class authScreen {

    public:

        authScreen(){};
        std::string pranesimas;

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

class valdymoEkr{

private:

    Tevas tevas;
    Vaikas vaikas;
    mokejimoKorteleEkr mokejimoKortEkr;
    pridejimoEkr vaikoPridejimoEkr;
    pasalinimoEkr vaikoPasalinimoEkr;

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
