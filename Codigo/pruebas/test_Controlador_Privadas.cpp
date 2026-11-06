// Pruebas unitarias de las funciones PRIVADAS del Controlador (una prueba por funcion).
//
// Las funciones privadas no se pueden llamar desde fuera de la clase. Para probarlas una por
// una, este archivo (y solo este) abre el acceso con "#define private public" ANTES de
// incluir controlador.h. Las cabeceras estandar se incluyen primero para que no se vean
// afectadas por la macro. El codigo del programa no se modifica.
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#define private public
#include "mini_test.h"
#include "fixture.h"
#undef private

// Datos base (datosBasicos): profesor 1 = 0605293885 (Toa Orejuela Adrian Michael)
//   con 10001 ALGE101 Lun 07-09 y 10002 EDIF201 Lun 09-11; profesor 2 = 1710034065 (Almeida Ana)
//   con 10003 CALC101 Mar 07-09.
// Las funciones cargar* se prueban creando el Controlador SIN el archivo, escribiendo el
// archivo despues y llamando a la funcion directamente.

static const std::string P1 = "0605293885\n";
static const std::string P2 = "1710034065\n";

// ---------------------------------------------------------------------
//  Utilidades de archivos
// ---------------------------------------------------------------------
TEST(Controlador_ruta) {
    ArchivosTemporales tmp;
    Controlador c = crear();
    CHECK_EQ(c.ruta("cursos.txt"), "ut_cursos.txt");
    Controlador sinPrefijo;
    CHECK_EQ(sinPrefijo.ruta("cursos.txt"), "cursos.txt");
    Controlador conCarpeta("datos/");
    CHECK_EQ(conCarpeta.ruta("profesores.txt"), "datos/profesores.txt");
}

TEST(Controlador_cargarProfesores) {
    ArchivosTemporales tmp;
    {   // sin archivo: no carga nada y no imprime nada
        Controlador c = crear();
        Consola k;
        c.cargarProfesores();
        CHECK(c.profesores.estaVacia());
        CHECK_EQ(k.texto(), "");
    }
    {   // lineas validas, invalidas, duplicadas y vacias
        Controlador c = crear();
        ArchivosTemporales::escribir("profesores.txt",
            "0605293885,Adrian,Toa\n"      // valida
            "123,Ana,Perez\n"              // cedula invalida
            "0605293885,Otro,Nombre\n"     // duplicada
            "1710034065,Ana\n"             // faltan columnas
            "1710034065,Ana1,Perez\n"      // nombre con numero
            "\n");                         // linea vacia: no cuenta como error
        Consola k;
        c.cargarProfesores();
        CHECK_EQ(c.profesores.getTamanio(), 1u);
        CHECK_EQ(c.profesores.getCabeza()->getDato().getNombres(), "Adrian");
        CHECK_CONTAINS(k.texto(), "Aviso: 4 linea invalida o duplicada ignorada en ut_profesores.txt.");
    }
    {   // fin de linea de Windows (CRLF) y sin avisos si todo es valido
        ArchivosTemporales::borrar();
        Controlador c = crear();
        ArchivosTemporales::escribir("profesores.txt", "0605293885,Adrian,Toa\r\n1710034065,Ana,Almeida\r\n");
        Consola k;
        c.cargarProfesores();
        CHECK_EQ(c.profesores.getTamanio(), 2u);
        CHECK_NOT_CONTAINS(k.texto(), "Aviso");
    }
}

TEST(Controlador_guardarProfesores) {
    ArchivosTemporales tmp;
    {   // escribe un profesor por linea y sobrescribe el contenido anterior
        Controlador c = crear();
        c.profesores.insertar(Profesor("0605293885", "Adrian Michael", "Toa Orejuela"));
        c.profesores.insertar(Profesor("1710034065", "Ana", "Almeida"));
        c.guardarProfesores();
        CHECK_EQ(ArchivosTemporales::leer("profesores.txt"), "0605293885,Adrian Michael,Toa Orejuela\n1710034065,Ana,Almeida\n");
        c.profesores.eliminar([](const Profesor& p) { return p.getCedula() == "1710034065"; });
        c.guardarProfesores();
        CHECK_EQ(ArchivosTemporales::leer("profesores.txt"), "0605293885,Adrian Michael,Toa Orejuela\n");
    }
    {   // lista vacia: archivo vacio
        Controlador c = crear();
        c.profesores.vaciar();
        c.guardarProfesores();
        CHECK_EQ(ArchivosTemporales::leer("profesores.txt"), "");
    }
    {   // no se puede escribir: avisa
        Controlador malo("carpeta_que_no_existe_ut/");
        malo.profesores.insertar(Profesor("0605293885", "Adrian", "Toa"));
        Consola k;
        malo.guardarProfesores();
        CHECK_CONTAINS(k.texto(), "no se pudo escribir carpeta_que_no_existe_ut/profesores.txt");
    }
}

