#ifndef INTERVALOHORARIO_H
#define INTERVALOHORARIO_H

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
    bool esIngresoValido(int horaActualMinutos) const; // Con los 10 min de holgura
};

#endif