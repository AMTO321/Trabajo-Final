#include "Aula.h"

Aula::Aula() : nrc(""), idMateria(""), cedulaProfesor(""), dia(LUNES), horario() {}

Aula::Aula(std::string nrc, std::string idMateria, std::string cedulaProfesor,
           DiaSemana dia, IntervaloHorario horario)
    : nrc(nrc), idMateria(idMateria), cedulaProfesor(cedulaProfesor), dia(dia), horario(horario) {}

std::string Aula::getNrc() const { return nrc; }
void Aula::setNrc(std::string newNrc) { nrc = newNrc; }

std::string Aula::getIdMateria() const { return idMateria; }
void Aula::setIdMateria(std::string newIdMateria) { idMateria = newIdMateria; }

std::string Aula::getCedulaProfesor() const { return cedulaProfesor; }
void Aula::setCedulaProfesor(std::string newCedulaProfesor) { cedulaProfesor = newCedulaProfesor; }

DiaSemana Aula::getDia() const { return dia; }
void Aula::setDia(DiaSemana newDia) { dia = newDia; }

IntervaloHorario Aula::getHorario() const { return horario; }
void Aula::setHorario(IntervaloHorario newHorario) { horario = newHorario; }

void Aula::asignarProfesor(std::string cedula) {
    this->cedulaProfesor = cedula;
}

bool Aula::tieneCruce(const std::string& cedula, DiaSemana otroDia, const IntervaloHorario& f) const {
    return cedulaProfesor == cedula && dia == otroDia && horario.seCruzaCon(f);
}
