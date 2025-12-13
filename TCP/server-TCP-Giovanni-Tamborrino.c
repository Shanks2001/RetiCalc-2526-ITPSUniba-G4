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
#define QLEN 6

void ErrorHandler(const char* errorMessage) {
    printf("Errore: %s\n", errorMessage);
}

void ClearWinSock() {
    #if defined(WIN32)
        WSACleanup();
    #endif
}

void remove_vowels(char* str, char* dst) {
    int i=0, j=0;
    while (str[i]) {
        char c = str[i];
        char ls = c | 0x20;
        if (ls!='a' && ls!='e' && ls!='i' && ls!='o' && ls!='u') {
            dst[j++] = c;
        }
        i++;
    }
    dst[j] = '\0';
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

    // CREAZIONE SOCKET SERVER
    int Ssocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (Ssocket < 0) {
        ErrorHandler("socket creation failed.\n");
        ClearWinSock();
        return -1;
    }

    // assegnazione INDIRIZZO SERVER
    struct sockaddr_in sad;
    memset(&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    sad.sin_addr.s_addr = INADDR_ANY;
    sad.sin_port = htons(port);

    // BINDING
    if (bind(Ssocket, (struct sockaddr*)&sad, sizeof(sad)) < 0) {
        ErrorHandler("bind falito.\n");
        closesocket(Ssocket);
        ClearWinSock();
        return -1;
    }
    
    // LISTENING
    if (listen(Ssocket, QLEN) < 0) {
    ErrorHandler("listen fallito.\n");
    closesocket(Ssocket);
    ClearWinSock();
    return -1;
}
    

    printf("Server in ascolto sulla porta %d...\n", port);

    // ciclo principale accept e handle client
    while (1) {
        struct sockaddr_in cad;
        int clientLen = sizeof(cad);
        int clientSocket = accept(Ssocket, (struct sockaddr *)&cad, &clientLen);
        if(clientSocket < 0) {
            ErrorHandler("accesso falitto.\n");
            closesocket(Ssocket);
            ClearWinSock();
            return -1;
        }
         printf("ricevuti dati dal client con indirizzo: %s\n", inet_ntoa(cad.sin_addr));
         //ricevo messaggio iniziale "Hello"
        char buf[BUFFERSIZE];
        int bytesRcvd = recv(clientSocket, buf, BUFFERSIZE-1, 0);
            if (bytesRcvd <= 0) {
                ErrorHandler("Ricezione messaggio iniziale fallita o connessione chiusa.\n");
                closesocket(clientSocket);
                continue;
            }
        buf[bytesRcvd] = '\0';
        printf("Messaggio iniziale dal client: %s\n", buf);
            //ricevere stringa inviata dal client
        bytesRcvd = recv(clientSocket, buf, BUFFERSIZE-1, 0);
        if (bytesRcvd <= 0) {
                ErrorHandler("Ricezione stringa fallita o connessione chiusa.\n");
                closesocket(clientSocket);
                continue;
            }
        printf("Stringa ricevuta dal client: %s\n", buf);
        char out[BUFFERSIZE];
        remove_vowels(buf, out);
        //invio la stringa senza vocali al client
        int sendLen = strlen(out);
        if (send(clientSocket, out, sendLen, 0) != sendLen) {
            ErrorHandler("Invio stringa fallito.\n");
            
        }

        // chiusura socket client
        closesocket(clientSocket);
    }
    closesocket(Ssocket);
    ClearWinSock();
    return 0;
}