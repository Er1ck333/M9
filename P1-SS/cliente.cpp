#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    const char *message = "¡Hola servidor!";
    char buffer[1024] = {0};

    // Crear descriptor de socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Error al crear el socket\n";
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convertir dirección IP
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Dirección inválida/No soportada\n";
        return -1;
    }

    // Conectar al servidor
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Error al conectar al servidor\n";
        return -1;
    }

    std::cout << "Conexión establecida con el servidor.\n";

    // Enviar mensaje al servidor
    send(sock, message, strlen(message), 0);
    std::cout << "Enviando mensaje: " << message << "\n";

    // Recibir respuesta del servidor
    read(sock, buffer, 1024);
    std::cout << "Respuesta del servidor: " << buffer << "\n";

    // Cerrar el socket
    close(sock);

    return 0;
}
