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
    std::string prefijoArchivos;
    std::string ruta(const char* nombreBase) const;

  
    void cargarProfesores();
    void guardarProfesores() const;
    void cargarMaterias();
    void guardarMaterias() const;
    void cargarCursos();
    void guardarCursos() const;
    void cargarAsistencias();
    void guardarAsistenciaArchivo(const RegistroAsistencia& r) const;

    Profesor* buscarProfesor(const std::string& cedula);
    Materia* buscarMateria(const std::string& id);
    Aula* buscarCurso(const std::string& nrc);
    const RegistroAsistencia* buscarRegistro(const std::string& fecha, const std::string& nrc,
                                             const std::string& tipo) const;

    
    const Aula* buscarChoqueDeHorario(DiaSemana dia, const IntervaloHorario& h,
                                      const std::string& nrcExcluido) const;
    
    const RegistroAsistencia* buscarIngresoDeOtroProfesor(const std::string& fecha, const std::string& cedula,
                                                          const IntervaloHorario& h) const;

    
    Profesor* seleccionarProfesor();
    
    bool seleccionarMateriaDeCatalogo(Materia& materia);
    Aula* seleccionarCurso();
    Profesor* identificarProfesor();
    Aula* seleccionarCursoDeProfesor(const Profesor& p);

    
    bool pedirDiaYFranja(const std::string& cedula, const std::string& nrcExcluido,
                         DiaSemana& dia, IntervaloHorario& horario);
    std::string nombreMateria(const std::string& id);
    std::string nombreProfesor(const std::string& cedula);
    void mostrarEstadoReloj() const;
    void registrarMarcado(bool esIngreso);
    void pruebaDeHora(bool esIngreso);   // comun a pruebaLogin / pruebaLogout

public:
    explicit Controlador(const std::string& prefijoArchivos = "");

    const ListaDoble<Profesor>& getProfesores() const;
    const ListaDoble<Materia>& getMaterias() const;
    const ListaDoble<Aula>& getCursos() const;
    const ListaDoble<RegistroAsistencia>& getRegistros() const;


    void menuAdmin();
    void menuProfesor();
    void menuListados();
    void menuHorasDePrueba();   
    void registrarProfesor();
    void crearCursoConNrc();       
    void eliminarCurso();
    void eliminarProfesor();


    void listarProfesores();
    void listarMaterias();
    void listarCursos();

    void loginProfesor();     
    void logoutProfesor();    
    void verMisCursos();
    void pruebaLogin();       
    void pruebaLogout();     
};

#endif
