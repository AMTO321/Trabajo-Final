#include "Reloj.h"
#include <ctime>
#include <cstdio>

bool Reloj::simulando = false;
Instante Reloj::simulado = Instante();

int Reloj::diaDeSemanaDeFecha(const std::string& fecha) {
    int a = 0, m = 0, d = 0;
    std::sscanf(fecha.c_str(), "%d-%d-%d", &a, &m, &d);
    std::tm t = std::tm();
    t.tm_year = a - 1900;
    t.tm_mon = m - 1;
    t.tm_mday = d;
    t.tm_hour = 12;
    std::mktime(&t);
    return t.tm_wday;
}

Instante Reloj::ahora() {
    if (simulando) return simulado;

    std::time_t actual = std::time(nullptr);
    std::tm* t = std::localtime(&actual);
    char buffer[40];
    std::snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);

    Instante i;
    i.fecha = buffer;
    i.minutos = t->tm_hour * 60 + t->tm_min;
    i.diaSemana = t->tm_wday;
    return i;
}

void Reloj::simular(const std::string& fecha, int minutos) {
    simulando = true;
    simulado.fecha = fecha;
    simulado.minutos = minutos;
    simulado.diaSemana = diaDeSemanaDeFecha(fecha);
}

void Reloj::detenerSimulacion() { simulando = false; }
bool Reloj::estaSimulando() { return simulando; }

bool Reloj::aDiaSemana(int diaSemanaC, DiaSemana& resultado) {
    if (diaSemanaC < 1 || diaSemanaC > 5) return false;
    resultado = static_cast<DiaSemana>(diaSemanaC - 1);
    return true;
}
