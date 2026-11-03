#include "mini_test.h"
#include "fixture.h"

// =============== registrarProfesor ===============
TEST(CtrlProfesor_registrar_ok_normalizaEspaciosYGuarda) {
    ArchivosTemporales tmp;
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

TEST(CtrlProfesor_registrar_cedulaInvalida_repitePregunta) {
    ArchivosTemporales tmp;
    Controlador c = crear();
    Consola k("123\n0605293886\n1710034065\nAna\nPerez\n");
    c.registrarProfesor();
    CHECK_EQ(c.getProfesores().getTamanio(), 1u);
    CHECK_CONTAINS(k.texto(), "no es valida en Ecuador");
}

TEST(CtrlProfesor_registrar_cedulaDuplicada) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    Consola k("0605293885\n0\n");
    c.registrarProfesor();
    CHECK_EQ(c.getProfesores().getTamanio(), 2u);
    CHECK_CONTAINS(k.texto(), "Ya existe un profesor");
}

TEST(CtrlProfesor_registrar_nombreOApellidoInvalido_repite) {
    ArchivosTemporales tmp;
    Controlador c = crear();
    Consola k("1710034065\nAna1\nA\nAna\nPe@rez\nPerez\n");
    c.registrarProfesor();
    CHECK_EQ(c.getProfesores().getTamanio(), 1u);
    CHECK_CONTAINS(k.texto(), "el nombre solo debe contener letras");
    CHECK_CONTAINS(k.texto(), "el apellido solo debe contener letras");
}

TEST(CtrlProfesor_registrar_cancelarConCero_noGuardaNada) {
    ArchivosTemporales tmp;
    Controlador c = crear();
    { Consola k("0\n"); c.registrarProfesor(); }
    { Consola k("1710034065\n0\n"); c.registrarProfesor(); }
    { Consola k("1710034065\nAna\n0\n"); c.registrarProfesor(); }
    CHECK(c.getProfesores().estaVacia());
    CHECK_FALSE(ArchivosTemporales::existe("profesores.txt"));
}

TEST(CtrlProfesor_registrar_persisteEntreInstancias) {
    ArchivosTemporales tmp;
    { Controlador c = crear(); Consola k("1710034065\nAna\nPerez\n"); c.registrarProfesor(); }
    Controlador otra = crear();
    CHECK(buscarProfesor(otra, "1710034065") != nullptr);
}

// =============== registrarMateria ===============
TEST(CtrlMateria_registrar_ok_idEnMayusculas) {
    ArchivosTemporales tmp;
    Controlador c = crear();
    Consola k("calc101\nCalculo 1\n4\n");
    c.registrarMateria();
    const Materia* m = buscarMateria(c, "CALC101");
    CHECK(m != nullptr);
    if (m) { CHECK_EQ(m->getNombre(), "Calculo 1"); CHECK_EQ(m->getCreditos(), 4); }
    CHECK_EQ(ArchivosTemporales::leer("materias.txt"), "CALC101,Calculo 1,4\n");
}

TEST(CtrlMateria_registrar_idInvalidoODuplicado_repite) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    Consola k("abc\nALGE101\nFISI101\nFisica\n3\n");
    c.registrarMateria();
    CHECK_EQ(c.getMaterias().getTamanio(), 4u);
    CHECK_CONTAINS(k.texto(), "el codigo debe tener 4 a 10");
    CHECK_CONTAINS(k.texto(), "ya existe una materia con ese codigo");
}

TEST(CtrlMateria_registrar_nombreDuplicadoSinImportarMayusculas) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    Consola k("FISI101\nalgebra lineal\nFisica\n3\n");
    c.registrarMateria();
    CHECK_EQ(c.getMaterias().getTamanio(), 4u);
    CHECK_CONTAINS(k.texto(), "ya existe una materia con ese nombre");
}

TEST(CtrlMateria_registrar_nombreConComa_seRechaza) {
    ArchivosTemporales tmp;
    Controlador c = crear();
    Consola k("FISI101\nFisica, general\nFisica\n3\n");
    c.registrarMateria();
    CHECK_CONTAINS(k.texto(), "el nombre debe tener 3 a 60");
    CHECK_EQ(buscarMateria(c, "FISI101")->getNombre(), "Fisica");
}

