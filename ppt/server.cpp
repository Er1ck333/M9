#include <iostream>
#include <winsock2.h>
#include <map>
#include <string>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

struct Player {
    sockaddr_in address;
    std::string move;
    bool ready = false;
};

std::map<std::string, Player> waitingPlayers;

std::string determineWinner(std::string move1, std::string move2) {
    if (move1 == move2) return "Empate";
    if ((move1 == "Piedra" && move2 == "Tijeras") ||
        (move1 == "Papel" && move2 == "Piedra") ||
        (move1 == "Tijeras" && move2 == "Papel")) {
        return "Jugador 1 gana";
    }
    return "Jugador 2 gana";
}

void sendMessage(SOCKET socket, sockaddr_in& clientAddr, std::string message) {
    sendto(socket, message.c_str(), message.size(), 0, (sockaddr*)&clientAddr, sizeof(clientAddr));
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);
    
    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    std::cout << "Servidor iniciado en el puerto " << PORT << "\n";

    char buffer[BUFFER_SIZE];
    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytesReceived = recvfrom(serverSocket, buffer, BUFFER_SIZE, 0, (sockaddr*)&clientAddr, &clientAddrSize);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::string move(buffer);
            std::string clientKey = inet_ntoa(clientAddr.sin_addr) + std::to_string(ntohs(clientAddr.sin_port));

            if (waitingPlayers.empty()) {
                // Primer jugador en la espera
                waitingPlayers[clientKey] = {clientAddr, move, true};
                sendMessage(serverSocket, clientAddr, "Esperando al oponente...");
            } else {
                // Encontrar al primer jugador esperando
                auto opponent = waitingPlayers.begin();
                std::string result = determineWinner(opponent->second.move, move);

                // Enviar resultados a ambos jugadores
                sendMessage(serverSocket, opponent->second.address, "Tu oponente jugó " + move + ". Resultado: " + result);
                sendMessage(serverSocket, clientAddr, "Tu oponente jugó " + opponent->second.move + ". Resultado: " + result);

                // Eliminar la pareja de la lista de espera
                waitingPlayers.erase(opponent);
            }
        }
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
