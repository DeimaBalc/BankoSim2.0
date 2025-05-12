#ifndef ADMIN_H
#define ADMIN_H

#include <string>
#include <vector>

class Admin {
private:
    std::string id;       // Administratoriaus ID
    std::string slaptazodis; // Administratoriaus slaptažodis

public:

    Admin(const std::string& id, const std::string& slaptazodis);

    // Gavimo metodai
    std::string getId() const;
    std::string getSlapt() const;

    //setter
    void setSlapt(const std::string& naujasSlaptazodis);

    // Administratoriaus funkcionalumas
    void perziureti_vartotojus(const std::string& vartotojuKatalogas) const;
    void istrinti_vartotoja(const std::string& vartotojuKatalogas, const std::string& vartotojoId) const;
    double tikrinti_banko_balansa(const std::string& bankoSaskaitosFailas) const;
    void ideti_i_banka(const std::string& bankoSaskaitosFailas, double suma) const;
    void atsiimti_is_banko(const std::string& bankoSaskaitosFailas, double suma) const;
    void pervesti_is_banko(const std::string& bankoSaskaitosFailas, const std::string& gavejoSaskaitosFailas, double suma) const;

    // Statinės pagalbinės funkcijos
    static Admin registruoti_administratori(const std::string& id, const std::string& slaptazodis);
    static bool autentifikuoti(const std::string& id, const std::string& slaptazodis, const std::string& adminKatalogas);

    // Pagalbinės funkcijos
    static void issaugoti_administratoriaus_duomenis(const std::string& failas, const Admin& admin);
    static Admin nuskaityti_administratoriaus_duomenis(const std::string& failas);
};



#endif // ADMIN_H