#ifndef AULA_H
#define AULA_H

#include <string>
#include "enumeracion.h"
#include "IntervaloHorario.h"
#include "Profesor.h"

class Aula {
private:
    std::string nrc;
    std::string materia;
    std::string cedulaProfesor;

public:
    Aula();
    Aula(std::string nrc, std::string materia, std::string cedulaProfesor);

    std::string getNrc() const;
    void setNrc(std::string newNrc);

    std::string getMateria() const;
    void setMateria(std::string newMateria);

    std::string getCedulaProfesor() const;
    void setCedulaProfesor(std::string newCedulaProfesor);

    void asignarProfesor(std::string cedula);
    bool tieneCruce(Profesor p, DiaSemana dia, IntervaloHorario f);
};

#endif