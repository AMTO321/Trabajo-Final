#include "Entrada.h"
#include "validacion.h"
#include <iostream>
#include <stdexcept>

std::string Entrada::leerLinea(const std::string& mensaje) {
    std::cout << mensaje;
    std::string linea;
    if (!std::getline(std::cin, linea)) {
        throw std::runtime_error("entrada cerrada");
    }
    return Validacion::trim(linea);
}

int Entrada::leerEntero(const std::string& mensaje, int minimo, int maximo) {
    while (true) {
        std::string texto = leerLinea(mensaje);
        if (!Validacion::soloNumeros(texto) || texto.size() > 9) {
            std::cout << " ingrese un numero entero entre " << minimo << " y " << maximo << ".\n";
            continue;
        }
        int valor = std::stoi(texto);
        if (valor < minimo || valor > maximo) {
            std::cout << "el valor debe estar entre " << minimo << " y " << maximo << ".\n";
            continue;
        }
        return valor;
    }
}

bool Entrada::confirmar(const std::string& mensaje) {
    while (true) {
        std::string r = Validacion::aMayusculas(leerLinea(mensaje + " (s/n): "));
        if (r == "S" || r == "SI") return true;
        if (r == "N" || r == "NO") return false;
        std::cout << " responda 's' o 'n'.\n";
    }
}
