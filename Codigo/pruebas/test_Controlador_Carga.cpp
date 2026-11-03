#include "mini_test.h"
#include "fixture.h"

TEST(CtrlCarga_sinArchivos_todoVacio) {
    ArchivosTemporales tmp;
    Controlador c = crear();
    CHECK(c.getProfesores().estaVacia());
    CHECK(c.getMaterias().estaVacia());
    CHECK(c.getCursos().estaVacia());
    CHECK(c.getRegistros().estaVacia());
}

TEST(CtrlCarga_datosValidos) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    CHECK_EQ(c.getProfesores().getTamanio(), 2u);
    CHECK_EQ(c.getMaterias().getTamanio(), 3u);
    CHECK_EQ(c.getCursos().getTamanio(), 3u);

    const Aula* a = buscarCurso(c, "10002");
    CHECK(a != nullptr);
    if (a) {
        CHECK_EQ(a->getIdMateria(), "EDIF201");
        CHECK_EQ(a->getCedulaProfesor(), "0605293885");
        CHECK_EQ(a->getDia(), LUNES);
        CHECK_EQ(a->getHorario().toString(), "09:00 - 11:00");
    }
    const Profesor* p = buscarProfesor(c, "1710034065");
    CHECK(p != nullptr);
    if (p) CHECK_EQ(p->getApellidos(), "Almeida");
}

TEST(CtrlCarga_archivosConFinDeLineaWindows) {
    ArchivosTemporales tmp;
    ArchivosTemporales::escribir("profesores.txt", "0605293885,Adrian,Toa\r\n1710034065,Ana,Almeida\r\n");
    ArchivosTemporales::escribir("materias.txt", "ALGE101,Algebra Lineal,4\r\n");
    ArchivosTemporales::escribir("cursos.txt", "10001,ALGE101,0605293885,LUNES,420,540\r\n");
    Controlador c = crear();
    CHECK_EQ(c.getProfesores().getTamanio(), 2u);
    CHECK_EQ(c.getMaterias().getTamanio(), 1u);
    CHECK_EQ(c.getCursos().getTamanio(), 1u);
}

TEST(CtrlCarga_profesoresInvalidosOSDuplicados_seIgnoranConAviso) {
    ArchivosTemporales tmp;
    ArchivosTemporales::escribir("profesores.txt",
        "0605293885,Adrian,Toa\n"      // valida
        "123,Ana,Perez\n"              // cedula invalida
        "0605293885,Otro,Nombre\n"     // duplicada
        "1710034065,Ana\n"             // faltan columnas
        "1710034065,Ana1,Perez\n"      // nombre con numero
        "\n");                         // linea vacia: no cuenta como error
    Consola consola;
    Controlador c = crear();
    CHECK_EQ(c.getProfesores().getTamanio(), 1u);
    CHECK_CONTAINS(consola.texto(), "Aviso: 4 linea(s) invalida(s)");
    CHECK_CONTAINS(consola.texto(), "profesores.txt");
}

TEST(CtrlCarga_materiasInvalidasODuplicadas_seIgnoran) {
    ArchivosTemporales tmp;
    ArchivosTemporales::escribir("materias.txt",
        "ALGE101,Algebra Lineal,4\n"   // valida
        "alge102,Minusculas,4\n"       // id invalido
        "CALC101,Calculo,0\n"          // 0 creditos
        "CALC102,Calculo 2,100\n"      // creditos de 3 digitos
        "ALGE101,Otra Cosa,3\n"        // id duplicado
        "FISI101,Fisica\n"             // faltan columnas
        "FISI101,Fisica,4\n");         // valida
    Consola consola;
    Controlador c = crear();
    CHECK_EQ(c.getMaterias().getTamanio(), 2u);
    CHECK(buscarMateria(c, "ALGE101") != nullptr);
    CHECK(buscarMateria(c, "FISI101") != nullptr);
    CHECK_CONTAINS(consola.texto(), "Aviso: 5 linea(s)");
}

