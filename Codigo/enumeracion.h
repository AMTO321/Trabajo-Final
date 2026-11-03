#ifndef ENUMS_H
#define ENUMS_H

#include <string>

enum EstadoAsistencia {
    PUNTUAL,
    ATRASO,
    AUSENTE
};

enum DiaSemana {
    LUNES,
    MARTES,
    MIERCOLES,
    JUEVES,
    VIERNES
};

inline std::string nombreDia(DiaSemana d) {
    switch (d) {
        case LUNES:     return "LUNES";
        case MARTES:    return "MARTES";
        case MIERCOLES: return "MIERCOLES";
        case JUEVES:    return "JUEVES";
        case VIERNES:   return "VIERNES";
    }
    return "?";
}

inline bool parseDia(const std::string& texto, DiaSemana& resultado) {
    for (int i = LUNES; i <= VIERNES; i++) {
        if (nombreDia(static_cast<DiaSemana>(i)) == texto) {
            resultado = static_cast<DiaSemana>(i);
            return true;
        }
    }
    return false;
}

inline std::string nombreEstado(EstadoAsistencia e) {
    switch (e) {
        case PUNTUAL: return "PUNTUAL";
        case ATRASO:  return "ATRASO";
        case AUSENTE: return "AUSENTE";
    }
    return "?";
}

#endif
