#include "mini_test.h"
#include "fixture.h"

// Marca un ingreso/salida simulando teclado y reloj. Devuelve lo que se imprimio.
static std::string ingreso(Controlador& c, const std::string& fecha, int h, int m, const std::string& teclas) {
    RelojFijo reloj(fecha, h, m);
    Consola k(teclas);
    c.loginProfesor();
    return k.texto();
}
static std::string salida(Controlador& c, const std::string& fecha, int h, int m, const std::string& teclas) {
    RelojFijo reloj(fecha, h, m);
    Consola k(teclas);
    c.logoutProfesor();
    return k.texto();
}
static const std::string P1 = "0605293885\n";   // profesor 1: NRC 10001 (opcion 1) y 10002 (opcion 2)

// =============== INGRESO ===============
TEST(CtrlIngreso_puntual_registraYGuarda) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    std::string s = ingreso(c, LUNES_FECHA, 7, 5, P1 + "1\n");
    CHECK_EQ(c.getRegistros().getTamanio(), 1u);
    const RegistroAsistencia& r = c.getRegistros().getCabeza()->getDato();
    CHECK_EQ(r.getFecha(), "2026-11-02");
    CHECK_EQ(r.getHora(), "07:05");
    CHECK_EQ(r.getNrc(), "10001");
    CHECK_EQ(r.getCedula(), "0605293885");
    CHECK_EQ(r.getTipo(), "ENTRADA");
    CHECK_EQ(r.getEstado(), "PUNTUAL");
    CHECK_EQ(ArchivosTemporales::leer("asistencias.txt"), "2026-11-02,07:05,0605293885,10001,ENTRADA,PUNTUAL\n");
    CHECK_CONTAINS(s, "Estado: PUNTUAL");
}

TEST(CtrlIngreso_limitePuntualYAtraso) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    ingreso(c, LUNES_FECHA, 7, 0, P1 + "1\n");      // 07:00 exacto -> puntual
    CHECK_EQ(c.getRegistros().getCola()->getDato().getEstado(), "PUNTUAL");

    ArchivosTemporales::borrar();
    datosBasicos();
    Controlador d = crear();
    ingreso(d, LUNES_FECHA, 7, 10, P1 + "1\n");     // 07:10 -> aun puntual
    CHECK_EQ(d.getRegistros().getCola()->getDato().getEstado(), "PUNTUAL");

    ArchivosTemporales::borrar();
    datosBasicos();
    Controlador e = crear();
    std::string s = ingreso(e, LUNES_FECHA, 7, 11, P1 + "1\n");   // 07:11 -> atraso
    CHECK_EQ(e.getRegistros().getCola()->getDato().getEstado(), "ATRASO");
    CHECK_CONTAINS(s, "ATRASO");
}

TEST(CtrlIngreso_antesDeLaHora_rechaza) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    std::string s = ingreso(c, LUNES_FECHA, 6, 59, P1 + "1\n");
    CHECK(c.getRegistros().estaVacia());
    CHECK_CONTAINS(s, "aun no es hora");
    CHECK_CONTAINS(s, "07:00");
}

TEST(CtrlIngreso_ultimoMinutoPermitidoYPlazoVencido) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    std::string tarde = ingreso(c, LUNES_FECHA, 8, 50, P1 + "1\n");   // 08:50 = ya en ventana de salida
    CHECK(c.getRegistros().estaVacia());
    CHECK_CONTAINS(tarde, "el plazo para registrar ingreso termino a las 08:49");
    ingreso(c, LUNES_FECHA, 8, 49, P1 + "1\n");                       // 08:49 -> ultimo minuto
    CHECK_EQ(c.getRegistros().getTamanio(), 1u);
    CHECK_EQ(c.getRegistros().getCabeza()->getDato().getEstado(), "ATRASO");
}

TEST(CtrlIngreso_duplicadoElMismoDia_rechaza) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    ingreso(c, LUNES_FECHA, 7, 5, P1 + "1\n");
    std::string s = ingreso(c, LUNES_FECHA, 7, 6, P1 + "1\n");
    CHECK_EQ(c.getRegistros().getTamanio(), 1u);
    CHECK_CONTAINS(s, "ya registro su ingreso hoy en el NRC 10001 a las 07:05");
}

