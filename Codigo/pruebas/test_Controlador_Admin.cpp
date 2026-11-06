#include "mini_test.h"
#include "fixture.h"

// Una prueba por funcion publica del Controlador (administracion y listados).
// Cada caso reinicia los archivos temporales con ArchivosTemporales::borrar().
// Datos base (datosBasicos): profesor 1 = 0605293885 (Toa Orejuela) con 10001 Lun 07-09 y 10002 Lun 09-11;
//                            profesor 2 = 1710034065 (Almeida) con 10003 Mar 07-09.

// =====================================================================
//  registrarProfesor
// =====================================================================
TEST(Controlador_registrarProfesor) {
    ArchivosTemporales tmp;
    {   // ok: normaliza espacios y guarda en el archivo
        Controlador c = crear();
        Consola k("1710034065\n  Ana   Maria \nPerez    Lopez\n");
        c.registrarProfesor();
        CHECK_EQ(c.getProfesores().getTamanio(), 1u);
        const Profesor* p = buscarProfesor(c, "1710034065");
        CHECK(p != nullptr);
        if (p) {
            CHECK_EQ(p->getNombres(), "Ana Maria");
            CHECK_EQ(p->getApellidos(), "Perez Lopez");
        }
        CHECK_EQ(ArchivosTemporales::leer("profesores.txt"), "1710034065,Ana Maria,Perez Lopez\n");
        CHECK_CONTAINS(k.texto(), "con exito");
    }
    {   // cedula invalida: repite la pregunta
        ArchivosTemporales::borrar();
        Controlador c = crear();
        Consola k("123\n0605293886\nabc\n1710034065\nAna\nPerez\n");
        c.registrarProfesor();
        CHECK_EQ(c.getProfesores().getTamanio(), 1u);
        CHECK_CONTAINS(k.texto(), "La cedula ingresada no es valida");
    }
    {   // cedula duplicada
        ArchivosTemporales::borrar();
        datosBasicos();
        Controlador c = crear();
        Consola k("0605293885\n0\n");
        c.registrarProfesor();
        CHECK_EQ(c.getProfesores().getTamanio(), 2u);
        CHECK_CONTAINS(k.texto(), "Ya existe un profesor");
    }
    {   // nombre o apellido invalido: repite
        ArchivosTemporales::borrar();
        Controlador c = crear();
        Consola k("1710034065\nAna1\nA\nAna\nPe@rez\nPerez\n");
        c.registrarProfesor();
        CHECK_EQ(c.getProfesores().getTamanio(), 1u);
        CHECK_CONTAINS(k.texto(), "el nombre solo debe contener letras");
        CHECK_CONTAINS(k.texto(), "el apellido solo debe contener letras");
    }
    {   // cancelar con 0 en cada paso: no guarda nada
        ArchivosTemporales::borrar();
        Controlador c = crear();
        { Consola k("0\n"); c.registrarProfesor(); }
        { Consola k("1710034065\n0\n"); c.registrarProfesor(); }
        { Consola k("1710034065\nAna\n0\n"); c.registrarProfesor(); }
        CHECK(c.getProfesores().estaVacia());
        CHECK_FALSE(ArchivosTemporales::existe("profesores.txt"));
    }
    {   // persiste entre ejecuciones
        ArchivosTemporales::borrar();
        { Controlador c = crear(); Consola k("1710034065\nAna\nPerez\n"); c.registrarProfesor(); }
        Controlador otra = crear();
        CHECK(buscarProfesor(otra, "1710034065") != nullptr);
    }
}

