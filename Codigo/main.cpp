#include <iostream>
#include "controlador.h"

int main() {
    Controlador sistema;
    int opcion = 0;

    do {
        std::cout << "\n=== SISTEMA DE ASISTENCIA Y HORARIOS ===" << std::endl;
        std::cout << "1. Menu Administrador" << std::endl;
        std::cout << "2. Menu Docente" << std::endl;
        std::cout << "3. Salir" << std::endl;
        std::cout << "Seleccione una opcion: ";
        std::cin >> opcion;

        switch (opcion) {
            case 1:
                sistema.menuAdmin();
                break;
            case 2:
                sistema.menuProfesor();
                break;
            case 3:
                std::cout << "Saliendo del programa..." << std::endl;
                break;
            default:
                std::cout << "Opcion invalida. Intente de nuevo." << std::endl;
        }
    } while (opcion != 3);

    return 0;
}