TEST(Controlador_cargarMaterias) {
    ArchivosTemporales tmp;
    {   // sin archivo
        Controlador c = crear();
        Consola k;
        c.cargarMaterias();
        CHECK(c.materias.estaVacia());
        CHECK_EQ(k.texto(), "");
    }
    {   // validas, invalidas y duplicadas
        Controlador c = crear();
        ArchivosTemporales::escribir("materias.txt",
            "ALGE101,Algebra Lineal,4\n"   // valida
            "alge102,Minusculas,4\n"       // id invalido
            "CALC101,Calculo,0\n"          // 0 creditos
            "CALC102,Calculo 2,100\n"      // creditos de 3 digitos
            "ALGE101,Otra Cosa,3\n"        // id duplicado
            "FISI101,Fisica\n"             // faltan columnas
            "FISI101,Fisica,4\n");         // valida
        Consola k;
        c.cargarMaterias();
        CHECK_EQ(c.materias.getTamanio(), 2u);
        CHECK(c.buscarMateria("ALGE101") != nullptr);
        CHECK(c.buscarMateria("FISI101") != nullptr);
        CHECK_CONTAINS(k.texto(), "Aviso: 5 linea invalida o duplicada ignorada en ut_materias.txt.");
    }
}

TEST(Controlador_guardarMaterias) {
    ArchivosTemporales tmp;
    {
        Controlador c = crear();
        c.materias.insertar(Materia("ALGE101", "Algebra Lineal", 4));
        c.materias.insertar(Materia("CALC101", "Calculo", 5));
        c.guardarMaterias();
        CHECK_EQ(ArchivosTemporales::leer("materias.txt"), "ALGE101,Algebra Lineal,4\nCALC101,Calculo,5\n");
    }
    {   // no se puede escribir: avisa
        Controlador malo("carpeta_que_no_existe_ut/");
        Consola k;
        malo.guardarMaterias();
        CHECK_CONTAINS(k.texto(), "no se pudo escribir carpeta_que_no_existe_ut/materias.txt");
    }
}

TEST(Controlador_cargarCursos) {
    ArchivosTemporales tmp;
    {   // sin archivo
        datosSinCursos();
        Controlador c = crear();
        Consola k;
        c.cargarCursos();
        CHECK(c.cursos.estaVacia());
        CHECK_EQ(k.texto(), "");
    }
    {   // validos e invalidos: NRC (solo numeros, hasta 9 digitos, sin cero inicial), duplicados y choques
        ArchivosTemporales::borrar();
        datosSinCursos();
        Controlador c = crear();
        ArchivosTemporales::escribir("cursos.txt",
            "10001,ALGE101,0605293885,LUNES,420,540\n"      // valido
            "10002,NOEXISTE,0605293885,LUNES,540,660\n"     // materia inexistente
            "10003,ALGE101,1701234567,LUNES,540,660\n"      // profesor inexistente
            "10004,ALGE101,0605293885,LUNES,420,530\n"      // franja no oficial
            "10001,CALC101,1710034065,MARTES,420,540\n"     // NRC duplicado
            "0123,ALGE101,0605293885,LUNES,540,660\n"       // NRC con cero inicial
            "1234567890,ALGE101,0605293885,LUNES,540,660\n" // NRC de 10 digitos
            "ab12,ALGE101,0605293885,LUNES,540,660\n"       // NRC no numerico
            "10005,ALGE101,0605293885,DOMINGO,540,660\n"    // dia invalido
            "10006,ALGE101,0605293885,LUNES,abc,540\n"      // hora no numerica
            "10007,ALGE101,0605293885,LUNES\n"              // faltan columnas
            "10009,CALC101,1710034065,LUNES,420,540\n"      // otro profesor, mismo dia y hora que 10001
            "10008,CALC101,1710034065,VIERNES,1140,1260\n"  // valido
            "7,CALC101,1710034065,MARTES,420,540\n"         // valido: NRC corto
            "999999999,EDIF201,1710034065,JUEVES,420,540\n");// valido: NRC de 9 digitos
        Consola k;
        c.cargarCursos();
        CHECK_EQ(c.cursos.getTamanio(), 4u);
        CHECK(c.buscarCurso("10001") != nullptr);
        CHECK(c.buscarCurso("10008") != nullptr);
        CHECK(c.buscarCurso("7") != nullptr);
        CHECK(c.buscarCurso("999999999") != nullptr);
        CHECK(c.buscarCurso("10009") == nullptr);
        CHECK_CONTAINS(k.texto(), "Aviso: 11 linea invalida o duplicada ignorada en ut_cursos.txt.");
    }
    {   // formato antiguo de 8 columnas (con periodo y paralelo): se migra
        ArchivosTemporales::borrar();
        datosSinCursos();
        Controlador c = crear();
        ArchivosTemporales::escribir("cursos.txt",
            "10001,ALGE101,202610,0605293885,A,LUNES,420,540\n"     // formato antiguo
            "10002,EDIF201,0605293885,LUNES,540,660\n");            // formato nuevo
        c.cargarCursos();
        CHECK_EQ(c.cursos.getTamanio(), 2u);
        const Aula* a = c.buscarCurso("10001");
        CHECK(a != nullptr);
        if (a) {
            CHECK_EQ(a->getIdMateria(), "ALGE101");
            CHECK_EQ(a->getCedulaProfesor(), "0605293885");
            CHECK_EQ(a->getHorario().toString(), "07:00 - 09:00");
        }
    }
}

