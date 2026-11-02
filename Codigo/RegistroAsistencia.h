#ifndef REGISTRO_ASISTENCIA_H
#define REGISTRO_ASISTENCIA_H

#include <string>

class RegistroAsistencia {
private:
    std::string cedula;
    std::string nrc;
    std::string estado;
    std::string hora;

public:
    RegistroAsistencia();
    RegistroAsistencia(std::string cedula, std::string nrc, std::string estado, std::string hora);

    void registrarIngreso(std::string cedulaProfesor, std::string nrcCurso);
    void registrarSalida();
    
    std::string toString() const;
};

#endif