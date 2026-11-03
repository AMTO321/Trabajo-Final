#include "Materia.h"

Materia::Materia() : id(""), nombre(""), creditos(0) {}

Materia::Materia(std::string id, std::string nombre, int creditos)
    : id(id), nombre(nombre), creditos(creditos) {}

std::string Materia::getId() const { return id; }
void Materia::setId(std::string newId) { id = newId; }

std::string Materia::getNombre() const { return nombre; }
void Materia::setNombre(std::string newNombre) { nombre = newNombre; }

int Materia::getCreditos() const { return creditos; }
void Materia::setCreditos(int newCreditos) { creditos = newCreditos; }