TEST(Controlador_guardarCursos) {
    ArchivosTemporales tmp;
    {
        Controlador c = crear();
        c.cursos.insertar(Aula("10001", "ALGE101", "0605293885", LUNES, IntervaloHorario::franja(1)));
        c.cursos.insertar(Aula("7", "CALC101", "1710034065", VIERNES, IntervaloHorario::franja(7)));
        c.guardarCursos();
        CHECK_EQ(ArchivosTemporales::leer("cursos.txt"),
                 "10001,ALGE101,0605293885,LUNES,420,540\n7,CALC101,1710034065,VIERNES,1140,1260\n");
    }
    {   // lo guardado se vuelve a cargar igual
        datosBasicos();
        Controlador c = crear();
        c.guardarCursos();
        Controlador otra = crear();
        CHECK_EQ(otra.getCursos().getTamanio(), 3u);
    }
    {   // no se puede escribir: avisa
        Controlador malo("carpeta_que_no_existe_ut/");
        Consola k;
        malo.guardarCursos();
        CHECK_CONTAINS(k.texto(), "no se pudo escribir carpeta_que_no_existe_ut/cursos.txt");
    }
}

TEST(Controlador_cargarAsistencias) {
    ArchivosTemporales tmp;
    {   // sin archivo
        datosBasicos();
        Controlador c = crear();
        Consola k;
        c.cargarAsistencias();
        CHECK(c.registros.estaVacia());
        CHECK_EQ(k.texto(), "");
    }
    {   // validas e invalidas
        ArchivosTemporales::borrar();
        datosBasicos();
        Controlador c = crear();
        ArchivosTemporales::escribir("asistencias.txt",
            "2026-11-02,07:05,0605293885,10001,ENTRADA,PUNTUAL\n"   // valida
            "2026-11-02,08:55,0605293885,10001,SALIDA,REGISTRADA\n" // valida
            "2026-13-02,07:05,0605293885,10001,ENTRADA,PUNTUAL\n"   // fecha invalida
            "2026-11-02,25:05,0605293885,10001,ENTRADA,PUNTUAL\n"   // hora invalida
            "2026-11-02,07:05,0605293885,10001,OTRO,PUNTUAL\n"      // tipo invalido
            "2026-11-02,07:05,0605293885,abc,ENTRADA,PUNTUAL\n"     // nrc no numerico
            "2026-11-02,07:05,0605293886,10001,ENTRADA,PUNTUAL\n"   // cedula invalida
            "2026-11-02,07:05,0605293885,10001,ENTRADA,REGISTRADA\n"// estado incoherente con el tipo
            "2026-11-02,07:05,0605293885,10001,ENTRADA\n"           // faltan columnas
            "2026-11-02,07:06,0605293885,10001,ENTRADA,PUNTUAL\n"); // ingreso repetido el mismo dia
        Consola k;
        c.cargarAsistencias();
        CHECK_EQ(c.registros.getTamanio(), 2u);
        CHECK_EQ(c.registros.getCabeza()->getDato().getEstado(), "PUNTUAL");
        CHECK_CONTAINS(k.texto(), "Aviso: 8 linea invalida o duplicada ignorada en ut_asistencias.txt.");
    }
    {   // dos profesores no pueden tener ingreso el mismo dia a la misma hora
        ArchivosTemporales::borrar();
        datosBasicos();
        Controlador c = crear();
        ArchivosTemporales::escribir("asistencias.txt",
            "2026-11-02,07:05,0605293885,10001,ENTRADA,PUNTUAL\n"   // profesor 1
            "2026-11-02,07:06,1710034065,10003,ENTRADA,PUNTUAL\n"   // profesor 2, horario 07-09 cruzado: se ignora
            "2026-11-03,07:06,1710034065,10003,ENTRADA,PUNTUAL\n"); // otro dia: valida
        Consola k;
        c.cargarAsistencias();
        CHECK_EQ(c.registros.getTamanio(), 2u);
        CHECK_CONTAINS(k.texto(), "Aviso: 1 linea");
    }
}

