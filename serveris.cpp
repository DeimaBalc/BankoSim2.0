#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string>
#include <filesystem>
#include "admin.h" // Corrected include for user-defined header
#include "AuthScreen.h" // Corrected include for user-defined header
#include <thread> // Added for threading
#include <algorithm> // Added for std::all_of

const std::string SOKETO_FAILAS = "./banko_sim.sock";

// Function to handle client requests
void valdykKlienta(int klientoSoketas) {
    int pasirinkimas = 0;

    try {
        while (!pasirinkimas) {
            std::string pranesimas = (
                "\n\n*** VU BANKAS ***\n\n"
                "1. PRISIJUNGTI\n"
                "2. REGISTRUOTIS\n"
                "3. ADMINISTRATORIUS\n\n"
                "Pasirinkite veiksmą (1-3)"
            );

            send(klientoSoketas, pranesimas.c_str(), pranesimas.size(), 0);

            char buffer[4096];
            ssize_t bytesRead = recv(klientoSoketas, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead <= 0) {
                throw std::runtime_error("Nepavyko gauti atsakymo iš kliento.");
            }

            buffer[bytesRead] = '\0'; // Null-terminate the string
            std::string atsakymas(buffer);

            // Check if the response is valid
            if (atsakymas.empty() || !std::all_of(atsakymas.begin(), atsakymas.end(), ::isdigit) || 
                std::stoi(atsakymas) < 1 || std::stoi(atsakymas) > 3) {
                std::string klaida = "Neteisingas pasirinkimas. Bandykite dar kartą.\n";
                send(klientoSoketas, klaida.c_str(), klaida.size(), 0);
                continue;
            }else {

                pasirinkimas = std::stoi(atsakymas);

            }

            Admin admin("11112222");

            AuthScreen authScreen;

            bool arTas = authScreen.authAdmin(admin, klientoSoketas);

            std::cout << arTas << std::endl;

        }

    

    } catch (const std::exception& e) {
        std::cerr << "Klaida tvarkant klientą: " << e.what() << std::endl;
        std::string klaida = "Įvyko klaida: " + std::string(e.what()) + "\n";
        send(klientoSoketas, klaida.c_str(), klaida.size(), 0);
    }

    close(klientoSoketas); // Close client socket after handling
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
            std::thread clientThread(valdykKlienta, klientoSoketas);
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