TEST(CtrlIngreso_otroCursoDelMismoProfesor_esIndependiente) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    ingreso(c, LUNES_FECHA, 7, 5, P1 + "1\n");
    ingreso(c, LUNES_FECHA, 9, 3, P1 + "2\n");       // NRC 10002 (09:00-11:00)
    CHECK_EQ(c.getRegistros().getTamanio(), 2u);
    CHECK_EQ(c.getRegistros().getCola()->getDato().getNrc(), "10002");
}

TEST(CtrlIngreso_cursoDeOtroHorario_noEsHora) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    std::string s = ingreso(c, LUNES_FECHA, 7, 5, P1 + "2\n");       // 10002 empieza 09:00
    CHECK(c.getRegistros().estaVacia());
    CHECK_CONTAINS(s, "aun no es hora");
}

TEST(CtrlIngreso_diaIncorrecto_rechaza) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    std::string s = ingreso(c, MARTES_FECHA, 7, 5, P1 + "1\n");      // 10001 es de LUNES
    CHECK(c.getRegistros().estaVacia());
    CHECK_CONTAINS(s, "se dicta los LUNES y hoy es MARTES");
}

TEST(CtrlIngreso_finDeSemana_rechaza) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    std::string s = ingreso(c, SABADO_FECHA, 7, 5, P1 + "1\n");
    CHECK(c.getRegistros().estaVacia());
    CHECK_CONTAINS(s, "fin de semana");
}

TEST(CtrlIngreso_otroProfesor_soloVeSusCursos) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    std::string s = ingreso(c, MARTES_FECHA, 7, 5, "1710034065\n1\n");
    CHECK_EQ(c.getRegistros().getTamanio(), 1u);
    CHECK_EQ(c.getRegistros().getCabeza()->getDato().getNrc(), "10003");
    CHECK_NOT_CONTAINS(s, "10001");
    CHECK_NOT_CONTAINS(s, "10002");
}

TEST(CtrlIngreso_identificacion_cedulaInvalidaNoRegistradaOCancelada) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    CHECK_CONTAINS(ingreso(c, LUNES_FECHA, 7, 5, "123\n"), "la cedula ingresada no es valida");
    CHECK_CONTAINS(ingreso(c, LUNES_FECHA, 7, 5, "1701234567\n"), "Acceso denegado");
    ingreso(c, LUNES_FECHA, 7, 5, "0\n");
    ingreso(c, LUNES_FECHA, 7, 5, P1 + "0\n");       // cancela en la seleccion de curso
    CHECK(c.getRegistros().estaVacia());
}

TEST(CtrlIngreso_profesorSinCursos) {
    ArchivosTemporales tmp;
    datosBasicos();
    ArchivosTemporales::escribir("profesores.txt",
        "0605293885,Adrian Michael,Toa Orejuela\n1710034065,Ana,Almeida\n3000000012,Luis,Mora\n");
    Controlador c = crear();
    std::string s = ingreso(c, LUNES_FECHA, 7, 5, "3000000012\n");
    CHECK_CONTAINS(s, "No tiene cursos asignados.");
    CHECK(c.getRegistros().estaVacia());
}

TEST(CtrlIngreso_muestraRelojAlInicio) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    std::string s = ingreso(c, LUNES_FECHA, 7, 5, "0\n");
    CHECK_CONTAINS(s, "2026-11-02 07:05 (LUNES)");
}

TEST(CtrlIngreso_persisteEntreInstancias) {
    ArchivosTemporales tmp;
    datosBasicos();
    { Controlador c = crear(); ingreso(c, LUNES_FECHA, 7, 5, P1 + "1\n"); }
    Controlador otra = crear();
    CHECK_EQ(otra.getRegistros().getTamanio(), 1u);
    std::string s = ingreso(otra, LUNES_FECHA, 7, 20, P1 + "1\n");    // sigue sin dejar duplicar
    CHECK_CONTAINS(s, "ya registro su ingreso");
}

