#ifndef GENERADOR_NRC_H
#define GENERADOR_NRC_H

#include <string>
#include "ListaDoble.h"
#include "Aula.h"

// Genera NRCs secuenciales de 5 digitos:
//   - Si no hay cursos: 10001
//   - Si ya hay:        NRC numericamente mas alto + 1   (ej. 10042 -> 10043)
class GeneradorNrc {
public:
    enum { NRC_INICIAL = 10001, NRC_MAXIMO = 99999 };

    // Devuelve "" si ya se agotaron los NRC (> 99999).
    static std::string siguiente(const ListaDoble<Aula>& cursos);
};

#endif
