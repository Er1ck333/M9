#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    const char *response = "¡Hola desde el servidor!";

    // Crear descriptor de socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Error al crear el socket");
        return -1;
    }

    // Configurar el socket
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Error al configurar el socket");
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Enlazar el socket al puerto
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Error al enlazar el socket");
        return -1;
    }

    // Escuchar conexiones
    if (listen(server_fd, 3) < 0) {
        perror("Error al escuchar");
        return -1;
    }

    std::cout << "Esperando conexiones...\n";

    // Aceptar una conexión
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Error al aceptar conexión");
        return -1;
    }

    // Leer mensaje del cliente
    read(new_socket, buffer, 1024);
    std::cout << "Mensaje recibido del cliente: " << buffer << "\n";

    // Enviar respuesta al cliente
    send(new_socket, response, strlen(response), 0);
    std::cout << "Respuesta enviada al cliente.\n";

    // Cerrar el socket
    close(new_socket);
    close(server_fd);

    return 0;
}
