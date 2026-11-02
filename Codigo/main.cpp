#include <iostream>
#include <limits> 
#include "controlador.h"

int main() {
    Controlador sistema;
    int opcion = 0;

    do {
        std::cout << "\n sistema de asistencias y horarios" << std::endl;
        std::cout << "1. Menu Administrador" << std::endl;
        std::cout << "2. Menu Profesor" << std::endl;
        std::cout << "3. Salir" << std::endl;
        std::cout << "Seleccione una opcion: ";
        
        if (!(std::cin >> opcion)) {
            std::cout << "\nOpcion invalida Ingrese un numero valido (1, 2 o 3).\n";
            std::cin.clear(); 
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
            opcion = 0; 
        }

        switch (opcion) {
            case 1:
                sistema.menuAdmin();
                break;
            case 2:
                sistema.menuProfesor();
                break;
            case 3:
                std::cout << "Saliendo del programa\n" << std::endl;
                break;
            
        }
    } while (opcion != 3);

    return 0;
}