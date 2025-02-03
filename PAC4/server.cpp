#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <mutex>
#include <vector>
#include <winsock2.h>
#include <iomanip> 

#pragma comment(lib, "ws2_32.lib")

std::mutex fileMutex;
int orderCounter = 1;

void handleClient(SOCKET clientSocket) {
    char buffer[1024];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "Error al recibir el pedido." << std::endl;
        closesocket(clientSocket);
        return;
    }

    buffer[bytesReceived] = '\0';
    std::string pedido(buffer);
    std::ostringstream orderID;
    orderID << "ORD-" << std::setw(4) << std::setfill('0') << orderCounter++;

    // Guardar el pedido en el archivo de texto
    std::lock_guard<std::mutex> lock(fileMutex);
    std::ofstream pedidoFile("pedidos.txt", std::ios::app);
    if (pedidoFile.is_open()) {
        pedidoFile << orderID.str() << ": \"" << pedido << "\"" << std::endl;
        pedidoFile.close();
    }
    else {
        std::cerr << "Error al abrir el archivo de pedidos." << std::endl;
    }

    // Enviar el identificador al cliente
    std::string response = orderID.str();
    send(clientSocket, response.c_str(), response.length(), 0);
    closesocket(clientSocket);
}

void startServer() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup falló." << std::endl;
        return;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "No se pudo crear el socket." << std::endl;
        WSACleanup();
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9100);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Error al hacer bind." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        std::cerr << "Error al escuchar." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    std::cout << "Servicio iniciado. Escuchando en el puerto 9100..." << std::endl;

    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Error al aceptar conexión." << std::endl;
            continue;
        }

        std::cout << "Conexión aceptada del cliente." << std::endl;
        std::thread clientThread(handleClient, clientSocket);
        clientThread.detach();
    }

    closesocket(serverSocket);
    WSACleanup();
}

int main() {
    startServer();
    return 0;
}