TEST(Controlador_guardarAsistenciaArchivo) {
    ArchivosTemporales tmp;
    {   // agrega una linea por registro, sin borrar las anteriores
        Controlador c = crear();
        c.guardarAsistenciaArchivo(RegistroAsistencia("2026-11-02", "07:05", "0605293885", "10001", "ENTRADA", "PUNTUAL"));
        c.guardarAsistenciaArchivo(RegistroAsistencia("2026-11-02", "08:55", "0605293885", "10001", "SALIDA", "REGISTRADA"));
        CHECK_EQ(ArchivosTemporales::leer("asistencias.txt"),
                 "2026-11-02,07:05,0605293885,10001,ENTRADA,PUNTUAL\n2026-11-02,08:55,0605293885,10001,SALIDA,REGISTRADA\n");
    }
    {   // no se puede escribir: avisa
        Controlador malo("carpeta_que_no_existe_ut/");
        Consola k;
        malo.guardarAsistenciaArchivo(RegistroAsistencia("2026-11-02", "07:05", "0605293885", "10001", "ENTRADA", "PUNTUAL"));
        CHECK_CONTAINS(k.texto(), "no se pudo escribir carpeta_que_no_existe_ut/asistencias.txt");
    }
}

// ---------------------------------------------------------------------
//  Busquedas
// ---------------------------------------------------------------------
TEST(Controlador_buscarProfesor) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    Profesor* p = c.buscarProfesor("1710034065");
    CHECK(p != nullptr);
    if (p) CHECK_EQ(p->getApellidos(), "Almeida");
    CHECK(c.buscarProfesor("1701234567") == nullptr);
    CHECK(c.buscarProfesor("") == nullptr);
}

TEST(Controlador_buscarMateria) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    Materia* m = c.buscarMateria("CALC101");
    CHECK(m != nullptr);
    if (m) CHECK_EQ(m->getNombre(), "Calculo 1");
    CHECK(c.buscarMateria("NOEXISTE") == nullptr);
}

TEST(Controlador_buscarCurso) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    Aula* a = c.buscarCurso("10002");
    CHECK(a != nullptr);
    if (a) CHECK_EQ(a->getIdMateria(), "EDIF201");
    CHECK(c.buscarCurso("99999") == nullptr);
}

TEST(Controlador_buscarRegistro) {
    ArchivosTemporales tmp;
    datosBasicos();
    ArchivosTemporales::escribir("asistencias.txt",
        "2026-11-02,07:05,0605293885,10001,ENTRADA,PUNTUAL\n2026-11-02,08:55,0605293885,10001,SALIDA,REGISTRADA\n");
    Controlador c = crear();
    const RegistroAsistencia* e = c.buscarRegistro("2026-11-02", "10001", "ENTRADA");
    CHECK(e != nullptr);
    if (e) CHECK_EQ(e->getHora(), "07:05");
    const RegistroAsistencia* s = c.buscarRegistro("2026-11-02", "10001", "SALIDA");
    CHECK(s != nullptr);
    if (s) CHECK_EQ(s->getHora(), "08:55");
    CHECK(c.buscarRegistro("2026-11-09", "10001", "ENTRADA") == nullptr);   // otra fecha
    CHECK(c.buscarRegistro("2026-11-02", "10002", "ENTRADA") == nullptr);   // otro NRC
}