// =====================================================================
//  crearCursoConNrc   (teclas: materia, profesor, NRC, dia, franja, confirmar)
// =====================================================================
TEST(Controlador_crearCursoConNrc) {
    ArchivosTemporales tmp;
    {   // sin profesores no se puede crear
        Controlador c = crear();
        Consola k;
        c.crearCursoConNrc();
        CHECK_CONTAINS(k.texto(), "necesita tener al menos un profesor");
        CHECK(c.getCursos().estaVacia());
    }
    {   // ofrece solo las 8 materias fijas, sin creditos
        datosSinCursos();
        Controlador c = crear();
        Consola k("0\n");
        c.crearCursoConNrc();
        std::string s = k.texto();
        CHECK_CONTAINS(s, "1. Calculo\n");
        CHECK_CONTAINS(s, "2. Ecuaciones Diferenciales");
        CHECK_CONTAINS(s, "3. Fisica");
        CHECK_CONTAINS(s, "4. Algebra Lineal");
        CHECK_CONTAINS(s, "5. Calculo Vectorial");
        CHECK_CONTAINS(s, "6. Estadistica");
        CHECK_CONTAINS(s, "7. Metodos Numericos");
        CHECK_CONTAINS(s, "8. Quimica");
        CHECK_NOT_CONTAINS(s, "9. ");
        CHECK_NOT_CONTAINS(s, "reditos");
    }
    {   // el NRC se ingresa por teclado y se guarda
        ArchivosTemporales::borrar();
        datosSinCursos();
        Controlador c = crear();
        Consola k("4\n1\n20555\n1\n1\ns\n");   // Algebra Lineal, prof 1, NRC 20555, Lunes, franja 1
        c.crearCursoConNrc();
        CHECK_EQ(c.getCursos().getTamanio(), 1u);
        const Aula* a = buscarCurso(c, "20555");
        CHECK(a != nullptr);
        if (a) {
            CHECK_EQ(a->getIdMateria(), "ALGE101");
            CHECK_EQ(a->getCedulaProfesor(), "0605293885");
            CHECK_EQ(a->getDia(), LUNES);
            CHECK_EQ(a->getHorario().toString(), "07:00 - 09:00");
        }
        CHECK_EQ(ArchivosTemporales::leer("cursos.txt"), "20555,ALGE101,0605293885,LUNES,420,540\n");
        std::string s = k.texto();
        CHECK_CONTAINS(s, "NRC del curso");
        CHECK_CONTAINS(s, "NRC          : 20555");
        CHECK_CONTAINS(s, "Curso creado correctamente con NRC 20555");
        CHECK_NOT_CONTAINS(s, "eriodo");
        CHECK_NOT_CONTAINS(s, "aralelo");
    }
    {   // NRC no numerico, con cero inicial, de 10 digitos, decimal o duplicado: repite la pregunta
        ArchivosTemporales::borrar();
        datosBasicos();                          // ya existen 10001..10003
        Controlador c = crear();
        Consola k("1\n2\nabc\n0123\n1234567890\n12.5\n10001\n10004\n3\n3\ns\n");
        c.crearCursoConNrc();
        CHECK_CONTAINS(k.texto(), "el NRC debe contener solo numeros");
        CHECK_CONTAINS(k.texto(), "ya existe un curso con el NRC 10001");
        CHECK_EQ(c.getCursos().getTamanio(), 4u);
        const Aula* a = buscarCurso(c, "10004");
        CHECK(a != nullptr);
        if (a) {
            CHECK_EQ(a->getDia(), MIERCOLES);
            CHECK_EQ(a->getHorario().toString(), "11:00 - 13:00");
            CHECK_EQ(a->getCedulaProfesor(), "1710034065");
        }
    }
    {   // un NRC corto (sin minimo) es valido
        ArchivosTemporales::borrar();
        datosSinCursos();
        Controlador c = crear();
        Consola k("1\n1\n7\n1\n1\ns\n");
        c.crearCursoConNrc();
        CHECK(buscarCurso(c, "7") != nullptr);
        CHECK_EQ(ArchivosTemporales::leer("cursos.txt"), "7,CALC101,0605293885,LUNES,420,540\n");
    }
    {   // materia del catalogo que aun no existe: se registra junto con el curso
        ArchivosTemporales::borrar();
        ArchivosTemporales::escribir("profesores.txt", "0605293885,Adrian Michael,Toa Orejuela\n");
        Controlador c = crear();                 // sin materias registradas
        Consola k("8\n1\n10001\n1\n1\ns\n");     // Quimica, prof 1, NRC 10001, Lunes, franja 1
        c.crearCursoConNrc();
        CHECK_EQ(c.getCursos().getTamanio(), 1u);
        const Materia* m = buscarMateria(c, "QUIM101");
        CHECK(m != nullptr);
        if (m) CHECK_EQ(m->getNombre(), "Quimica");
        CHECK_CONTAINS(k.texto(), "se registrara con el curso");
        CHECK_CONTAINS(k.texto(), "Materia Quimica registrada con exito.");
        CHECK_CONTAINS(ArchivosTemporales::leer("materias.txt"), "QUIM101,Quimica,");
        CHECK_EQ(ArchivosTemporales::leer("cursos.txt"), "10001,QUIM101,0605293885,LUNES,420,540\n");
    }
    {   // materia que ya existe: no se duplica
        ArchivosTemporales::borrar();
        datosSinCursos();                        // ya tiene ALGE101
        Controlador c = crear();
        Consola k("4\n1\n10001\n1\n1\ns\n");
        c.crearCursoConNrc();
        CHECK_EQ(c.getMaterias().getTamanio(), 3u);
        CHECK_NOT_CONTAINS(k.texto(), "se registrara con el curso");
    }
    {   // la misma materia puede tener varios NRC (en horarios distintos)
        ArchivosTemporales::borrar();
        datosSinCursos();
        Controlador c = crear();
        { Consola k("4\n1\n30001\n1\n1\ns\n"); c.crearCursoConNrc(); }   // Lunes 07-09
        { Consola k("4\n2\n30002\n1\n2\ns\n"); c.crearCursoConNrc(); }   // Lunes 09-11 (contigua, sin choque)
        CHECK_EQ(c.getCursos().getTamanio(), 2u);
        CHECK_EQ(c.getCursos().contar([](const Aula& a) { return a.getIdMateria() == "ALGE101"; }), 2u);
    }
    {   // choque con el mismo profesor: pide otro horario
        ArchivosTemporales::borrar();
        datosBasicos();
        Controlador c = crear();
        // profesor 1 ya tiene Lunes 07-09 (10001): intenta ese horario y luego Lunes 13-15
        Consola k("1\n1\n10004\n1\n1\n1\n4\ns\n");
        c.crearCursoConNrc();
        CHECK_CONTAINS(k.texto(), "el profesor ya tiene el NRC 10001");
        const Aula* a = buscarCurso(c, "10004");
        CHECK(a != nullptr);
        if (a) CHECK_EQ(a->getHorario().toString(), "13:00 - 15:00");
    }
    {   // dos profesores NO pueden tener curso el mismo dia a la misma hora
        ArchivosTemporales::borrar();
        datosBasicos();
        Controlador c = crear();
        // profesor 2 intenta Lunes 07-09 (ocupado por prof 1), luego Martes 07-09 (ocupado por el mismo), luego Martes 09-11
        Consola k("1\n2\n10004\n1\n1\n2\n1\n2\n2\ns\n");
        c.crearCursoConNrc();
        std::string s = k.texto();
        CHECK_CONTAINS(s, "[Ocupado - NRC 10001]");
        CHECK_CONTAINS(s, "el LUNES de 07:00 - 09:00 ya esta ocupado por el NRC 10001");
        CHECK_CONTAINS(s, "Dos profesores no pueden coincidir el mismo dia a la misma hora");
        CHECK_CONTAINS(s, "el profesor ya tiene el NRC 10003");
        CHECK_EQ(c.getCursos().getTamanio(), 4u);
        const Aula* a = buscarCurso(c, "10004");
        CHECK(a != nullptr);
        if (a) {
            CHECK_EQ(a->getDia(), MARTES);
            CHECK_EQ(a->getHorario().toString(), "09:00 - 11:00");
        }
    }
    {   // cancelar en la confirmacion: no guarda ni registra la materia
        ArchivosTemporales::borrar();
        ArchivosTemporales::escribir("profesores.txt", "0605293885,Adrian Michael,Toa Orejuela\n");
        Controlador c = crear();
        Consola k("1\n1\n10001\n1\n1\nn\n");
        c.crearCursoConNrc();
        CHECK(c.getCursos().estaVacia());
        CHECK(c.getMaterias().estaVacia());
        CHECK_FALSE(ArchivosTemporales::existe("cursos.txt"));
        CHECK_CONTAINS(k.texto(), "Operacion cancelada");
    }
    {   // cancelar con 0 en cada paso
        ArchivosTemporales::borrar();
        datosSinCursos();
        Controlador c = crear();
        { Consola k("0\n"); c.crearCursoConNrc(); }                    // materia
        { Consola k("1\n0\n"); c.crearCursoConNrc(); }                 // profesor
        { Consola k("1\n1\n0\n"); c.crearCursoConNrc(); }              // NRC
        { Consola k("1\n1\n10001\n0\n"); c.crearCursoConNrc(); }       // dia
        { Consola k("1\n1\n10001\n1\n0\n"); c.crearCursoConNrc(); }    // franja
        CHECK(c.getCursos().estaVacia());
    }
}

