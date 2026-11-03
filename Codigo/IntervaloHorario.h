#ifndef INTERVALOHORARIO_H
#define INTERVALOHORARIO_H

#include <string>

// Franja horaria en minutos desde las 00:00. Rango [inicio, fin).
// Las clases se dictan en 7 franjas fijas de 2 horas entre 07:00 y 21:00.
class IntervaloHorario {
private:
    int horaInicioMinutos;
    int horaFinMinutos;

public:
    IntervaloHorario();
    IntervaloHorario(int inicioMin, int finMin);

    int getHoraInicioMinutos() const;
    void setHoraInicioMinutos(int val);

    int getHoraFinMinutos() const;
    void setHoraFinMinutos(int val);

    bool estaEnFranja(int horaActualMinutos) const;      // inicio <= t < fin
    bool esIngresoValido(int horaActualMinutos) const;   // puntual: primeros 10 min
    bool esIngresoPermitido(int horaActualMinutos) const;// desde inicio hasta antes de la ventana de salida
    bool esSalidaValido(int horaActualMinutos) const;    // ultimos 10 min de la clase
    bool seCruzaCon(const IntervaloHorario& otro) const;
    bool esFranjaOficial() const;

    std::string toString() const;                        // "07:00 - 09:00"

    static int totalFranjas();
    static IntervaloHorario franja(int numero);          // numero 1..totalFranjas()
    static std::string textoHora(int minutos);           // 425 -> "07:05"
};
#endif
