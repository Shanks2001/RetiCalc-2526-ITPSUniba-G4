#if defined _WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ECHOMAX 255
#define PORT 48000

void ErrorHandler(const char* errorMessage) {
    printf("Errore: %s\n", errorMessage);
}

void ClearWinSock() {
#if defined(WIN32)
    WSACleanup();
#endif
}

void remove_vowels(char* str, char* dst) {
    int i = 0, j = 0;
    while (str[i]) {
        char c = str[i];
        char lc = c | 0x20;
        if (lc!='a' && lc!='e' && lc!='i' && lc!='o' && lc!='u') {
            dst[j++] = c;
        }
        i++;
    }
    dst[j] = '\0';
}

int main(void) {

#if defined WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        printf("Errore WSAStartup\n");
        return 0;
    }
#endif

    int sock;
    struct sockaddr_in servAddr, clntAddr;
    unsigned int clntLen;
    char buffer[ECHOMAX];
    int recvSize;

    /* creazione socket UDP */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        ErrorHandler("Creazione socket fallita");
        return 0;
    }

    /* indirizzo server */
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(PORT);

    /* bind */
    if (bind(sock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
        ErrorHandler("Bind fallito");
        closesocket(sock);
        ClearWinSock();
        return 0;
    }

    printf("Server UDP in ascolto sulla porta %d\n", PORT);

    while (1) {

        clntLen = sizeof(clntAddr);
        recvSize = recvfrom(sock, buffer, ECHOMAX-1, 0,
                            (struct sockaddr*)&clntAddr, &clntLen);

        if (recvSize < 0)
            continue;

        buffer[recvSize] = '\0';

        struct in_addr addr = clntAddr.sin_addr;
        struct hostent *host = gethostbyaddr((char *)&addr, sizeof(addr), AF_INET);
        char *clientName = host ? host->h_name : "Unknown";

        /* HELLO */
        if (strcmp(buffer, "Hello") == 0 || strcmp(buffer, "hello") == 0) {
            printf("ricevuti dati dal client nome:%s indirizzo:%s\n",
                   clientName, inet_ntoa(clntAddr.sin_addr));
            continue;
        }

        /* STRINGA NORMALE */
        printf("Stringa ricevuta: %s\n", buffer);

        char out[ECHOMAX];
        remove_vowels(buffer, out);

        sendto(sock, out, strlen(out), 0,
               (struct sockaddr *)&clntAddr, clntLen);
    }

    closesocket(sock);
    ClearWinSock();
    return 0;
}
