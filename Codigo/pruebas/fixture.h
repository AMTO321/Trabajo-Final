#ifndef FIXTURE_H
#define FIXTURE_H

#include "soporte.h"
#include "../controlador.h"

// Datos base: 2 profesores, 3 materias y 3 cursos.
//   Profesor 1 (0605293885, Toa Orejuela): 10001 ALGE101 LUN 07-09 | 10002 EDIF201 LUN 09-11
//   Profesor 2 (1710034065, Almeida)      : 10003 CALC101 MAR 07-09
inline void datosBasicos() {
    ArchivosTemporales::escribir("profesores.txt",
        "0605293885,Adrian Michael,Toa Orejuela\n"
        "1710034065,Ana,Almeida\n");
    ArchivosTemporales::escribir("materias.txt",
        "ALGE101,Algebra Lineal,4\n"
        "CALC101,Calculo 1,4\n"
        "EDIF201,Ecuaciones Diferenciales,4\n");
    ArchivosTemporales::escribir("cursos.txt",
        "10001,ALGE101,0605293885,LUNES,420,540\n"
        "10002,EDIF201,0605293885,LUNES,540,660\n"
        "10003,CALC101,1710034065,MARTES,420,540\n");
}

// Solo profesores y materias (sin cursos), util para probar la creacion de NRC desde cero.
inline void datosSinCursos() {
    datosBasicos();
    ArchivosTemporales::escribir("cursos.txt", "");
}

inline Controlador crear() { return Controlador(ArchivosTemporales::prefijo()); }

inline const Aula* buscarCurso(const Controlador& c, const std::string& nrc) {
    return c.getCursos().buscarPtr([&nrc](const Aula& a) { return a.getNrc() == nrc; });
}
inline const Profesor* buscarProfesor(const Controlador& c, const std::string& cedula) {
    return c.getProfesores().buscarPtr([&cedula](const Profesor& p) { return p.getCedula() == cedula; });
}
inline const Materia* buscarMateria(const Controlador& c, const std::string& id) {
    return c.getMaterias().buscarPtr([&id](const Materia& m) { return m.getId() == id; });
}

#endif
