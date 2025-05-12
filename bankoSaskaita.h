#ifndef BANKOSASKAITA_H
#define BANKOSASKAITA_H

#include <string>
#include <vector>
#include <random>
#include <map>


class BankoSaskaita {   

private:
    std::string saskaitosNumeris;
    double balansas;
    std::string vartotojas;

public:

    // Konstruktorius
    BankoSaskaita(const std::string& vartotojas);
    // Funkcijos
    void ideti(double suma);
    void isimti(double suma);
    void pervesti(BankoSaskaita& kitaSaskaita, double suma);
    double gautiBalansa() const;
    std::string gautiSaskaitosNumeri() const;
    std::string gautiVartotoja() const;

    void setBalansa(double naujasBalansas);

    void irasytiIFaila() const;

    friend std::ostream& operator<<(std::ostream& os, const BankoSaskaita& saskaita);
    

};


#endif