// =============== SALIDA ===============
TEST(CtrlSalida_sinIngresoPrevio_rechaza) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    std::string s = salida(c, LUNES_FECHA, 8, 55, P1 + "1\n");
    CHECK(c.getRegistros().estaVacia());
    CHECK_CONTAINS(s, "primero debe registrar su ingreso");
}

TEST(CtrlSalida_ok_guardaRegistro) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    ingreso(c, LUNES_FECHA, 7, 5, P1 + "1\n");
    std::string s = salida(c, LUNES_FECHA, 8, 55, P1 + "1\n");
    CHECK_EQ(c.getRegistros().getTamanio(), 2u);
    const RegistroAsistencia& r = c.getRegistros().getCola()->getDato();
    CHECK_EQ(r.getTipo(), "SALIDA");
    CHECK_EQ(r.getEstado(), "REGISTRADA");
    CHECK_EQ(r.getHora(), "08:55");
    CHECK_CONTAINS(s, "Salida registrada a las 08:55");
    CHECK_CONTAINS(ArchivosTemporales::leer("asistencias.txt"), "2026-11-02,08:55,0605293885,10001,SALIDA,REGISTRADA");
}

TEST(CtrlSalida_ventanaDeLosUltimos10Minutos) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    ingreso(c, LUNES_FECHA, 7, 5, P1 + "1\n");
    std::string temprano = salida(c, LUNES_FECHA, 8, 49, P1 + "1\n");
    CHECK_CONTAINS(temprano, "la salida se registra entre 08:50 y 08:59");
    std::string tardia = salida(c, LUNES_FECHA, 9, 0, P1 + "1\n");
    CHECK_CONTAINS(tardia, "la salida se registra entre 08:50 y 08:59");
    CHECK_EQ(c.getRegistros().getTamanio(), 1u);
    salida(c, LUNES_FECHA, 8, 50, P1 + "1\n");        // limite inferior valido
    CHECK_EQ(c.getRegistros().getTamanio(), 2u);
}

TEST(CtrlSalida_ultimoMinutoValido) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    ingreso(c, LUNES_FECHA, 7, 5, P1 + "1\n");
    salida(c, LUNES_FECHA, 8, 59, P1 + "1\n");
    CHECK_EQ(c.getRegistros().getTamanio(), 2u);
}

TEST(CtrlSalida_duplicada_rechaza) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    ingreso(c, LUNES_FECHA, 7, 5, P1 + "1\n");
    salida(c, LUNES_FECHA, 8, 55, P1 + "1\n");
    std::string s = salida(c, LUNES_FECHA, 8, 56, P1 + "1\n");
    CHECK_EQ(c.getRegistros().getTamanio(), 2u);
    CHECK_CONTAINS(s, "ya registro su salida hoy en el NRC 10001");
}

TEST(CtrlSalida_diaIncorrectoYFinDeSemana) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    CHECK_CONTAINS(salida(c, MARTES_FECHA, 8, 55, P1 + "1\n"), "se dicta los LUNES");
    CHECK_CONTAINS(salida(c, SABADO_FECHA, 8, 55, P1 + "1\n"), "fin de semana");
}

TEST(CtrlSalida_ingresoDeUnDiaNoValeParaOtro) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    ingreso(c, LUNES_FECHA, 7, 5, P1 + "1\n");
    std::string s = salida(c, "2026-11-09", 8, 55, P1 + "1\n");        // lunes siguiente
    CHECK_CONTAINS(s, "primero debe registrar su ingreso");
}

// =============== MIS CURSOS ===============
TEST(CtrlMisCursos_listaSusCursos_sinPeriodo) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    Consola k(P1);
    c.verMisCursos();
    std::string s = k.texto();
    CHECK_CONTAINS(s, "NRC 10001 | Algebra Lineal | LUNES 07:00 - 09:00");
    CHECK_CONTAINS(s, "NRC 10002 | Ecuaciones Diferenciales | LUNES 09:00 - 11:00");
    CHECK_NOT_CONTAINS(s, "10003");
    CHECK_NOT_CONTAINS(s, "periodo");
    CHECK_NOT_CONTAINS(s, "inactivo");
}

