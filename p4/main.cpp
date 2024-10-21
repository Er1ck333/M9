#include <iostream>
#include <unistd.h>  // Para fork()
#include <sys/wait.h>  // Para wait()
#include <vector>
#include <string>
#include <cstdlib>  // Para rand() y srand()
#include <ctime>    // Para time()

// Variable global compartida (no compartida entre procesos, pero simula la idea)
std::vector<std::string> nombres;

void procesoHijo(int id, const std::string& nombre_usuario) {
    // Cada proceso añade un nombre
    nombres.push_back(nombre_usuario);

    // Mostrar que el nombre ha sido añadido por este proceso
    std::cout << "Proceso " << id << " añadió el nombre: " << nombre_usuario << std::endl;

    // Elegir aleatoriamente un nombre del array
    if (!nombres.empty()) {
        int indice = rand() % nombres.size();
        std::string nombre_aleatorio = nombres[indice];
        std::cout << "Proceso " << id << " dice: ¡Hola, " << nombre_aleatorio << "!" << std::endl;
    }
}

int main() {
    int N;
    std::cout << "Ingrese la cantidad de procesos a crear: ";
    std::cin >> N;

    // Inicializar el generador de números aleatorios
    srand(time(0));

    // Array global inicial de nombres
    nombres.push_back("Carlos");
    nombres.push_back("Ana");
    nombres.push_back("Maria");

    for (int i = 0; i < N; ++i) {
        std::string nombre_usuario;
        std::cout << "Ingrese un nombre para el proceso " << i + 1 << ": ";
        std::cin >> nombre_usuario;

        pid_t pid = fork();
        
        if (pid == 0) {
            // Proceso hijo
            procesoHijo(i + 1, nombre_usuario);
            _exit(0); // Termina el proceso hijo correctamente
        } else if (pid > 0) {
            // Proceso padre espera que el hijo termine
            wait(NULL);
        } else {
            std::cerr << "Error al crear el proceso." << std::endl;
        }
    }

    return 0;
}
