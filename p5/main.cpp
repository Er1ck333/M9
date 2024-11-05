#include <iostream>
#include <thread>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <atomic>
#include <chrono>

using namespace std;

const int META = 50; // Meta de la carrera
atomic<bool> carreraFinalizada(false); // Variable para indicar si la carrera ha terminado

void SetCursorPosition(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void carrera(const string& nombre, int velocidadBase, int posY) {
    int posicion = 0;
    srand(time(0) + posY); // Semilla aleatoria diferente para cada hilo
    while (posicion < META && !carreraFinalizada) {
        int avanceAleatorio = (rand() % velocidadBase) + 1; // Avance aleatorio del animal
        posicion += avanceAleatorio;

        if (posicion >= META) {
            carreraFinalizada = true;
            SetCursorPosition(0, 5);
            cout << "El ganador es " << nombre << " con una posicion de " << posicion << "!\n";
        } else {
            SetCursorPosition(0, posY);
            cout << nombre << ": ";
            for (int i = 0; i < posicion; i++) cout << ".";
            cout << " (" << posicion << ")\r" << flush;
        }
        
        // Pausa entre movimientos
        this_thread::sleep_for(chrono::milliseconds(100 + rand() % 200));
    }
}

int main() {
    // Definición de los participantes
    vector<thread> hilos;
    hilos.push_back(thread(carrera, "Tortuga", 3, 1));
    hilos.push_back(thread(carrera, "Liebre", 5, 2));
    hilos.push_back(thread(carrera, "Perro", 4, 3));

    // Esperamos a que todos los hilos terminen
    for (auto& hilo : hilos) {
        hilo.join();
    }

    return 0;
}
