#include "IntervaloHorario.h"

IntervaloHorario::IntervaloHorario() : horaInicioMinutos(0), horaFinMinutos(0) {}

IntervaloHorario::IntervaloHorario(int inicioMin, int finMin) 
    : horaInicioMinutos(inicioMin), horaFinMinutos(finMin) {}

int IntervaloHorario::getHoraInicioMinutos() const { return horaInicioMinutos; }
void IntervaloHorario::setHoraInicioMinutos(int val) { horaInicioMinutos = val; }

int IntervaloHorario::getHoraFinMinutos() const { return horaFinMinutos; }
void IntervaloHorario::setHoraFinMinutos(int val) { horaFinMinutos = val; }

bool IntervaloHorario::estaEnFranja(int horaActualMinutos) const {
    return (horaActualMinutos >= horaInicioMinutos && horaActualMinutos <= horaFinMinutos);
}

bool IntervaloHorario::esIngresoValido(int horaActualMinutos) const {
    // Holgura de 10 minutos para registrar ingreso
    return (horaActualMinutos >= horaInicioMinutos && horaActualMinutos <= horaInicioMinutos + 10);
}