TEST(Controlador_buscarChoqueDeHorario) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    const Aula* a = c.buscarChoqueDeHorario(LUNES, IntervaloHorario::franja(1), "");
    CHECK(a != nullptr);
    if (a) CHECK_EQ(a->getNrc(), "10001");
    const Aula* b = c.buscarChoqueDeHorario(LUNES, IntervaloHorario::franja(2), "");
    CHECK(b != nullptr);
    if (b) CHECK_EQ(b->getNrc(), "10002");
    // sin profesor: el choque se busca solo por dia y hora
    const Aula* m = c.buscarChoqueDeHorario(MARTES, IntervaloHorario::franja(1), "");
    CHECK(m != nullptr);
    if (m) CHECK_EQ(m->getNrc(), "10003");
    // libre
    CHECK(c.buscarChoqueDeHorario(LUNES, IntervaloHorario::franja(3), "") == nullptr);
    CHECK(c.buscarChoqueDeHorario(MIERCOLES, IntervaloHorario::franja(1), "") == nullptr);
    // traslapado: 08:00-10:00 cruza con 10001 (07-09) y con 10002 (09-11)
    const Aula* t = c.buscarChoqueDeHorario(LUNES, IntervaloHorario(480, 600), "");
    CHECK(t != nullptr);
    if (t) CHECK_EQ(t->getNrc(), "10001");
    // el NRC excluido no cuenta (edicion del mismo curso)
    CHECK(c.buscarChoqueDeHorario(MARTES, IntervaloHorario::franja(1), "10003") == nullptr);
    const Aula* sinPrimero = c.buscarChoqueDeHorario(LUNES, IntervaloHorario(480, 600), "10001");
    CHECK(sinPrimero != nullptr);
    if (sinPrimero) CHECK_EQ(sinPrimero->getNrc(), "10002");
}

TEST(Controlador_buscarIngresoDeOtroProfesor) {
    ArchivosTemporales tmp;
    datosBasicos();
    ArchivosTemporales::escribir("asistencias.txt",
        "2026-11-02,07:05,1710034065,10003,ENTRADA,PUNTUAL\n"   // profesor 2, horario 07-09
        "2026-11-04,08:55,1710034065,10003,SALIDA,REGISTRADA\n" // una salida no cuenta
        "2026-11-09,07:05,1710034065,99999,ENTRADA,PUNTUAL\n"); // curso que no existe: no cuenta
    Controlador c = crear();
    const RegistroAsistencia* r = c.buscarIngresoDeOtroProfesor("2026-11-02", "0605293885", IntervaloHorario::franja(1));
    CHECK(r != nullptr);
    if (r) CHECK_EQ(r->getCedula(), "1710034065");
    // el mismo profesor no se bloquea a si mismo
    CHECK(c.buscarIngresoDeOtroProfesor("2026-11-02", "1710034065", IntervaloHorario::franja(1)) == nullptr);
    // otra fecha, o un horario que no se cruza
    CHECK(c.buscarIngresoDeOtroProfesor("2026-11-03", "0605293885", IntervaloHorario::franja(1)) == nullptr);
    CHECK(c.buscarIngresoDeOtroProfesor("2026-11-02", "0605293885", IntervaloHorario::franja(2)) == nullptr);
    // solo cuentan las ENTRADAS y los cursos existentes
    CHECK(c.buscarIngresoDeOtroProfesor("2026-11-04", "0605293885", IntervaloHorario::franja(1)) == nullptr);
    CHECK(c.buscarIngresoDeOtroProfesor("2026-11-09", "0605293885", IntervaloHorario::franja(1)) == nullptr);
}

TEST(Controlador_nombreMateria) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    CHECK_EQ(c.nombreMateria("ALGE101"), "Algebra Lineal");
    CHECK_EQ(c.nombreMateria("NOEXISTE"), "(materia desconocida)");
}

TEST(Controlador_nombreProfesor) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    CHECK_EQ(c.nombreProfesor("0605293885"), "Toa Orejuela Adrian Michael");
    CHECK_EQ(c.nombreProfesor("1701234567"), "(profesor desconocido)");
}

// ---------------------------------------------------------------------
//  Selecciones por teclado
// ---------------------------------------------------------------------
TEST(Controlador_seleccionarProfesor) {
    ArchivosTemporales tmp;
    {   // sin profesores
        Controlador c = crear();
        Consola k;
        CHECK(c.seleccionarProfesor() == nullptr);
        CHECK_CONTAINS(k.texto(), "No hay profesores registrados.");
    }
    {   // lista y elige
        datosBasicos();
        Controlador c = crear();
        Consola k("2\n");
        Profesor* p = c.seleccionarProfesor();
        CHECK(p != nullptr);
        if (p) CHECK_EQ(p->getCedula(), "1710034065");
        CHECK_CONTAINS(k.texto(), "1. 0605293885 - Toa Orejuela Adrian Michael");
        CHECK_CONTAINS(k.texto(), "2. 1710034065 - Almeida Ana");
    }
    {   // 0 cancela; un numero fuera de rango repite la pregunta
        Controlador c = crear();
        { Consola k("0\n"); CHECK(c.seleccionarProfesor() == nullptr); }
        { Consola k("3\nabc\n1\n"); Profesor* p = c.seleccionarProfesor(); CHECK(p != nullptr);
          if (p) CHECK_EQ(p->getCedula(), "0605293885");
          CHECK_CONTAINS(k.texto(), "el valor debe estar entre 0 y 2"); }
    }
}