TEST(CtrlMateria_registrar_creditosFueraDeRango_repite) {
    ArchivosTemporales tmp;
    Controlador c = crear();
    Consola k("FISI101\nFisica\n0\n11\nabc\n10\n");
    c.registrarMateria();
    CHECK_EQ(buscarMateria(c, "FISI101")->getCreditos(), 10);
}

TEST(CtrlMateria_registrar_cancelar) {
    ArchivosTemporales tmp;
    Controlador c = crear();
    { Consola k("0\n"); c.registrarMateria(); }
    { Consola k("FISI101\n0\n"); c.registrarMateria(); }
    CHECK(c.getMaterias().estaVacia());
    CHECK_FALSE(ArchivosTemporales::existe("materias.txt"));
}

// =============== crearCursoConNrc ===============
TEST(CtrlCurso_crear_sinMateriasOProfesores_noPermite) {
    ArchivosTemporales tmp;
    Controlador c = crear();
    Consola k;
    c.crearCursoConNrc();
    CHECK_CONTAINS(k.texto(), "necesita tener al menos una materia y un profesor");
    CHECK(c.getCursos().estaVacia());
}

TEST(CtrlCurso_crear_primerCurso_generaNrc10001_yGuarda) {
    ArchivosTemporales tmp;
    datosSinCursos();
    Controlador c = crear();
    Consola k("1\n1\n1\n1\ns\n");    // materia 1, profesor 1, Lunes, franja 1, confirmar
    c.crearCursoConNrc();
    CHECK_EQ(c.getCursos().getTamanio(), 1u);
    const Aula* a = buscarCurso(c, "10001");
    CHECK(a != nullptr);
    if (a) {
        CHECK_EQ(a->getIdMateria(), "ALGE101");
        CHECK_EQ(a->getCedulaProfesor(), "0605293885");
        CHECK_EQ(a->getDia(), LUNES);
        CHECK_EQ(a->getHorario().toString(), "07:00 - 09:00");
    }
    CHECK_EQ(ArchivosTemporales::leer("cursos.txt"), "10001,ALGE101,0605293885,LUNES,420,540\n");
    CHECK_CONTAINS(k.texto(), "NRC generado : 10001");
    CHECK_CONTAINS(k.texto(), "Curso creado correctamente con NRC 10001");
}

TEST(CtrlCurso_crear_noPideNiMuestraPeriodoNiParalelo) {
    ArchivosTemporales tmp;
    datosSinCursos();
    Controlador c = crear();
    Consola k("1\n1\n1\n1\ns\n");
    c.crearCursoConNrc();
    CHECK_NOT_CONTAINS(k.texto(), "eriodo");
    CHECK_NOT_CONTAINS(k.texto(), "ERIODO");
    CHECK_NOT_CONTAINS(k.texto(), "aralelo");
    CHECK_NOT_CONTAINS(k.texto(), "ARALELO");
}

TEST(CtrlCurso_crear_nrcContinuaLaSecuencia) {
    ArchivosTemporales tmp;
    datosBasicos();                   // ya existen 10001..10003
    Controlador c = crear();
    Consola k("2\n2\n3\n3\ns\n");    // CALC101, profesor 2, Miercoles, 11:00-13:00
    c.crearCursoConNrc();
    CHECK_EQ(c.getCursos().getTamanio(), 4u);
    const Aula* a = buscarCurso(c, "10004");
    CHECK(a != nullptr);
    if (a) {
        CHECK_EQ(a->getDia(), MIERCOLES);
        CHECK_EQ(a->getHorario().toString(), "11:00 - 13:00");
        CHECK_EQ(a->getCedulaProfesor(), "1710034065");
    }
}

