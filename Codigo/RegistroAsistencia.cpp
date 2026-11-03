#include "RegistroAsistencia.h"

RegistroAsistencia::RegistroAsistencia()
    : fecha(""), hora(""), cedula(""), nrc(""), tipo(""), estado("") {}

RegistroAsistencia::RegistroAsistencia(std::string fecha, std::string hora, std::string cedula,
                                       std::string nrc, std::string tipo, std::string estado)
    : fecha(fecha), hora(hora), cedula(cedula), nrc(nrc), tipo(tipo), estado(estado) {}

std::string RegistroAsistencia::getFecha() const { return fecha; }
std::string RegistroAsistencia::getHora() const { return hora; }
std::string RegistroAsistencia::getCedula() const { return cedula; }
std::string RegistroAsistencia::getNrc() const { return nrc; }
std::string RegistroAsistencia::getTipo() const { return tipo; }
std::string RegistroAsistencia::getEstado() const { return estado; }

std::string RegistroAsistencia::toCsv() const {
    return fecha + "," + hora + "," + cedula + "," + nrc + "," + tipo + "," + estado;
}

std::string RegistroAsistencia::toString() const {
    return "Fecha: " + fecha + " | Hora: " + hora + " | Cedula: " + cedula +
           " | NRC: " + nrc + " | " + tipo + " | Estado: " + estado;
}