TEST(Controlador_seleccionarMateriaDeCatalogo) {
    ArchivosTemporales tmp;
    datosSinCursos();
    ArchivosTemporales::escribir("materias.txt", "ALGE101,Algebra Lineal,6\n");
    Controlador c = crear();
    {   // muestra las 8 materias fijas
        Consola k("0\n");
        Materia m;
        CHECK_FALSE(c.seleccionarMateriaDeCatalogo(m));
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
    }
    {   // materia ya registrada: devuelve la registrada (con sus creditos)
        Consola k("4\n");
        Materia m;
        CHECK(c.seleccionarMateriaDeCatalogo(m));
        CHECK_EQ(m.getId(), "ALGE101");
        CHECK_EQ(m.getNombre(), "Algebra Lineal");
        CHECK_EQ(m.getCreditos(), 6);
    }
    {   // materia nueva: devuelve los datos del catalogo, sin registrarla todavia
        Consola k("8\n");
        Materia m;
        CHECK(c.seleccionarMateriaDeCatalogo(m));
        CHECK_EQ(m.getId(), "QUIM101");
        CHECK_EQ(m.getNombre(), "Quimica");
        CHECK_EQ(m.getCreditos(), 4);
        CHECK(c.buscarMateria("QUIM101") == nullptr);
        CHECK_EQ(c.materias.getTamanio(), 1u);
    }
    {   // cancelar no modifica la materia recibida; fuera de rango repite
        Materia m("XXXX", "Sin tocar", 9);
        { Consola k("0\n"); CHECK_FALSE(c.seleccionarMateriaDeCatalogo(m)); }
        CHECK_EQ(m.getId(), "XXXX");
        { Consola k("9\nabc\n1\n"); CHECK(c.seleccionarMateriaDeCatalogo(m));
          CHECK_CONTAINS(k.texto(), "el valor debe estar entre 0 y 8"); }
        CHECK_EQ(m.getId(), "CALC101");
    }
}

TEST(Controlador_seleccionarCurso) {
    ArchivosTemporales tmp;
    {   // sin cursos
        Controlador c = crear();
        Consola k;
        CHECK(c.seleccionarCurso() == nullptr);
        CHECK_CONTAINS(k.texto(), "No hay cursos registrados.");
    }
    {   // lista y elige
        datosBasicos();
        Controlador c = crear();
        Consola k("2\n");
        Aula* a = c.seleccionarCurso();
        CHECK(a != nullptr);
        if (a) CHECK_EQ(a->getNrc(), "10002");
        CHECK_CONTAINS(k.texto(), "1. NRC 10001 - Algebra Lineal - LUNES 07:00 - 09:00 - Toa Orejuela Adrian Michael");
        CHECK_CONTAINS(k.texto(), "3. NRC 10003 - Calculo 1 - MARTES 07:00 - 09:00 - Almeida Ana");
    }
    {   // 0 cancela; fuera de rango repite
        Controlador c = crear();
        { Consola k("0\n"); CHECK(c.seleccionarCurso() == nullptr); }
        { Consola k("4\n3\n"); Aula* a = c.seleccionarCurso(); CHECK(a != nullptr);
          if (a) CHECK_EQ(a->getNrc(), "10003");
          CHECK_CONTAINS(k.texto(), "el valor debe estar entre 0 y 3"); }
    }
}

TEST(Controlador_identificarProfesor) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    {   // cedula registrada
        Consola k(P1);
        Profesor* p = c.identificarProfesor();
        CHECK(p != nullptr);
        if (p) CHECK_EQ(p->getApellidos(), "Toa Orejuela");
        CHECK_CONTAINS(k.texto(), "bienvenido, Adrian Michael Toa Orejuela");
    }
    {   // 0 cancela
        Consola k("0\n");
        CHECK(c.identificarProfesor() == nullptr);
    }
    {   // cedula invalida
        Consola k("123\n");
        CHECK(c.identificarProfesor() == nullptr);
        CHECK_CONTAINS(k.texto(), "la cedula ingresada no es valida");
    }
    {   // cedula valida pero no registrada
        Consola k("1701234567\n");
        CHECK(c.identificarProfesor() == nullptr);
        CHECK_CONTAINS(k.texto(), "Profesor no encontrado en el sistema");
    }
}

