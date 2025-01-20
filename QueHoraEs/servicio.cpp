#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <ctime>

#pragma comment(lib, "ws2_32.lib")  // Enlazar con la librería Winsock

#define PORT 9000

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        std::cerr << "Error al crear el socket" << std::endl;
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        std::cerr << "Error al enlazar el socket" << std::endl;
        return 1;
    }

    if (listen(server_fd, 3) == SOCKET_ERROR) {
        std::cerr << "Error al escuchar conexiones" << std::endl;
        return 1;
    }

    std::cout << ">> Servicio iniciado. Escuchando en el puerto " << PORT << "..." << std::endl;

    while (true) {
        new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if (new_socket == INVALID_SOCKET) {
            std::cerr << "Error al aceptar conexión" << std::endl;
            continue;
        }

        std::cout << ">> Conexión aceptada desde el cliente" << std::endl;

        // Obtener la fecha y hora actuales de manera segura
        time_t now = time(0);
        struct tm ltm;
        localtime_s(&ltm, &now);  // Versión segura en Windows

        char buffer[80];
        strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", &ltm);

        send(new_socket, buffer, strlen(buffer), 0);
        std::cout << ">> Hora y fecha enviadas: " << buffer << std::endl;

        closesocket(new_socket);
    }

    WSACleanup();
    return 0;
}
