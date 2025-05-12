#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include "bankoSaskaita.h"
#include <fstream>

std::string generuotiSaskaitosNumeri() {
    
    std::string saliesKodas = "LT";

    // Generate a random 2-digit control number (10–99)
    int kontrolinisSkaicius = rand() % 90 + 10;

    std::string bankoKodas = "7300";

    // Split the 11-digit client number into two parts: 5 digits and 6 digits
    int prefix = rand() % 90000 + 10000;       // 5-digit number: 10000–99999
    int suffix = rand() % 900000 + 100000;     // 6-digit number: 100000–999999

    std::string klientoNr = std::to_string(prefix) + std::to_string(suffix);

    return saliesKodas + std::to_string(kontrolinisSkaicius) + bankoKodas + klientoNr;

}

BankoSaskaita::BankoSaskaita(const std::string& vartotojas) {

    std::srand(std::time(0)); // Seed the random number generator
    this->saskaitosNumeris = generuotiSaskaitosNumeri();
    this->balansas = 0.0;
    this->vartotojas = vartotojas;

    std::cout << "Sukurtas banko saskaitos numeris: " << this->saskaitosNumeris << std::endl; 

}

void BankoSaskaita::ideti(double suma) {

    if (suma > 0) {
        this->balansas += suma;
        std::cout << "Iš banko saskaitos ideta: " << suma << " EUR" << std::endl;
    } else {
        std::cout << "Negalima ideti neigiamo sumos." << std::endl;
    }


}

void BankoSaskaita::isimti(double suma) {

    if (suma > 0 && suma <= this->balansas) {
        this->balansas -= suma;
        std::cout << "Iš banko saskaitos atsiimta: " << suma << " EUR" << std::endl;
    } else {
        std::cout << "Negalima atsiimti neigiamo sumos arba viršyti balanso." << std::endl;
    }
}

void BankoSaskaita::pervesti(BankoSaskaita& kitaSaskaita, double suma) {

    if (suma > 0 && suma <= this->balansas) {
        this->balansas -= suma;
        kitaSaskaita.balansas += suma;
        std::cout << "Iš banko saskaitos pervesta: " << suma << " EUR" << std::endl;
    } else {
        std::cout << "Negalima pervesti neigiamo sumos arba viršyti balanso." << std::endl;
    }
}

double BankoSaskaita::gautiBalansa() const {
    return this->balansas;
}

std::string BankoSaskaita::gautiSaskaitosNumeri() const {
    return this->saskaitosNumeris;
}

std::string BankoSaskaita::gautiVartotoja() const {
    return this->vartotojas;
}

void BankoSaskaita::setBalansa(double naujasBalansas) {
    this->balansas = naujasBalansas;
}

void BankoSaskaita::irasytiIFaila() const {

    std::ofstream failas(this->saskaitosNumeris + ".txt");
    if (failas.is_open()) {
        failas << this->balansas;
        failas.close();
        std::cout << "Duomenys sėkmingai įrašyti į failą: " << this->saskaitosNumeris << ".txt" << std::endl;
    } else {
        std::cerr << "Klaida: Nepavyko atidaryti failo įrašymui." << std::endl;
    }
}

std::ostream& operator<<(std::ostream& os, const BankoSaskaita& saskaita) {
    os << "Saskaitos numeris: " << saskaita.gautiSaskaitosNumeri() << "\n";
    os << "Balansas: " << saskaita.gautiBalansa() << " EUR\n";
    os << "Vartotojas: " << saskaita.gautiVartotoja() << "\n";
    return os;
}