TEST(Controlador_seleccionarCursoDeProfesor) {
    ArchivosTemporales tmp;
    datosBasicos();
    ArchivosTemporales::escribir("profesores.txt",
        "0605293885,Adrian Michael,Toa Orejuela\n1710034065,Ana,Almeida\n3000000012,Luis,Mora\n");
    Controlador c = crear();
    {   // solo lista y devuelve los cursos del profesor
        Consola k("2\n");
        Aula* a = c.seleccionarCursoDeProfesor(*c.buscarProfesor("0605293885"));
        CHECK(a != nullptr);
        if (a) CHECK_EQ(a->getNrc(), "10002");
        CHECK_CONTAINS(k.texto(), "1. NRC 10001 - Algebra Lineal - LUNES 07:00 - 09:00");
        CHECK_NOT_CONTAINS(k.texto(), "10003");
    }
    {   // 0 cancela; fuera de rango repite
        { Consola k("0\n"); CHECK(c.seleccionarCursoDeProfesor(*c.buscarProfesor("1710034065")) == nullptr); }
        { Consola k("2\n1\n"); Aula* a = c.seleccionarCursoDeProfesor(*c.buscarProfesor("1710034065"));
          CHECK(a != nullptr);
          if (a) CHECK_EQ(a->getNrc(), "10003");
          CHECK_CONTAINS(k.texto(), "el valor debe estar entre 0 y 1"); }
    }
    {   // profesor sin cursos
        Consola k;
        CHECK(c.seleccionarCursoDeProfesor(*c.buscarProfesor("3000000012")) == nullptr);
        CHECK_CONTAINS(k.texto(), "No tiene cursos asignados.");
    }
}

TEST(Controlador_pedirDiaYFranja) {
    ArchivosTemporales tmp;
    datosBasicos();
    DiaSemana dia = LUNES;
    IntervaloHorario h;
    {   // dia y franja libres; marca los horarios ocupados
        Controlador c = crear();
        Consola k("2\n3\n");                       // Martes, franja 3 (11:00-13:00)
        CHECK(c.pedirDiaYFranja("1710034065", "", dia, h));
        CHECK_EQ(dia, MARTES);
        CHECK_EQ(h.toString(), "11:00 - 13:00");
        CHECK_CONTAINS(k.texto(), "Franjas horarias del MARTES:");
        CHECK_CONTAINS(k.texto(), "1. 07:00 - 09:00  [Ocupado - NRC 10003]");
    }
    {   // cancelar en el dia o en la franja: false y no modifica los resultados
        Controlador c = crear();
        dia = VIERNES; h = IntervaloHorario(1, 2);
        { Consola k("0\n"); CHECK_FALSE(c.pedirDiaYFranja("1710034065", "", dia, h)); }
        { Consola k("2\n0\n"); CHECK_FALSE(c.pedirDiaYFranja("1710034065", "", dia, h)); }
        CHECK_EQ(dia, VIERNES);
        CHECK_EQ(h.getHoraInicioMinutos(), 1);
    }
    {   // choque con un curso del MISMO profesor: lo indica y vuelve a pedir
        Controlador c = crear();
        Consola k("2\n1\n2\n2\n");                 // Martes franja 1 (ocupada por 10003), luego Martes franja 2
        CHECK(c.pedirDiaYFranja("1710034065", "", dia, h));
        CHECK_CONTAINS(k.texto(), "el profesor ya tiene el NRC 10003 (Calculo 1) el MARTES de 07:00 - 09:00");
        CHECK_EQ(h.toString(), "09:00 - 11:00");
    }
    {   // choque con un curso de OTRO profesor: dos profesores no pueden coincidir
        Controlador c = crear();
        Consola k("2\n1\n2\n2\n");
        CHECK(c.pedirDiaYFranja("0605293885", "", dia, h));
        CHECK_CONTAINS(k.texto(), "el MARTES de 07:00 - 09:00 ya esta ocupado por el NRC 10003 (Calculo 1) del profesor Almeida Ana");
        CHECK_CONTAINS(k.texto(), "Dos profesores no pueden coincidir el mismo dia a la misma hora");
    }
    {   // el NRC excluido (el curso que se edita) no cuenta como choque
        Controlador c = crear();
        Consola k("2\n1\n");
        CHECK(c.pedirDiaYFranja("1710034065", "10003", dia, h));
        CHECK_EQ(dia, MARTES);
        CHECK_EQ(h.toString(), "07:00 - 09:00");
    }
    {   // dia sin franjas libres: avisa y vuelve a pedir el dia
        ArchivosTemporales::escribir("cursos.txt",
            "1,ALGE101,0605293885,VIERNES,420,540\n2,ALGE101,0605293885,VIERNES,540,660\n"
            "3,ALGE101,0605293885,VIERNES,660,780\n4,ALGE101,0605293885,VIERNES,780,900\n"
            "5,ALGE101,0605293885,VIERNES,900,1020\n6,ALGE101,0605293885,VIERNES,1020,1140\n"
            "7,ALGE101,0605293885,VIERNES,1140,1260\n");
        Controlador c = crear();
        CHECK_EQ(c.getCursos().getTamanio(), 7u);
        Consola k("5\n1\n3\n");                    // Viernes (lleno) -> Lunes franja 3
        CHECK(c.pedirDiaYFranja("0605293885", "", dia, h));
        CHECK_CONTAINS(k.texto(), "el VIERNES no tiene franjas libres");
        CHECK_EQ(dia, LUNES);
        CHECK_EQ(h.toString(), "11:00 - 13:00");
    }
}

