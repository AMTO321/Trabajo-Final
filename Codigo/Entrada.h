#ifndef ENTRADA_H
#define ENTRADA_H

#include <string>


class Entrada {
public:
    
    static std::string leerLinea(const std::string& mensaje);

    static int leerEntero(const std::string& mensaje, int minimo, int maximo);
    
    static bool confirmar(const std::string& mensaje);
};

#endif