TEST(CtrlCurso_crear_mismaMateriaVariosNrc_sinParalelos) {
    ArchivosTemporales tmp;
    datosSinCursos();
    Controlador c = crear();
    { Consola k("1\n1\n1\n1\ns\n"); c.crearCursoConNrc(); }   // ALGE101, prof 1, Lun 07-09
    { Consola k("1\n2\n1\n1\ns\n"); c.crearCursoConNrc(); }   // ALGE101 otra vez, prof 2, Lun 07-09
    CHECK_EQ(c.getCursos().getTamanio(), 2u);
    CHECK(buscarCurso(c, "10001") != nullptr);
    CHECK(buscarCurso(c, "10002") != nullptr);
    CHECK_EQ(c.getCursos().contar([](const Aula& a) { return a.getIdMateria() == "ALGE101"; }), 2u);
}

TEST(CtrlCurso_crear_choqueDeHorarioDelProfesor_pideOtro) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    // profesor 1 ya tiene Lunes 07-09 (10001): primero intenta ese horario, luego Lunes 11-13
    Consola k("1\n1\n1\n1\n1\n3\ns\n");
    c.crearCursoConNrc();
    CHECK_CONTAINS(k.texto(), "el profesor ya tiene el NRC 10001");
    const Aula* a = buscarCurso(c, "10004");
    CHECK(a != nullptr);
    if (a) CHECK_EQ(a->getHorario().toString(), "11:00 - 13:00");
}

TEST(CtrlCurso_crear_otroProfesorMismoHorario_noEsChoque) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    Consola k("1\n2\n1\n1\ns\n");    // profesor 2, Lunes 07-09 (profesor 1 ya lo usa)
    c.crearCursoConNrc();
    CHECK_NOT_CONTAINS(k.texto(), "Error");
    CHECK_EQ(c.getCursos().getTamanio(), 4u);
}

TEST(CtrlCurso_crear_cancelarEnConfirmacion_noGuarda) {
    ArchivosTemporales tmp;
    datosSinCursos();
    Controlador c = crear();
    Consola k("1\n1\n1\n1\nn\n");
    c.crearCursoConNrc();
    CHECK(c.getCursos().estaVacia());
    CHECK_EQ(ArchivosTemporales::leer("cursos.txt"), "");
    CHECK_CONTAINS(k.texto(), "Operacion cancelada");
}

TEST(CtrlCurso_crear_cancelarEnCadaPaso) {
    ArchivosTemporales tmp;
    datosSinCursos();
    Controlador c = crear();
    { Consola k("0\n"); c.crearCursoConNrc(); }                // materia
    { Consola k("1\n0\n"); c.crearCursoConNrc(); }             // profesor
    { Consola k("1\n1\n0\n"); c.crearCursoConNrc(); }          // dia
    { Consola k("1\n1\n1\n0\n"); c.crearCursoConNrc(); }       // franja
    CHECK(c.getCursos().estaVacia());
}

TEST(CtrlCurso_crear_nrcAgotado_muestraError) {
    ArchivosTemporales tmp;
    datosSinCursos();
    ArchivosTemporales::escribir("cursos.txt", "99999,ALGE101,0605293885,LUNES,420,540\n");
    Controlador c = crear();
    Consola k("1\n2\n1\n1\n");
    c.crearCursoConNrc();
    CHECK_CONTAINS(k.texto(), "se agotaron los NRC");
    CHECK_EQ(c.getCursos().getTamanio(), 1u);
}

// =============== agregarHorario / cambiarHorarioCurso ===============
TEST(CtrlHorario_agregarHorario_ok_actualizaYGuarda) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    CHECK(c.agregarHorario("10003", VIERNES, IntervaloHorario::franja(7)));
    const Aula* a = buscarCurso(c, "10003");
    CHECK_EQ(a->getDia(), VIERNES);
    CHECK_EQ(a->getHorario().toString(), "19:00 - 21:00");
    CHECK_CONTAINS(ArchivosTemporales::leer("cursos.txt"), "10003,CALC101,1710034065,VIERNES,1140,1260\n");
}

TEST(CtrlHorario_agregarHorario_choqueMismoProfesor_rechaza) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    CHECK_FALSE(c.agregarHorario("10002", LUNES, IntervaloHorario::franja(1)));   // choca con 10001
    CHECK_EQ(buscarCurso(c, "10002")->getHorario().toString(), "09:00 - 11:00");  // sin cambios
}

