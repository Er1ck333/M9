#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

void receive_messages(SOCKET clientSocket) {
    char buffer[512];
    int bytesReceived;

    while (true) {
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            std::cout << "Desconectado del servidor." << std::endl;
            break;
        }

        buffer[bytesReceived] = '\0';
        std::cout << "Mensaje recibido: " << buffer << std::endl;
    }
}

int main() {
    // Inicializar Winsock
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Crear socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Configurar dirección del servidor
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    inet_pton(AF_INET, "172.17.20.163", &serverAddr.sin_addr);

    // Conectar al servidor
    int iResult = connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (iResult == SOCKET_ERROR) {
        std::cout << "Error al conectar con el servidor." << std::endl;
        WSACleanup();
        return 1;
    }

    std::cout << "Conectado al servidor." << std::endl;

    // Iniciar hilo para recibir mensajes
    std::thread recvThread(receive_messages, clientSocket);
    recvThread.detach();

    // Enviar mensajes al servidor
    std::string mensaje;
    while (true) {
        std::getline(std::cin, mensaje);
        if (mensaje == "/salir") {
            break;
        }
        send(clientSocket, mensaje.c_str(), static_cast<int>(mensaje.size()), 0);
    }

    // Cerrar el socket
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}