#ifndef CONTROLADOR_H
#define CONTROLADOR_H

#include "ListaDoble.h"
#include "Profesor.h"
#include "Aula.h"
#include "RegistroAsistencia.h"
#include "enumeracion.h"
#include "IntervaloHorario.h"

class Controlador {
private:
    ListaDoble<Profesor> profesores;
    ListaDoble<Aula> cursos; 
    ListaDoble<RegistroAsistencia> registros;

    void cargarProfesores();
    void guardarProfesorArchivo(const Profesor& p);
    void cargarCursos();
    void guardarCursoArchivo(const Aula& a);
    void guardarAsistenciaArchivo(const std::string& cedula, const std::string& nrc, const std::string& estado, const std::string& hora);

public:
    Controlador();

    // Declaraciones exactas para que coincidan con el controlador.cpp
    ListaDoble<Profesor> getProfesores() const;
    void setProfesores(ListaDoble<Profesor> newProfesores);

    ListaDoble<Aula> getCursos() const;
    void setCursos(ListaDoble<Aula> newCursos);

    ListaDoble<RegistroAsistencia> getRegistros() const;
    void setRegistros(ListaDoble<RegistroAsistencia> newRegistros);

    void menuAdmin();
    void menuProfesor();
    void registrarProfesor();
    void asignarCursoANrc();
    void loginProfesor();
    void agregarHorario(std::string nrc, DiaSemana dia, IntervaloHorario f);
};

#endif