TEST(CtrlHorario_agregarHorario_noChocaConsigoMismo) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    CHECK(c.agregarHorario("10001", LUNES, IntervaloHorario::franja(1)));
}

TEST(CtrlHorario_agregarHorario_otroProfesorNoEsChoque) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    CHECK(c.agregarHorario("10003", LUNES, IntervaloHorario::franja(1)));
}

TEST(CtrlHorario_agregarHorario_nrcInexistenteOFranjaNoOficial) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    CHECK_FALSE(c.agregarHorario("99999", LUNES, IntervaloHorario::franja(1)));
    CHECK_FALSE(c.agregarHorario("10003", LUNES, IntervaloHorario(425, 545)));
}

TEST(CtrlHorario_cambiarHorarioCurso_interactivo) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    Consola k("3\n3\n2\n");          // curso #3 (10003) -> Miercoles, 09:00-11:00
    c.cambiarHorarioCurso();
    const Aula* a = buscarCurso(c, "10003");
    CHECK_EQ(a->getDia(), MIERCOLES);
    CHECK_EQ(a->getHorario().toString(), "09:00 - 11:00");
    CHECK_CONTAINS(k.texto(), "actualizado");
}

TEST(CtrlHorario_cambiarHorarioCurso_choque_pideOtroHorario) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    Consola k("2\n1\n1\n2\n5\n");    // 10002: Lunes 07-09 (choca) -> Martes 15-17
    c.cambiarHorarioCurso();
    CHECK_CONTAINS(k.texto(), "el profesor ya tiene el NRC 10001");
    CHECK_EQ(buscarCurso(c, "10002")->getDia(), MARTES);
    CHECK_EQ(buscarCurso(c, "10002")->getHorario().toString(), "15:00 - 17:00");
}

TEST(CtrlHorario_cambiarHorarioCurso_cancelar_noCambiaNada) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    { Consola k("0\n"); c.cambiarHorarioCurso(); }
    { Consola k("1\n0\n"); c.cambiarHorarioCurso(); }
    { Consola k("1\n2\n0\n"); c.cambiarHorarioCurso(); }
    CHECK_EQ(buscarCurso(c, "10001")->getDia(), LUNES);
    CHECK_EQ(buscarCurso(c, "10001")->getHorario().toString(), "07:00 - 09:00");
}

TEST(CtrlHorario_cambiarHorarioCurso_sinCursos) {
    ArchivosTemporales tmp;
    Controlador c = crear();
    Consola k;
    c.cambiarHorarioCurso();
    CHECK_CONTAINS(k.texto(), "No hay cursos registrados.");
}

// =============== eliminarCurso ===============
TEST(CtrlEliminarCurso_ok_actualizaArchivo) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    Consola k("3\ns\n");
    c.eliminarCurso();
    CHECK_EQ(c.getCursos().getTamanio(), 2u);
    CHECK(buscarCurso(c, "10003") == nullptr);
    CHECK_NOT_CONTAINS(ArchivosTemporales::leer("cursos.txt"), "10003");
    CHECK_CONTAINS(k.texto(), "Curso 10003 eliminado");
}

TEST(CtrlEliminarCurso_conAsistencias_noSePuedeEliminar) {
    ArchivosTemporales tmp;
    datosBasicos();
    ArchivosTemporales::escribir("asistencias.txt", "2026-11-02,07:05,0605293885,10001,ENTRADA,PUNTUAL\n");
    Controlador c = crear();
    Consola k("1\n");
    c.eliminarCurso();
    CHECK_EQ(c.getCursos().getTamanio(), 3u);
    CHECK_CONTAINS(k.texto(), "tiene asistencias registradas");
}

TEST(CtrlEliminarCurso_cancelarConfirmacionOSeleccion) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    { Consola k("1\nn\n"); c.eliminarCurso(); }
    { Consola k("0\n"); c.eliminarCurso(); }
    CHECK_EQ(c.getCursos().getTamanio(), 3u);
}

