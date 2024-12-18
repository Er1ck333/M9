#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <mutex>
#include <algorithm>

#pragma comment(lib, "ws2_32.lib")

std::vector<SOCKET> clients;          // Lista de clientes conectados
std::mutex clientsMutex;              // Mutex para proteger la lista de clientes
std::vector<std::string> allowedIPs = {
    "127.0.0.1", // IP mia
    "172.17.20.163", // IP del compañero
};

// Función para verificar si una IP está en la lista de permitidas
bool isIPAllowed(const std::string& clientIP) {
    return std::find(allowedIPs.begin(), allowedIPs.end(), clientIP) != allowedIPs.end();
}

// Función para retransmitir mensajes a todos los clientes (excepto al remitente)
void broadcast_message(const std::string& message, SOCKET senderSocket) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (SOCKET clientSocket : clients) {
        if (clientSocket != senderSocket) { // No reenviar al remitente
            send(clientSocket, message.c_str(), static_cast<int>(message.size()), 0);
        }
    }
}

// Función que maneja a cada cliente
void handle_client(SOCKET clientSocket, sockaddr_in clientAddr) {
    char clientIP[NI_MAXHOST];
    inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, NI_MAXHOST);

    // Verificar si la IP está permitida
    if (!isIPAllowed(clientIP)) {
        std::cout << "Conexión rechazada desde: " << clientIP << std::endl;
        closesocket(clientSocket);
        return;
    }

    std::cout << "Conexión aceptada desde: " << clientIP << std::endl;

    {
        std::lock_guard<std::mutex> lock(clientsMutex);
        clients.push_back(clientSocket); // Añadir cliente a la lista
    }

    char buffer[512];
    int bytesReceived;

    while (true) {
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived <= 0) {
            std::cout << "Cliente desconectado: " << clientIP << std::endl;
            break;
        }

        buffer[bytesReceived] = '\0'; // Asegurar fin de cadena
        std::string message = "Mensaje de " + std::string(clientIP) + ": " + buffer;
        std::cout << message << std::endl;

        // Retransmitir mensaje a todos los demás clientes
        broadcast_message(message, clientSocket);
    }

    // Eliminar cliente de la lista al desconectarse
    {
        std::lock_guard<std::mutex> lock(clientsMutex);
        clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());
    }

    closesocket(clientSocket);
    std::cout << "Socket cerrado para: " << clientIP << std::endl;
}

int main() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cout << "Error al inicializar Winsock: " << result << std::endl;
        return 1;
    }

    SOCKET listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listeningSocket == INVALID_SOCKET) {
        std::cout << "Error al crear socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345); // Puerto
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Escuchar en todas las interfaces

    if (bind(listeningSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "Error en bind: " << WSAGetLastError() << std::endl;
        closesocket(listeningSocket);
        WSACleanup();
        return 1;
    }

    if (listen(listeningSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "Error en listen: " << WSAGetLastError() << std::endl;
        closesocket(listeningSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Servidor iniciado en el puerto 12345. Esperando conexiones..." << std::endl;

    while (true) {
        sockaddr_in clientAddr;
        int clientSize = sizeof(clientAddr);

        SOCKET clientSocket = accept(listeningSocket, (sockaddr*)&clientAddr, &clientSize);
        if (clientSocket == INVALID_SOCKET) {
            std::cout << "Error al aceptar conexión: " << WSAGetLastError() << std::endl;
            continue;
        }

        std::thread clientThread(handle_client, clientSocket, clientAddr);
        clientThread.detach();
    }

    closesocket(listeningSocket);
    WSACleanup();
    return 0;
}