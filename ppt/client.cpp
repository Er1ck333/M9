#include <iostream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddr.sin_port = htons(PORT);

    std::string move;
    std::cout << "Selecciona tu jugada (Piedra, Papel, Tijeras): ";
    std::cin >> move;

    sendto(clientSocket, move.c_str(), move.size(), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));

    char buffer[BUFFER_SIZE];
    int serverAddrSize = sizeof(serverAddr);
    
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytesReceived = recvfrom(clientSocket, buffer, BUFFER_SIZE, 0, (sockaddr*)&serverAddr, &serverAddrSize);
        
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::cout << "Servidor: " << buffer << "\n";

            // Si no es el mensaje de espera, salir del bucle
            if (std::string(buffer).find("Esperando") == std::string::npos) {
                break;
            }
        }
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
