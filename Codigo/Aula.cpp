#include "Aula.h"

Aula::Aula() : nrc(""), materia(""), cedulaProfesor("") {}

Aula::Aula(std::string nrc, std::string materia, std::string cedulaProfesor)
    : nrc(nrc), materia(materia), cedulaProfesor(cedulaProfesor) {}

std::string Aula::getNrc() const { return nrc; }
void Aula::setNrc(std::string newNrc) { nrc = newNrc; }

std::string Aula::getMateria() const { return materia; }
void Aula::setMateria(std::string newMateria) { materia = newMateria; }

std::string Aula::getCedulaProfesor() const { return cedulaProfesor; }
void Aula::setCedulaProfesor(std::string newCedulaProfesor) { cedulaProfesor = newCedulaProfesor; }

void Aula::asignarProfesor(std::string cedula) {
    this->cedulaProfesor = cedula;
}

bool Aula::tieneCruce(Profesor p, DiaSemana dia, IntervaloHorario f) {
    return false; 
}