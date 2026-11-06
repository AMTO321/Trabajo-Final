#ifndef RELOJ_H
#define RELOJ_H

#include <string>
#include "enumeracion.h"

struct Instante {
    std::string fecha;  
    int minutos;        
    int diaSemana;      
};


class Reloj {
private:
    static bool simulando;
    static Instante simulado;

public:
    static Instante ahora();
    static void simular(const std::string& fecha, int minutos);
    static void detenerSimulacion();
    static bool estaSimulando();

    static int diaDeSemanaDeFecha(const std::string& fecha);   // 0 = domingo
    static bool aDiaSemana(int diaSemanaC, DiaSemana& resultado); // false si es sabado/domingo
};

#endif