TEST(CtrlCarga_cursosInvalidos_seIgnoran) {
    ArchivosTemporales tmp;
    datosBasicos();
    ArchivosTemporales::escribir("cursos.txt",
        "10001,ALGE101,0605293885,LUNES,420,540\n"      // valido
        "10002,NOEXISTE,0605293885,LUNES,540,660\n"     // materia inexistente
        "10003,ALGE101,1701234567,LUNES,540,660\n"      // profesor inexistente
        "10004,ALGE101,0605293885,LUNES,420,530\n"      // franja no oficial
        "10001,CALC101,1710034065,MARTES,420,540\n"     // NRC duplicado
        "1234,ALGE101,0605293885,LUNES,540,660\n"       // NRC con 4 digitos
        "10005,ALGE101,0605293885,DOMINGO,540,660\n"    // dia invalido
        "10006,ALGE101,0605293885,LUNES,abc,540\n"      // hora no numerica
        "10007,ALGE101,0605293885,LUNES\n"              // faltan columnas
        "10008,CALC101,1710034065,VIERNES,1140,1260\n");// valido
    Consola consola;
    Controlador c = crear();
    CHECK_EQ(c.getCursos().getTamanio(), 2u);
    CHECK(buscarCurso(c, "10001") != nullptr);
    CHECK(buscarCurso(c, "10008") != nullptr);
    CHECK_CONTAINS(consola.texto(), "Aviso: 8 linea(s)");
}

TEST(CtrlCarga_formatoAntiguoDeOchoColumnas_seMigra) {
    ArchivosTemporales tmp;
    datosBasicos();
    ArchivosTemporales::escribir("cursos.txt",
        "10001,ALGE101,202610,0605293885,A,LUNES,420,540\n"     // formato antiguo (periodo + paralelo)
        "10002,EDIF201,0605293885,LUNES,540,660\n");            // formato nuevo
    Controlador c = crear();
    CHECK_EQ(c.getCursos().getTamanio(), 2u);
    const Aula* a = buscarCurso(c, "10001");
    CHECK(a != nullptr);
    if (a) {
        CHECK_EQ(a->getIdMateria(), "ALGE101");
        CHECK_EQ(a->getCedulaProfesor(), "0605293885");
        CHECK_EQ(a->getHorario().toString(), "07:00 - 09:00");
    }
}

TEST(CtrlCarga_formatoAntiguo_alGuardarQuedaEnFormatoNuevo) {
    ArchivosTemporales tmp;
    datosBasicos();
    ArchivosTemporales::escribir("cursos.txt", "10001,ALGE101,202610,0605293885,A,LUNES,420,540\n");
    Controlador c = crear();
    Consola consola;
    CHECK(c.agregarHorario("10001", MIERCOLES, IntervaloHorario::franja(3)));
    CHECK_EQ(ArchivosTemporales::leer("cursos.txt"), "10001,ALGE101,0605293885,MIERCOLES,660,780\n");
}

TEST(CtrlCarga_cursosSinProfesoresNiMaterias_seDescartan) {
    ArchivosTemporales tmp;
    ArchivosTemporales::escribir("cursos.txt", "10001,ALGE101,0605293885,LUNES,420,540\n");
    Consola consola;
    Controlador c = crear();
    CHECK(c.getCursos().estaVacia());
    CHECK_CONTAINS(consola.texto(), "Aviso: 1 linea(s)");
}

TEST(CtrlCarga_asistencias) {
    ArchivosTemporales tmp;
    datosBasicos();
    ArchivosTemporales::escribir("asistencias.txt",
        "2026-11-02,07:05,0605293885,10001,ENTRADA,PUNTUAL\n"   // valida
        "2026-11-02,08:55,0605293885,10001,SALIDA,REGISTRADA\n" // valida
        "2026-13-02,07:05,0605293885,10001,ENTRADA,PUNTUAL\n"   // fecha invalida
        "2026-11-02,25:05,0605293885,10001,ENTRADA,PUNTUAL\n"   // hora invalida
        "2026-11-02,07:05,0605293885,10001,OTRO,PUNTUAL\n"      // tipo invalido
        "2026-11-02,07:05,0605293885,abc,ENTRADA,PUNTUAL\n"     // nrc no numerico
        "2026-11-02,07:05,0605293885,10001,ENTRADA\n");         // faltan columnas
    Consola consola;
    Controlador c = crear();
    CHECK_EQ(c.getRegistros().getTamanio(), 2u);
    CHECK_EQ(c.getRegistros().getCabeza()->getDato().getEstado(), "PUNTUAL");
    CHECK_CONTAINS(consola.texto(), "Aviso: 5 linea(s)");
}

TEST(CtrlCarga_getters_devuelvenListasConsistentes) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    CHECK_EQ(c.getProfesores().getCabeza()->getDato().getCedula(), "0605293885");
    CHECK_EQ(c.getMaterias().getCola()->getDato().getId(), "EDIF201");
    CHECK_EQ(c.getCursos().getCola()->getDato().getNrc(), "10003");
}