// =====================================================================
//  eliminarCurso
// =====================================================================
TEST(Controlador_eliminarCurso) {
    ArchivosTemporales tmp;
    {   // ok: actualiza el archivo
        datosBasicos();
        Controlador c = crear();
        Consola k("3\ns\n");
        c.eliminarCurso();
        CHECK_EQ(c.getCursos().getTamanio(), 2u);
        CHECK(buscarCurso(c, "10003") == nullptr);
        CHECK_NOT_CONTAINS(ArchivosTemporales::leer("cursos.txt"), "10003");
        CHECK_CONTAINS(k.texto(), "Curso 10003 eliminado");
    }
    {   // con asistencias registradas no se puede eliminar
        ArchivosTemporales::borrar();
        datosBasicos();
        ArchivosTemporales::escribir("asistencias.txt", "2026-11-02,07:05,0605293885,10001,ENTRADA,PUNTUAL\n");
        Controlador c = crear();
        Consola k("1\n");
        c.eliminarCurso();
        CHECK_EQ(c.getCursos().getTamanio(), 3u);
        CHECK_CONTAINS(k.texto(), "tiene asistencias registradas");
    }
    {   // cancelar la confirmacion o la seleccion
        ArchivosTemporales::borrar();
        datosBasicos();
        Controlador c = crear();
        { Consola k("1\nn\n"); c.eliminarCurso(); CHECK_CONTAINS(k.texto(), "Operacion cancelada."); }
        { Consola k("0\n"); c.eliminarCurso(); }
        CHECK_EQ(c.getCursos().getTamanio(), 3u);
    }
    {   // sin cursos
        ArchivosTemporales::borrar();
        Controlador c = crear();
        Consola k;
        c.eliminarCurso();
        CHECK_CONTAINS(k.texto(), "No hay cursos registrados.");
    }
}

