#ifndef INTERVALOHORARIO_H
#define INTERVALOHORARIO_H

#include <string>

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

    bool estaEnFranja(int horaActualMinutos) const;      
    bool esIngresoValido(int horaActualMinutos) const;   
    bool esIngresoPermitido(int horaActualMinutos) const;
    bool esSalidaValido(int horaActualMinutos) const;    
    bool seCruzaCon(const IntervaloHorario& otro) const;
    bool esFranjaOficial() const;

    std::string toString() const;                       

    static int totalFranjas();
    static IntervaloHorario franja(int numero);          
    static std::string textoHora(int minutos);           
};
#endif
