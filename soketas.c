#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOKETO_FAILAS "./banko_sim.sock"
#define BUFERIO_DYDIS 4096


void klaida(const char *pranesimas) {
    perror(pranesimas);
    exit(EXIT_FAILURE);
}

int main() {
    int soketas;
    struct sockaddr_un serverio_adresas;
    char buferis[BUFERIO_DYDIS];

    
    if ((soketas = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
        klaida("Nepavyko sukurti soketo...");

    
    memset(&serverio_adresas, 0, sizeof(serverio_adresas));
    memset(buferis, 0, BUFERIO_DYDIS);
    
    serverio_adresas.sun_family = AF_UNIX;
    strncpy(serverio_adresas.sun_path, SOKETO_FAILAS, sizeof(serverio_adresas.sun_path) - 1);

    
    if (connect(soketas, (struct sockaddr *)&serverio_adresas, sizeof(serverio_adresas)) < 0)
        klaida("Nepavyko prisijungti prie serverio ...");

    printf("Prisijungta prie serverio.\n");

    
    memset(buferis, 0, BUFERIO_DYDIS);
    if (recv(soketas, buferis, BUFERIO_DYDIS, 0) > 0)
        printf("%s", buferis);

    
    while (1) {
        printf(" ");
        fgets(buferis, BUFERIO_DYDIS, stdin);

        // Nuvalome \n
        buferis[strcspn(buferis, "\n")] = '\0';
        // Jeigu įvedėme "exit"
        if (strcmp(buferis, "exit") == 0)
            break;

        if (send(soketas, buferis, strlen(buferis), 0) < 0) {
            perror("Klaida siunčiant duomenis į serverį");
            break;
        }

        memset(buferis, 0, BUFERIO_DYDIS);

        ssize_t received = recv(soketas, buferis, BUFERIO_DYDIS, 0);
        if (received > 0) {
            printf("%s", buferis);
        } else if (received == 0) {
            printf("Serveris uždarė ryšį.\n");
            break;
        } else {
            perror("Klaida gaunant duomenis iš serverio");
            break;
        }

        if (strncmp("ATSIJUNGTA", buferis, 10) == 0) 
            break;   
    }

    
    close(soketas);
    printf("Seansas baigėsi.\n\n");

    return 0;
}