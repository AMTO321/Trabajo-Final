#ifndef REGISTRO_ASISTENCIA_H
#define REGISTRO_ASISTENCIA_H

#include <string>


class RegistroAsistencia {
private:
    std::string fecha;   
    std::string hora;    
    std::string cedula;
    std::string nrc;
    std::string tipo;    
    std::string estado;

public:
    RegistroAsistencia();
    RegistroAsistencia(std::string fecha, std::string hora, std::string cedula,
                       std::string nrc, std::string tipo, std::string estado);

    std::string getFecha() const;
    std::string getHora() const;
    std::string getCedula() const;
    std::string getNrc() const;
    std::string getTipo() const;
    std::string getEstado() const;

    std::string toCsv() const;      
    std::string toString() const;   
};

#endif
