#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")  // Enlazar con la librería Winsock

#define PORT 9000

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET sock;
    struct sockaddr_in serv_addr;
    char buffer[1024] = { 0 };

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Error al crear el socket" << std::endl;
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        std::cerr << "Error al conectar con el servidor" << std::endl;
        return 1;
    }

    std::cout << ">> Conexión establecida con el servicio." << std::endl;

    int valread = recv(sock, buffer, sizeof(buffer), 0);
    if (valread > 0) {
        std::cout << ">> Hora y fecha recibidas: " << buffer << std::endl;
    }
    else {
        std::cerr << "Error al recibir datos" << std::endl;
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
