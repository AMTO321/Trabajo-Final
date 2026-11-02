#include "validacion.h"
#include <cctype>
#include <string>

bool Validacion::soloNumeros(std::string texto) {
    if (texto.empty()) return false;
    for (char c : texto) {
        if (!isdigit(c)) return false;
    }
    return true;
}

bool Validacion::soloLetras(std::string texto) {
    if (texto.empty()) return false;
    for (char c : texto) {
        if (!isalpha(c) && c != ' ') return false;
    }
    return true;
}

bool Validacion::validarCedula(std::string cedula) {
    if (cedula.length() != 10 || !soloNumeros(cedula)) {
        return false;
    }

    int provincia = std::stoi(cedula.substr(0, 2));
    bool provinciaValida = (provincia >= 1 && provincia <= 24) || provincia == 30;
    if (!provinciaValida) {
        return false;
    }

    int tercerDigito = cedula[2] - '0';
    if (tercerDigito >= 6) {
        return false;
    }

    int coeficientes[9] = {2, 1, 2, 1, 2, 1, 2, 1, 2};
    int digitoVerificador = cedula[9] - '0';
    int suma = 0;

    for (int i = 0; i < 9; i++) {
        int valor = (cedula[i] - '0') * coeficientes[i];
        if (valor >= 10) {
            valor -= 9;
        }
        suma += valor;
    }

    int residuo = suma % 10;
    int resultadoVerificador = (residuo == 0) ? 0 : (10 - residuo);

    return resultadoVerificador == digitoVerificador;
}