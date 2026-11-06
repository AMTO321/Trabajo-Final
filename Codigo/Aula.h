#ifndef AULA_H
#define AULA_H

#include <string>
#include "enumeracion.h"
#include "IntervaloHorario.h"

class Aula {
private:
    std::string nrc;
    std::string idMateria;
    std::string cedulaProfesor;
    DiaSemana dia;
    IntervaloHorario horario;

public:
    Aula();
    Aula(std::string nrc, std::string idMateria, std::string cedulaProfesor,
         DiaSemana dia, IntervaloHorario horario);

    std::string getNrc() const;
    void setNrc(std::string newNrc);

    std::string getIdMateria() const;
    void setIdMateria(std::string newIdMateria);

    std::string getCedulaProfesor() const;
    void setCedulaProfesor(std::string newCedulaProfesor);

    DiaSemana getDia() const;
    void setDia(DiaSemana newDia);

    IntervaloHorario getHorario() const;
    void setHorario(IntervaloHorario newHorario);

    void asignarProfesor(std::string cedula);

    // true si este curso choca con el dia/franja dado para ese profesor
    bool tieneCruce(const std::string& cedula, DiaSemana otroDia, const IntervaloHorario& f) const;

    // true si este curso ocupa ese dia/franja, sea cual sea el profesor
    // (dos profesores no pueden coincidir en el mismo dia y hora)
    bool ocupaHorario(DiaSemana otroDia, const IntervaloHorario& f) const;
};

#endif
