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

void ErrorHandler(const char* errorMessage) {
    printf("Errore: %s\n", errorMessage);
}

void ClearWinSock() {
#if defined(WIN32)
    WSACleanup();
#endif
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
    struct sockaddr_in servAddr, fromAddr;
    unsigned int fromSize;
    char buffer[ECHOMAX];
    char reply[ECHOMAX];
    int replyLen;

    char hostbuf[128];
    int port;

    printf("Inserisci il nome dell'host server: ");
    fgets(hostbuf, sizeof(hostbuf), stdin);
    hostbuf[strcspn(hostbuf, "\n")] = '\0';

    printf("Inserisci il numero di porta del server: ");
    scanf("%d", &port);
    getchar();

    /* socket */
    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        ErrorHandler("Creazione socket fallita");
        return 0;
    }

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(port);

    struct hostent *hp = gethostbyname(hostbuf);
    if (hp)
        servAddr.sin_addr = *(struct in_addr *)hp->h_addr;
    else
        servAddr.sin_addr.s_addr = inet_addr(hostbuf);

    printf("Mi connetto al server IP:%s porta:%d\n",
           inet_ntoa(servAddr.sin_addr), port);

    /* HELLO */
    sendto(sock, "Hello", 5, 0,
           (struct sockaddr *)&servAddr, sizeof(servAddr));

    /* stringa */
    printf("Inserisci una stringa da inviare al server: ");
    fgets(buffer, ECHOMAX, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';

    sendto(sock, buffer, strlen(buffer), 0,
           (struct sockaddr *)&servAddr, sizeof(servAddr));

    /* ricezione */
    fromSize = sizeof(fromAddr);
    replyLen = recvfrom(sock, reply, ECHOMAX-1, 0,
                        (struct sockaddr *)&fromAddr, &fromSize);

    reply[replyLen] = '\0';

    struct in_addr addr = fromAddr.sin_addr;
    struct hostent *host = gethostbyaddr((char *)&addr, sizeof(addr), AF_INET);
    char *serverName = host ? host->h_name : "Unknown";

    printf("Stringa %s ricevuta dal server nome:%s indirizzo:%s\n",
           reply, serverName, inet_ntoa(fromAddr.sin_addr));

    closesocket(sock);
    ClearWinSock();
    return 0;
}
