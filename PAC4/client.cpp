// client.cpp
#include <iostream>
#include <string>
#include <winsock2.h>
#include <Ws2tcpip.h> 

#pragma comment(lib, "ws2_32.lib")

void startClient(const std::string& pedido) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup falló." << std::endl;
        return;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "No se pudo crear el socket." << std::endl;
        WSACleanup();
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9100);

    // Reemplazar inet_addr con inet_pton
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0) {
        std::cerr << "Dirección inválida." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "No se pudo conectar al servidor." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    // Enviar el pedido con conversión segura
    send(clientSocket, pedido.c_str(), static_cast<int>(pedido.length()), 0);

    // Recibir el identificador único del pedido con protección de buffer
    char buffer[1024];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "Error al recibir el identificador del pedido." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    if (bytesReceived > 0 && bytesReceived < sizeof(buffer)) {
        buffer[bytesReceived] = '\0';
        std::cout << "Identificador recibido: " << buffer << std::endl;
    }

    closesocket(clientSocket);
    WSACleanup();
}

int main() {
    std::cout << "Conexión establecida con el servicio." << std::endl;
    std::cout << "Introduzca su pedido: ";

    std::string pedido;
    std::getline(std::cin, pedido);

    startClient(pedido);
    return 0;
}
