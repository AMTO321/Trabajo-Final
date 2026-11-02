#ifndef VALIDACION_H
#define VALIDACION_H

#include <string>

class Validacion {
public:
    static bool soloNumeros(std::string texto);
    static bool soloLetras(std::string texto);
    static bool validarCedula(std::string cedula);
};

#endif