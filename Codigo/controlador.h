#ifndef CONTROLADOR_H
#define CONTROLADOR_H

#include <string>
#include "ListaDoble.h"
#include "Profesor.h"
#include "Materia.h"
#include "Aula.h"
#include "RegistroAsistencia.h"
#include "enumeracion.h"
#include "IntervaloHorario.h"

class Controlador {
private:
    ListaDoble<Profesor> profesores;
    ListaDoble<Materia> materias;
    ListaDoble<Aula> cursos;
    ListaDoble<RegistroAsistencia> registros;

    // Prefijo que se antepone al nombre de los archivos (vacio por defecto; las pruebas
    // lo usan para no tocar los archivos reales).
    std::string prefijoArchivos;
    std::string ruta(const char* nombreBase) const;

    // ---- Persistencia (archivos CSV) ----
    void cargarProfesores();
    void guardarProfesores() const;
    void cargarMaterias();
    void guardarMaterias() const;
    void cargarCursos();
    void guardarCursos() const;
    void cargarAsistencias();
    void guardarAsistenciaArchivo(const RegistroAsistencia& r) const;

    // ---- Busquedas ----
    Profesor* buscarProfesor(const std::string& cedula);
    Materia* buscarMateria(const std::string& id);
    Aula* buscarCurso(const std::string& nrc);
    const RegistroAsistencia* buscarRegistro(const std::string& fecha, const std::string& nrc,
                                             const std::string& tipo) const;

    // ---- Seleccion por numero desde una lista (0 = cancelar) ----
    Profesor* seleccionarProfesor();
    Materia* seleccionarMateria();
    Aula* seleccionarCurso();
    Profesor* identificarProfesor();
    Aula* seleccionarCursoDeProfesor(const Profesor& p);

    // ---- Utilidades ----
    bool pedirDiaYFranja(const std::string& cedula, const std::string& nrcExcluido,
                         DiaSemana& dia, IntervaloHorario& horario);
    std::string nombreMateria(const std::string& id);
    std::string nombreProfesor(const std::string& cedula);
    void mostrarEstadoReloj() const;
    void registrarMarcado(bool esIngreso);

public:
    explicit Controlador(const std::string& prefijoArchivos = "");

    const ListaDoble<Profesor>& getProfesores() const;
    const ListaDoble<Materia>& getMaterias() const;
    const ListaDoble<Aula>& getCursos() const;
    const ListaDoble<RegistroAsistencia>& getRegistros() const;

    // Menus
    void menuAdmin();
    void menuProfesor();
    void menuListados();

    // Administracion
    void registrarProfesor();
    void registrarMateria();
    void crearCursoConNrc();        // genera el NRC automaticamente
    void cambiarHorarioCurso();
    void eliminarCurso();
    void eliminarProfesor();
    void reporteAsistencias();

    // Listados
    void listarProfesores();
    void listarMaterias();
    void listarCursos();

    // Profesor
    void loginProfesor();     // Registro de ingreso
    void logoutProfesor();    // Registro de salida
    void verMisCursos();

    // Cambia dia/franja de un curso validando cruces. false si hay choque.
    bool agregarHorario(const std::string& nrc, DiaSemana dia, const IntervaloHorario& f);
};

#endif