// =====================================================================
//  eliminarProfesor
// =====================================================================
TEST(Controlador_eliminarProfesor) {
    ArchivosTemporales tmp;
    {   // con cursos asignados no se puede eliminar
        datosBasicos();
        Controlador c = crear();
        Consola k("1\n");
        c.eliminarProfesor();
        CHECK_EQ(c.getProfesores().getTamanio(), 2u);
        CHECK_CONTAINS(k.texto(), "el profesor tiene 2 curso asignado");
    }
    {   // sin cursos se elimina y se actualiza el archivo
        ArchivosTemporales::borrar();
        datosBasicos();
        ArchivosTemporales::escribir("profesores.txt",
            "0605293885,Adrian Michael,Toa Orejuela\n1710034065,Ana,Almeida\n3000000012,Luis,Mora\n");
        Controlador c = crear();
        Consola k("3\ns\n");
        c.eliminarProfesor();
        CHECK_EQ(c.getProfesores().getTamanio(), 2u);
        CHECK(buscarProfesor(c, "3000000012") == nullptr);
        CHECK_NOT_CONTAINS(ArchivosTemporales::leer("profesores.txt"), "3000000012");
        CHECK_CONTAINS(k.texto(), "Profesor eliminado.");
    }
    {   // sin profesores
        ArchivosTemporales::borrar();
        Controlador vacio = crear();
        Consola k;
        vacio.eliminarProfesor();
        CHECK_CONTAINS(k.texto(), "No hay profesores registrados.");
    }
    {   // cancelar la confirmacion o la seleccion
        ArchivosTemporales::borrar();
        datosBasicos();
        ArchivosTemporales::escribir("cursos.txt", "");
        Controlador c = crear();
        { Consola k("1\nn\n"); c.eliminarProfesor(); CHECK_CONTAINS(k.texto(), "Operacion cancelada."); }
        { Consola k("0\n"); c.eliminarProfesor(); }
        CHECK_EQ(c.getProfesores().getTamanio(), 2u);
    }
}

