#ifndef REGISTROASISTENCIA_H
#define REGISTROASISTENCIA_H

#include <string>
#include "enumeracion.h"

class RegistroAsistencia {
private:
    std::string fecha;
    std::string horaIngreso;
    std::string horaSalida;
    EstadoAsistencia estado;

public:
    RegistroAsistencia();
    
    std::string getFecha() const;
    void setFecha(std::string newFecha);

    std::string getHoraIngreso() const;
    void setHoraIngreso(std::string newHoraIngreso);

    std::string getHoraSalida() const;
    void setHoraSalida(std::string newHoraSalida);

    EstadoAsistencia getEstado() const;
    void setEstado(EstadoAsistencia newState);

    void registrarIngreso();
    void registrarSalida();
};

#endif