TEST(CtrlMisCursos_ordenadosPorDiaYHora) {
    ArchivosTemporales tmp;
    datosBasicos();
    ArchivosTemporales::escribir("cursos.txt",
        "10001,ALGE101,0605293885,VIERNES,420,540\n"
        "10002,EDIF201,0605293885,LUNES,660,780\n"
        "10004,CALC101,0605293885,LUNES,420,540\n");
    Controlador c = crear();
    Consola k(P1);
    c.verMisCursos();
    std::string s = k.texto();
    CHECK(s.find("10004") < s.find("10002"));      // Lunes 07:00 antes que Lunes 11:00
    CHECK(s.find("10002") < s.find("10001"));      // Lunes antes que Viernes
}

TEST(CtrlMisCursos_sinCursosOCancelado) {
    ArchivosTemporales tmp;
    datosBasicos();
    ArchivosTemporales::escribir("profesores.txt",
        "0605293885,Adrian Michael,Toa Orejuela\n1710034065,Ana,Almeida\n3000000012,Luis,Mora\n");
    Controlador c = crear();
    { Consola k("3000000012\n"); c.verMisCursos(); CHECK_CONTAINS(k.texto(), "No tiene cursos asignados."); }
    { Consola k("0\n"); c.verMisCursos(); CHECK_NOT_CONTAINS(k.texto(), "NRC"); }
    { Consola k("1701234567\n"); c.verMisCursos(); CHECK_CONTAINS(k.texto(), "Acceso denegado"); }
}

// =============== REPORTE DE ASISTENCIAS ===============
static void datosReporte() {
    datosBasicos();
    ArchivosTemporales::escribir("cursos.txt",
        "10001,ALGE101,0605293885,LUNES,420,540\n"    // prof 1, Lun 07-09: entrada puntual + salida
        "10002,EDIF201,0605293885,LUNES,540,660\n"    // prof 1, Lun 09-11: entrada con atraso, sin salida
        "10003,CALC101,1710034065,MARTES,420,540\n"   // martes (no cuenta los lunes)
        "10004,CALC101,1710034065,LUNES,420,540\n"    // prof 2, Lun 07-09: sin registro -> AUSENTE
        "10005,ALGE101,1710034065,LUNES,660,780\n");  // prof 2, Lun 11-13: aun no empieza -> PENDIENTE
    ArchivosTemporales::escribir("asistencias.txt",
        "2026-11-02,07:05,0605293885,10001,ENTRADA,PUNTUAL\n"
        "2026-11-02,08:55,0605293885,10001,SALIDA,REGISTRADA\n"
        "2026-11-02,09:20,0605293885,10002,ENTRADA,ATRASO\n");
}

TEST(CtrlReporte_fechaDeHoy_clasificaTodosLosEstados) {
    ArchivosTemporales tmp;
    datosReporte();
    Controlador c = crear();
    RelojFijo reloj(LUNES_FECHA, 10, 0);
    Consola k("\n");                                   // Enter = fecha de hoy
    c.reporteAsistencias();
    std::string s = k.texto();
    CHECK_CONTAINS(s, "Clases del LUNES 2026-11-02");
    CHECK_CONTAINS(s, "Resumen: 4 clase(s) | 1 puntual(es) | 1 atraso(s) | 1 ausente(s) | 1 pendiente(s)");
    CHECK_CONTAINS(s, "ATRASO (sin salida)");
    CHECK_CONTAINS(s, "AUSENTE");
    CHECK_CONTAINS(s, "PENDIENTE");
    CHECK_NOT_CONTAINS(s, "10003");                    // clase del martes
    CHECK_NOT_CONTAINS(s, "periodos activos");
}

TEST(CtrlReporte_ordenadoPorHoraYNrc) {
    ArchivosTemporales tmp;
    datosReporte();
    Controlador c = crear();
    RelojFijo reloj(LUNES_FECHA, 10, 0);
    Consola k("\n");
    c.reporteAsistencias();
    std::string s = k.texto();
    size_t p1 = s.find("10001"), p4 = s.find("10004"), p2 = s.find("10002"), p5 = s.find("10005");
    CHECK(p1 < p4);      // misma hora: por NRC
    CHECK(p4 < p2);      // 07:00 antes que 09:00
    CHECK(p2 < p5);
}

