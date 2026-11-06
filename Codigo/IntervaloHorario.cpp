#include "IntervaloHorario.h"

namespace {
const int PRIMERA_FRANJA_INICIO = 420; 
const int DURACION_FRANJA = 120;       
const int NUM_FRANJAS = 7;            
}

IntervaloHorario::IntervaloHorario() : horaInicioMinutos(0), horaFinMinutos(0) {}

IntervaloHorario::IntervaloHorario(int inicioMin, int finMin)
    : horaInicioMinutos(inicioMin), horaFinMinutos(finMin) {}

int IntervaloHorario::getHoraInicioMinutos() const { return horaInicioMinutos; }
void IntervaloHorario::setHoraInicioMinutos(int val) { horaInicioMinutos = val; }

int IntervaloHorario::getHoraFinMinutos() const { return horaFinMinutos; }
void IntervaloHorario::setHoraFinMinutos(int val) { horaFinMinutos = val; }

bool IntervaloHorario::estaEnFranja(int t) const {
    return t >= horaInicioMinutos && t < horaFinMinutos;
}

bool IntervaloHorario::esIngresoValido(int t) const {
    return t >= horaInicioMinutos && t <= horaInicioMinutos + 10;
}

bool IntervaloHorario::esIngresoPermitido(int t) const {
    return t >= horaInicioMinutos && t < horaFinMinutos - 10;
}

bool IntervaloHorario::esSalidaValido(int t) const {
    return t >= horaFinMinutos - 10 && t <= horaFinMinutos - 1;
}

bool IntervaloHorario::seCruzaCon(const IntervaloHorario& otro) const {
    return horaInicioMinutos < otro.horaFinMinutos && otro.horaInicioMinutos < horaFinMinutos;
}

bool IntervaloHorario::esFranjaOficial() const {
    for (int i = 1; i <= NUM_FRANJAS; i++) {
        IntervaloHorario f = franja(i);
        if (f.horaInicioMinutos == horaInicioMinutos && f.horaFinMinutos == horaFinMinutos) return true;
    }
    return false;
}

std::string IntervaloHorario::toString() const {
    return textoHora(horaInicioMinutos) + " - " + textoHora(horaFinMinutos);
}

int IntervaloHorario::totalFranjas() { return NUM_FRANJAS; }

IntervaloHorario IntervaloHorario::franja(int numero) {
    int inicio = PRIMERA_FRANJA_INICIO + (numero - 1) * DURACION_FRANJA;
    return IntervaloHorario(inicio, inicio + DURACION_FRANJA);
}

std::string IntervaloHorario::textoHora(int minutos) {
    int h = minutos / 60;
    int m = minutos % 60;
    return std::string(h < 10 ? "0" : "") + std::to_string(h) + ":" +
           (m < 10 ? "0" : "") + std::to_string(m);
}
