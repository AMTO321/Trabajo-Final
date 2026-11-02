#include "RegistroAsistencia.h"

RegistroAsistencia::RegistroAsistencia() 
    : fecha(""), horaIngreso(""), horaSalida(""), estado(PUNTUAL) {}

std::string RegistroAsistencia::getFecha() const { return fecha; }
void RegistroAsistencia::setFecha(std::string newFecha) { fecha = newFecha; }

std::string RegistroAsistencia::getHoraIngreso() const { return horaIngreso; }
void RegistroAsistencia::setHoraIngreso(std::string newHoraIngreso) { horaIngreso = newHoraIngreso; }

std::string RegistroAsistencia::getHoraSalida() const { return horaSalida; }
void RegistroAsistencia::setHoraSalida(std::string newHoraSalida) { horaSalida = newHoraSalida; }

EstadoAsistencia RegistroAsistencia::getEstado() const { return estado; }
void RegistroAsistencia::setEstado(EstadoAsistencia newState) { estado = newState; }

void RegistroAsistencia::registrarIngreso() {
    // Lógica para registrar ingreso
}

void RegistroAsistencia::registrarSalida() {
    // Lógica para registrar salida
}