TEST(CtrlReporte_fechaPasada_sinRegistroEsAusente) {
    ArchivosTemporales tmp;
    datosReporte();
    Controlador c = crear();
    RelojFijo reloj("2026-11-04", 10, 0);              // hoy es miercoles
    Consola k("2026-11-02\n");
    c.reporteAsistencias();
    CHECK_CONTAINS(k.texto(), "Resumen: 4 clase(s) | 1 puntual(es) | 1 atraso(s) | 2 ausente(s) | 0 pendiente(s)");
}

TEST(CtrlReporte_fechaFutura_todoPendiente) {
    ArchivosTemporales tmp;
    datosReporte();
    Controlador c = crear();
    RelojFijo reloj(LUNES_FECHA, 10, 0);
    Consola k("2026-11-09\n");
    c.reporteAsistencias();
    CHECK_CONTAINS(k.texto(), "Resumen: 4 clase(s) | 0 puntual(es) | 0 atraso(s) | 0 ausente(s) | 4 pendiente(s)");
}

TEST(CtrlReporte_limiteDeFinDeClase_pendienteYAusente) {
    ArchivosTemporales tmp;
    datosBasicos();
    ArchivosTemporales::escribir("cursos.txt", "10001,ALGE101,0605293885,LUNES,420,540\n");
    Controlador c = crear();
    {
        RelojFijo reloj(LUNES_FECHA, 8, 59);
        Consola k("\n");
        c.reporteAsistencias();
        CHECK_CONTAINS(k.texto(), "1 pendiente(s)");
    }
    {
        RelojFijo reloj(LUNES_FECHA, 9, 0);
        Consola k("\n");
        c.reporteAsistencias();
        CHECK_CONTAINS(k.texto(), "1 ausente(s)");
    }
}

TEST(CtrlReporte_finDeSemanaYDiaSinClases) {
    ArchivosTemporales tmp;
    datosReporte();
    Controlador c = crear();
    RelojFijo reloj(LUNES_FECHA, 10, 0);
    { Consola k("2026-11-07\n"); c.reporteAsistencias(); CHECK_CONTAINS(k.texto(), "cae en fin de semana"); }
    { Consola k("2026-11-04\n"); c.reporteAsistencias(); CHECK_CONTAINS(k.texto(), "No hay clases programadas para ese dia."); }
}

TEST(CtrlReporte_fechaInvalida_repiteYCancelar) {
    ArchivosTemporales tmp;
    datosReporte();
    Controlador c = crear();
    RelojFijo reloj(LUNES_FECHA, 10, 0);
    Consola k("2026-13-45\nhola\n0\n");
    c.reporteAsistencias();
    CHECK_CONTAINS(k.texto(), "Error: fecha invalida");
    CHECK_NOT_CONTAINS(k.texto(), "Resumen");
}

// =============== MENUS ===============
TEST(CtrlMenuAdmin_opcionesSimplificadas) {
    ArchivosTemporales tmp;
    Controlador c = crear();
    Consola k("9\n");
    c.menuAdmin();
    std::string s = k.texto();
    CHECK_CONTAINS(s, "1. Registrar profesor");
    CHECK_CONTAINS(s, "2. Registrar materia");
    CHECK_CONTAINS(s, "3. Crear curso (genera el NRC automaticamente)");
    CHECK_CONTAINS(s, "4. Cambiar horario de un curso");
    CHECK_CONTAINS(s, "5. Eliminar curso");
    CHECK_CONTAINS(s, "6. Eliminar profesor");
    CHECK_CONTAINS(s, "7. Listados");
    CHECK_CONTAINS(s, "8. Reporte de asistencias por fecha");
    CHECK_CONTAINS(s, "9. Volver al menu principal");
}

TEST(CtrlMenuAdmin_noTieneOpcionesEliminadas) {
    ArchivosTemporales tmp;
    Controlador c = crear();
    Consola k("9\n");
    c.menuAdmin();
    std::string s = k.texto();
    CHECK_NOT_CONTAINS(s, "eriodo");
    CHECK_NOT_CONTAINS(s, "imular");
    CHECK_NOT_CONTAINS(s, "aralelo");
    CHECK_NOT_CONTAINS(s, "10.");
    CHECK_NOT_CONTAINS(s, "11.");
    CHECK_NOT_CONTAINS(s, "12.");
}

