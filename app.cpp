#include "klases.h"
#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string>

const std::string SOKETO_FAILAS = "./banko_sim.sock";

int app(int klientoSoketas) {
    std::cout << "Placeholder: Pradedamas kliento apdorojimas.\n";

    userInterface user;

    try {
        user.pradzia(klientoSoketas);
    } catch (const std::exception& e) {
        std::cerr << "Klaida tvarkant klientą: " << e.what() << std::endl;
    }

    close(klientoSoketas); // Ensure the socket is closed after handling the client
    std::cout << "Placeholder: Kliento apdorojimas baigtas.\n";
    return 0;
}

void startuokServeri() {
    // Patikrina, ar egzistuoja socket failas ir jei taip, jį pašalina
    if (std::filesystem::exists(SOKETO_FAILAS)) {
        std::filesystem::remove(SOKETO_FAILAS);
    }

    // Sukuria serverio socket'ą
    int serverioSoketas = socket(AF_UNIX, SOCK_STREAM, 0);
    if (serverioSoketas == -1) {
        std::cerr << "Nepavyko sukurti serverio socket'o" << std::endl;
        return; // Vietoj sys.exit(1)
    }

    // Nustato serverio adresą
    sockaddr_un serverioAdresas;
    serverioAdresas.sun_family = AF_UNIX;
    strcpy(serverioAdresas.sun_path, SOKETO_FAILAS.c_str());

    // Susieja socket'ą su adresu
    if (bind(serverioSoketas, (struct sockaddr*)&serverioAdresas, sizeof(serverioAdresas)) == -1) {
        std::cerr << "Nepavyko susieti serverio socket'o" << std::endl;
        close(serverioSoketas);
        return; // Vietoj sys.exit(1)
    }

    // Pradeda klausyti prisijungimų
    if (listen(serverioSoketas, 5) == -1) {
        std::cerr << "Nepavyko pradėti klausyti" << std::endl;
        close(serverioSoketas);
        return; // Vietoj sys.exit(1)
    }

    std::cout << "Serveris įjungtas: " << SOKETO_FAILAS << std::endl;

    try {
        while (true) {
            // Priima kliento prisijungimą
            sockaddr_un klientoAdresas;
            socklen_t klientoAdresoIlgis = sizeof(klientoAdresas);
            int klientoSoketas = accept(serverioSoketas, (struct sockaddr*)&klientoAdresas, &klientoAdresoIlgis);
            if (klientoSoketas == -1) {
                std::cerr << "Nepavyko priimti kliento prisijungimo" << std::endl;
                continue; // Skip to the next iteration instead of stopping the server
            }
            std::cout << "Naujas klientas" << std::endl;

            // Create a thread to handle the client
            std::thread clientThread(app, klientoSoketas);
            clientThread.detach(); // Detach the thread to allow independent execution
        }
    } catch (const std::exception& e) {
        std::cerr << "\nServeris baigia darbą. Įvyko klaida: " << e.what() << std::endl;
    }

    // Uždaro serverio socket'ą ir pašalina socket failą
    close(serverioSoketas);
    std::filesystem::remove(SOKETO_FAILAS);
}

int main() {
    startuokServeri();
    return 0;
}