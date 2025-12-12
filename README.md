# RetiCalc-2526-ITPSUniba-G4
Giovanni Tamborrino

Linux / macOS:

Compilazione: gcc tcp_server.c -o tcp_server e gcc tcp_client.c -o tcp_client (stessa cosa per udp files).

Esegui prima il server: ./tcp_server (o ./tcp_server 27015 per porta diversa).

Poi il client: ./tcp_client (o ./tcp_client 27015).

UDP: ./udp_server e ./udp_client.

Windows: usa Visual Studio / mingw: i file includono i blocchi per Winsock;
