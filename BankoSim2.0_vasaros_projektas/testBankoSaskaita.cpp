#include <iostream>
#include <cassert>
#include <fstream>
#include "bankoSaskaita.h"

void testBankoSaskaita() {
    // Test constructor and getters
    BankoSaskaita saskaita("Jonas");
    assert(saskaita.gautiVartotoja() == "Jonas");
    assert(saskaita.gautiBalansa() == 0.0);
    std::cout << "Saskaitos numeris: " << saskaita.gautiSaskaitosNumeri() << std::endl;

    // Test ideti
    saskaita.ideti(100.0);
    assert(saskaita.gautiBalansa() == 100.0);

    // Test isimti
    saskaita.isimti(50.0);
    assert(saskaita.gautiBalansa() == 50.0);

    // Test isimti with invalid amount
    saskaita.isimti(100.0); // Should not change balance
    assert(saskaita.gautiBalansa() == 50.0);

    // Test pervesti
    BankoSaskaita kitaSaskaita("Petras");
    saskaita.pervesti(kitaSaskaita, 30.0);
    assert(saskaita.gautiBalansa() == 20.0);
    assert(kitaSaskaita.gautiBalansa() == 30.0);

    // Test pervesti with invalid amount
    saskaita.pervesti(kitaSaskaita, 50.0); // Should not change balances
    assert(saskaita.gautiBalansa() == 20.0);
    assert(kitaSaskaita.gautiBalansa() == 30.0);

    // Test irasytiIFaila
    saskaita.irasytiIFaila();
    std::ifstream failas(saskaita.gautiSaskaitosNumeri() + ".txt");
    

    std::cout << saskaita << std::endl;

    std::cout << "Visi testai praejo sekmingai!" << std::endl;
}

int main() {
    testBankoSaskaita();
    return 0;
}