TEST(CtrlEliminarCurso_sinCursos) {
    ArchivosTemporales tmp;
    Controlador c = crear();
    Consola k;
    c.eliminarCurso();
    CHECK_CONTAINS(k.texto(), "No hay cursos registrados.");
}

// =============== eliminarProfesor ===============
TEST(CtrlEliminarProfesor_conCursos_noSePuedeEliminar) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    Consola k("1\n");
    c.eliminarProfesor();
    CHECK_EQ(c.getProfesores().getTamanio(), 2u);
    CHECK_CONTAINS(k.texto(), "tiene 2 curso(s) asignado(s)");
}

TEST(CtrlEliminarProfesor_sinCursos_seElimina) {
    ArchivosTemporales tmp;
    datosBasicos();
    ArchivosTemporales::escribir("profesores.txt",
        "0605293885,Adrian Michael,Toa Orejuela\n1710034065,Ana,Almeida\n3000000012,Luis,Mora\n");
    Controlador c = crear();
    Consola k("3\ns\n");
    c.eliminarProfesor();
    CHECK_EQ(c.getProfesores().getTamanio(), 2u);
    CHECK(buscarProfesor(c, "3000000012") == nullptr);
    CHECK_NOT_CONTAINS(ArchivosTemporales::leer("profesores.txt"), "3000000012");
}

TEST(CtrlEliminarProfesor_cancelarOSinProfesores) {
    ArchivosTemporales tmp;
    {
        Controlador vacio = crear();
        Consola k;
        vacio.eliminarProfesor();
        CHECK_CONTAINS(k.texto(), "No hay profesores registrados.");
    }
    datosBasicos();
    ArchivosTemporales::escribir("cursos.txt", "");
    Controlador c = crear();
    { Consola k("1\nn\n"); c.eliminarProfesor(); }
    { Consola k("0\n"); c.eliminarProfesor(); }
    CHECK_EQ(c.getProfesores().getTamanio(), 2u);
}

// =============== listados ===============
TEST(CtrlListados_profesores_ordenadosPorApellido_sinAlterarOriginal) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    Consola k;
    c.listarProfesores();
    std::string s = k.texto();
    CHECK_CONTAINS(s, "PROFESORES (2)");
    CHECK(s.find("Almeida") < s.find("Toa Orejuela"));
    CHECK_EQ(c.getProfesores().getCabeza()->getDato().getCedula(), "0605293885");   // la lista original conserva su orden
}

TEST(CtrlListados_materias_ordenadasPorCodigo) {
    ArchivosTemporales tmp;
    ArchivosTemporales::escribir("materias.txt", "EDIF201,Ecuaciones Diferenciales,4\nALGE101,Algebra Lineal,4\n");
    Controlador c = crear();
    Consola k;
    c.listarMaterias();
    CHECK(k.texto().find("ALGE101") < k.texto().find("EDIF201"));
}

TEST(CtrlListados_cursos_ordenadosPorNrc_sinColumnasDePeriodoNiParalelo) {
    ArchivosTemporales tmp;
    datosBasicos();
    ArchivosTemporales::escribir("cursos.txt",
        "10003,CALC101,1710034065,MARTES,420,540\n10001,ALGE101,0605293885,LUNES,420,540\n");
    Controlador c = crear();
    Consola k;
    c.listarCursos();
    std::string s = k.texto();
    CHECK_CONTAINS(s, "CURSOS / NRC (2)");
    CHECK(s.find("10001") < s.find("10003"));
    CHECK_CONTAINS(s, "Algebra Lineal");
    CHECK_CONTAINS(s, "Almeida Ana");
    CHECK_NOT_CONTAINS(s, "Periodo");
    CHECK_NOT_CONTAINS(s, "Par ");
}

TEST(CtrlListados_vacios_muestranMensaje) {
    ArchivosTemporales tmp;
    Controlador c = crear();
    Consola k;
    c.listarProfesores(); c.listarMaterias(); c.listarCursos();
    CHECK_CONTAINS(k.texto(), "No hay profesores registrados.");
    CHECK_CONTAINS(k.texto(), "No hay materias registradas.");
    CHECK_CONTAINS(k.texto(), "No hay cursos registrados.");
}