TEST(CtrlMenuAdmin_opcionFueraDeRango_repite) {
    ArchivosTemporales tmp;
    Controlador c = crear();
    Consola k("10\n0\nabc\n9\n");
    c.menuAdmin();
    CHECK_CONTAINS(k.texto(), "Error: el valor debe estar entre 1 y 9");
}

TEST(CtrlMenuAdmin_cadaOpcionLlamaASuFuncion) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    // 1 registrar profesor | 2 registrar materia | 3 crear curso | 4 cambiar horario | 5 eliminar curso
    // 6 eliminar profesor | 7 listados -> 4 (volver) | 8 reporte (cancelar) | 9 salir
    Consola k("1\n0\n" "2\n0\n" "3\n0\n" "4\n0\n" "5\n0\n" "6\n0\n" "7\n4\n" "8\n0\n" "9\n");
    c.menuAdmin();
    std::string s = k.texto();
    CHECK_CONTAINS(s, "REGISTRAR NUEVO PROFESOR");
    CHECK_CONTAINS(s, "REGISTRAR MATERIA");
    CHECK_CONTAINS(s, "CREAR CURSO Y GENERAR NRC");
    CHECK_CONTAINS(s, "CAMBIAR HORARIO DE UN CURSO");
    CHECK_CONTAINS(s, "ELIMINAR CURSO");
    CHECK_CONTAINS(s, "ELIMINAR PROFESOR");
    CHECK_CONTAINS(s, "=== LISTADOS ===");
    CHECK_CONTAINS(s, "REPORTE DE ASISTENCIAS POR FECHA");
}

TEST(CtrlMenuAdmin_crearCursoDesdeElMenu) {
    ArchivosTemporales tmp;
    datosSinCursos();
    Controlador c = crear();
    Consola k("3\n1\n1\n1\n1\ns\n9\n");
    c.menuAdmin();
    CHECK_EQ(c.getCursos().getTamanio(), 1u);
    CHECK_CONTAINS(k.texto(), "NRC 10001");
}

TEST(CtrlMenuListados_soloProfesoresMateriasYCursos) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    Consola k("1\n2\n3\n4\n");
    c.menuListados();
    std::string s = k.texto();
    CHECK_CONTAINS(s, "1. Profesores");
    CHECK_CONTAINS(s, "2. Materias");
    CHECK_CONTAINS(s, "3. Cursos (NRC)");
    CHECK_CONTAINS(s, "4. Volver");
    CHECK_NOT_CONTAINS(s, "Periodos");
    CHECK_CONTAINS(s, "PROFESORES (2)");
    CHECK_CONTAINS(s, "MATERIAS (3)");
    CHECK_CONTAINS(s, "CURSOS / NRC (3)");
}

TEST(CtrlMenuProfesor_opcionesYNavegacion) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    RelojFijo reloj(LUNES_FECHA, 7, 5);
    Consola k("3\n" + P1 + "1\n" + P1 + "1\n" + "4\n");   // ver cursos; registrar ingreso (NRC 10001); salir
    c.menuProfesor();
    std::string s = k.texto();
    CHECK_CONTAINS(s, "1. Registrar ingreso");
    CHECK_CONTAINS(s, "2. Registrar salida / desloguearse");
    CHECK_CONTAINS(s, "3. Ver mis cursos");
    CHECK_CONTAINS(s, "4. Volver al menu principal");
    CHECK_CONTAINS(s, "=== MIS CURSOS ===");
    CHECK_EQ(c.getRegistros().getTamanio(), 1u);
}

TEST(CtrlMenuProfesor_noMuestraRelojSimulado) {
    ArchivosTemporales tmp;
    Controlador c = crear();
    RelojFijo reloj(LUNES_FECHA, 7, 5);
    Consola k("4\n");
    c.menuProfesor();
    CHECK_NOT_CONTAINS(k.texto(), "SIMULADO");
}
