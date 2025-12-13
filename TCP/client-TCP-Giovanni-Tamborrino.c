#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFERSIZE 512
#define PROTOPORT 27015

void ErrorHandler(const char* errorMessage) {
    printf("Errore: %s\n", errorMessage);
}

void ClearWinSock() {
    #if defined(WIN32)
        WSACleanup();
    #endif
}

int main(int argc, char *argv[]) {

    int port = PROTOPORT;

    if(argc > 1) {
        port = atoi(argv[1]);
    }

    #if defined WIN32
        WSADATA wsaData;
        int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
        if (iResult != 0) {
            printf("error at WSAStartup\n");
            return -1;
        }
    #endif

    // CREAZIONE SOCKET CLIENT
    int Csocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (Csocket < 0) {
        ErrorHandler("socket creation failed.\n");
        return -1;
    }

    // COSTRUZIONE INDIRIZZO SERVER
    struct sockaddr_in sad;
    memset(&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    sad.sin_addr.s_addr = inet_addr("127.0.0.1");
    sad.sin_port = htons(port);

    // CONNESSIONE AL SERVER
    if (connect(Csocket, (struct sockaddr*)&sad, sizeof(sad)) < 0) {
        ErrorHandler("Failed to connect.\n");
        closesocket(Csocket);
        ClearWinSock();
        return -1;
        }
    //invia hello al server
    char *hello = "hello";
    int helloLen = strlen(hello);
    if (send(Csocket, hello, helloLen, 0) != helloLen) {
        ErrorHandler("Failed to send hello message.\n");
        return -1;
    }
    
    // leggo striga da stdin e la invio al server
    char buffer[BUFFERSIZE];
    printf("Inserisci una stringa da inviare al server: "); 
    if (fgets(buffer, BUFFERSIZE, stdin) == NULL) {
        ErrorHandler("Failed to read string from stdin.\n");
        closesocket(Csocket);
        ClearWinSock();
        return -1;
    }
    // rimuovi newline
    size_t ln = strlen(buffer);
    if (ln>0 && buffer[ln-1]=='\n') buffer[ln-1] = '\0';
    int sendLen = strlen(buffer);
    if (send(Csocket, buffer, sendLen, 0) != sendLen) {
        ErrorHandler("send() sent a different number of bytes than expected");
    }
    // ricevo risposta dal server e la stampo
    char recvBuffer[BUFFERSIZE];
    int bytesRcvd = recv(Csocket, recvBuffer, BUFFERSIZE - 1, 0);
    if(bytesRcvd <= 0) {
        ErrorHandler("recv erorre o conessione chiusa prematuramente.\n");
    }else {
        recvBuffer[bytesRcvd] = '\0';
        printf("Risposta dal server: %s\n", recvBuffer);
    }
    // chiudo socket e pulisco winsock
    closesocket(Csocket);
    ClearWinSock();
    return 0;
}