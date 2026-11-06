#ifndef GENERADOR_NRC_H
#define GENERADOR_NRC_H

#include <string>
#include "ListaDoble.h"
#include "Aula.h"

 
class GeneradorNrc {
public:
    enum { NRC_INICIAL = 1, NRC_MAXIMO = 999999999 };

    
    static std::string siguiente(const ListaDoble<Aula>& cursos);
};

#endif
