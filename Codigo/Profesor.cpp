#include "Profesor.h"

Profesor::Profesor() : cedula(""), nombres(""), apellidos("") {}

Profesor::Profesor(std::string cedula, std::string nombres, std::string apellidos)
    : cedula(cedula), nombres(nombres), apellidos(apellidos) {}

std::string Profesor::getCedula() const { return cedula; }
void Profesor::setCedula(std::string newCedula) { cedula = newCedula; }

std::string Profesor::getNombres() const { return nombres; }
void Profesor::setNombres(std::string newNombres) { nombres = newNombres; }

std::string Profesor::getApellidos() const { return apellidos; }
void Profesor::setApellidos(std::string newApellidos) { apellidos = newApellidos; }