// ---------------------------------------------------------------------
//  Reloj y marcado
// ---------------------------------------------------------------------
TEST(Controlador_mostrarEstadoReloj) {
    ArchivosTemporales tmp;
    Controlador c = crear();
    {   // reloj simulado en dia de clase
        RelojFijo fijo(LUNES_FECHA, 7, 5);
        Consola k;
        c.mostrarEstadoReloj();
        CHECK_EQ(k.texto(), "[SIMULADO] 2026-11-02 07:05 (LUNES)\n");
    }
    {   // fin de semana
        RelojFijo fijo(SABADO_FECHA, 21, 0);
        Consola k;
        c.mostrarEstadoReloj();
        CHECK_EQ(k.texto(), "[SIMULADO] 2026-11-07 21:00 (FIN DE SEMANA)\n");
    }
    {   // reloj real
        Consola k;
        c.mostrarEstadoReloj();
        CHECK_CONTAINS(k.texto(), "[REAL] ");
    }
}

TEST(Controlador_registrarMarcado) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    {   // ingreso (true)
        RelojFijo reloj(LUNES_FECHA, 7, 5);
        Consola k(P1 + "1\n");
        c.registrarMarcado(true);
        CHECK_CONTAINS(k.texto(), "Registro de ingreso");
        CHECK_CONTAINS(k.texto(), "Ingreso registrado a las 07:05. Estado: PUNTUAL.");
        CHECK_EQ(c.registros.getTamanio(), 1u);
    }
    {   // salida (false)
        RelojFijo reloj(LUNES_FECHA, 8, 55);
        Consola k(P1 + "1\n");
        c.registrarMarcado(false);
        CHECK_CONTAINS(k.texto(), "Registro de salida");
        CHECK_CONTAINS(k.texto(), "Salida registrada a las 08:55 para el NRC 10001.");
        CHECK_EQ(c.registros.getTamanio(), 2u);
    }
    {   // cancelar la identificacion o la seleccion de curso: no registra
        RelojFijo reloj(LUNES_FECHA, 9, 3);
        { Consola k("0\n"); c.registrarMarcado(true); }
        { Consola k(P1 + "0\n"); c.registrarMarcado(true); }
        CHECK_EQ(c.registros.getTamanio(), 2u);
    }
}

TEST(Controlador_pruebaDeHora) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    {   // prueba de ingreso (true)
        Consola k(P1 + "1\n7:05\n3\n");
        c.pruebaDeHora(true);
        CHECK_CONTAINS(k.texto(), "prueba de login");
        CHECK_CONTAINS(k.texto(), "07:05  ingreso a tiempo");
    }
    {   // prueba de salida (false)
        Consola k(P1 + "1\n8:55\n3\n");
        c.pruebaDeHora(false);
        CHECK_CONTAINS(k.texto(), "prueba de deslogin");
        CHECK_CONTAINS(k.texto(), "08:55  salida a tiempo");
    }
    {   // cancelar con 0 en cada paso y no guardar nada
        { Consola k("0\n"); c.pruebaDeHora(true); }
        { Consola k(P1 + "0\n"); c.pruebaDeHora(false); }
        { Consola k(P1 + "1\n0\n"); c.pruebaDeHora(true); }
        CHECK(c.registros.estaVacia());
        CHECK_FALSE(ArchivosTemporales::existe("asistencias.txt"));
    }
}