// =====================================================================
//  listados
// =====================================================================
TEST(Controlador_listarProfesores) {
    ArchivosTemporales tmp;
    {   // ordenados por apellido, sin alterar la lista original
        datosBasicos();
        Controlador c = crear();
        Consola k;
        c.listarProfesores();
        std::string s = k.texto();
        CHECK_CONTAINS(s, "Profesores (2)");
        CHECK_CONTAINS(s, "1710034065 | Almeida Ana");
        CHECK(s.find("Almeida") < s.find("Toa Orejuela"));
        CHECK_EQ(c.getProfesores().getCabeza()->getDato().getCedula(), "0605293885");
    }
    {   // vacio
        ArchivosTemporales::borrar();
        Controlador c = crear();
        Consola k;
        c.listarProfesores();
        CHECK_CONTAINS(k.texto(), "No hay profesores registrados.");
    }
}

TEST(Controlador_listarMaterias) {
    ArchivosTemporales tmp;
    {   // ordenadas por codigo
        ArchivosTemporales::escribir("materias.txt", "EDIF201,Ecuaciones Diferenciales,4\nALGE101,Algebra Lineal,4\n");
        Controlador c = crear();
        Consola k;
        c.listarMaterias();
        CHECK(k.texto().find("ALGE101") < k.texto().find("EDIF201"));
        CHECK_CONTAINS(k.texto(), "Materias (2)");
        CHECK_CONTAINS(k.texto(), "4 creditos");
    }
    {   // vacio
        ArchivosTemporales::borrar();
        Controlador c = crear();
        Consola k;
        c.listarMaterias();
        CHECK_CONTAINS(k.texto(), "No hay materias registradas.");
    }
}

TEST(Controlador_listarCursos) {
    ArchivosTemporales tmp;
    {   // ordenados por NRC, sin columnas de periodo ni paralelo
        datosBasicos();
        ArchivosTemporales::escribir("cursos.txt",
            "10003,CALC101,1710034065,MARTES,420,540\n10001,ALGE101,0605293885,LUNES,420,540\n");
        Controlador c = crear();
        Consola k;
        c.listarCursos();
        std::string s = k.texto();
        CHECK_CONTAINS(s, "Cursos (2)");
        CHECK(s.find("10001") < s.find("10003"));
        CHECK_CONTAINS(s, "Algebra Lineal");
        CHECK_CONTAINS(s, "Almeida Ana");
        CHECK_NOT_CONTAINS(s, "Periodo");
        CHECK_NOT_CONTAINS(s, "Par ");
    }
    {   // el orden es numerico: 7 antes que 10 y que 100
        ArchivosTemporales::borrar();
        datosBasicos();
        ArchivosTemporales::escribir("cursos.txt",
            "100,ALGE101,0605293885,LUNES,420,540\n7,CALC101,0605293885,MARTES,420,540\n10,EDIF201,0605293885,MIERCOLES,420,540\n");
        Controlador c = crear();
        Consola k;
        c.listarCursos();
        std::string s = k.texto();
        CHECK(s.find("\n7 ") < s.find("\n10 "));
        CHECK(s.find("\n10 ") < s.find("\n100 "));
    }
    {   // vacio
        ArchivosTemporales::borrar();
        Controlador c = crear();
        Consola k;
        c.listarCursos();
        CHECK_CONTAINS(k.texto(), "No hay cursos registrados.");
    }
}
