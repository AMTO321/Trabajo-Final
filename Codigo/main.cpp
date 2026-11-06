#include <iostream>
#include <stdexcept>
#include "controlador.h"
#include "Entrada.h"

int main() {
    try {
        Controlador sistema;
        int opcion = 0;

        do {
            std::cout << "\n Programa de asistencias y horarios" << std::endl;
            std::cout << "1. Menu Administrador" << std::endl;
            std::cout << "2. Menu Profesor" << std::endl;
            std::cout << "3. Salir" << std::endl;
            opcion = Entrada::leerEntero("Seleccione una opcion: ", 1, 3);

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
    } catch (const std::runtime_error&) {
        
        std::cout << "\nEntrada finalizada. Cerrando el sistema.\n";
    }
    return 0;
}
