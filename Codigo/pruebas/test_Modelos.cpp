#include "mini_test.h"
#include "../enumeracion.h"
#include "../Profesor.h"
#include "../Materia.h"
#include "../Aula.h"
#include "../RegistroAsistencia.h"

// Una prueba por funcion. Cada una agrupa todos los casos de esa funcion.

// =====================================================================
//  enumeracion.h
// =====================================================================
TEST(Enum_nombreDia) {
    CHECK_EQ(nombreDia(LUNES), "LUNES");
    CHECK_EQ(nombreDia(MARTES), "MARTES");
    CHECK_EQ(nombreDia(MIERCOLES), "MIERCOLES");
    CHECK_EQ(nombreDia(JUEVES), "JUEVES");
    CHECK_EQ(nombreDia(VIERNES), "VIERNES");
}

TEST(Enum_parseDia) {
    // ida y vuelta con los cinco dias
    for (int i = LUNES; i <= VIERNES; i++) {
        DiaSemana d = LUNES;
        CHECK(parseDia(nombreDia(static_cast<DiaSemana>(i)), d));
        CHECK_EQ((int)d, i);
    }
    // texto invalido: devuelve false y no modifica el resultado
    DiaSemana d = MARTES;
    CHECK_FALSE(parseDia("SABADO", d));
    CHECK_FALSE(parseDia("lunes", d));    // distingue mayusculas
    CHECK_FALSE(parseDia("", d));
    CHECK_EQ(d, MARTES);
}

TEST(Enum_nombreEstado) {
    CHECK_EQ(nombreEstado(PUNTUAL), "PUNTUAL");
    CHECK_EQ(nombreEstado(ATRASO), "ATRASO");
    CHECK_EQ(nombreEstado(AUSENTE), "AUSENTE");
}

// =====================================================================
//  Profesor
// =====================================================================
TEST(Profesor_constructores) {
    Profesor vacio;
    CHECK_EQ(vacio.getCedula(), "");
    CHECK_EQ(vacio.getNombres(), "");
    CHECK_EQ(vacio.getApellidos(), "");
    Profesor p("0605293885", "Adrian", "Toa");
    CHECK_EQ(p.getCedula(), "0605293885");
    CHECK_EQ(p.getNombres(), "Adrian");
    CHECK_EQ(p.getApellidos(), "Toa");
}

TEST(Profesor_getCedula_setCedula) {
    Profesor p;
    p.setCedula("1710034065");
    CHECK_EQ(p.getCedula(), "1710034065");
}

TEST(Profesor_getNombres_setNombres) {
    Profesor p;
    p.setNombres("Ana");
    CHECK_EQ(p.getNombres(), "Ana");
}

TEST(Profesor_getApellidos_setApellidos) {
    Profesor p;
    p.setApellidos("Perez");
    CHECK_EQ(p.getApellidos(), "Perez");
}

// =====================================================================
//  Materia
// =====================================================================
TEST(Materia_constructores) {
    Materia vacia;
    CHECK_EQ(vacia.getId(), "");
    CHECK_EQ(vacia.getNombre(), "");
    CHECK_EQ(vacia.getCreditos(), 0);
    Materia m("ALGE101", "Algebra Lineal", 4);
    CHECK_EQ(m.getId(), "ALGE101");
    CHECK_EQ(m.getNombre(), "Algebra Lineal");
    CHECK_EQ(m.getCreditos(), 4);
}

TEST(Materia_getId_setId) {
    Materia m;
    m.setId("CALC101");
    CHECK_EQ(m.getId(), "CALC101");
}

TEST(Materia_getNombre_setNombre) {
    Materia m;
    m.setNombre("Calculo");
    CHECK_EQ(m.getNombre(), "Calculo");
}

TEST(Materia_getCreditos_setCreditos) {
    Materia m;
    m.setCreditos(5);
    CHECK_EQ(m.getCreditos(), 5);
}

// =====================================================================
//  Aula (curso identificado por NRC)
// =====================================================================
TEST(Aula_constructores) {
    Aula vacia;
    CHECK_EQ(vacia.getNrc(), "");
    CHECK_EQ(vacia.getIdMateria(), "");
    CHECK_EQ(vacia.getCedulaProfesor(), "");
    CHECK_EQ(vacia.getDia(), LUNES);
    Aula a("10001", "ALGE101", "0605293885", MARTES, IntervaloHorario::franja(2));
    CHECK_EQ(a.getNrc(), "10001");
    CHECK_EQ(a.getIdMateria(), "ALGE101");
    CHECK_EQ(a.getCedulaProfesor(), "0605293885");
    CHECK_EQ(a.getDia(), MARTES);
    CHECK_EQ(a.getHorario().toString(), "09:00 - 11:00");
}

TEST(Aula_getNrc_setNrc) {
    Aula a;
    a.setNrc("10005");
    CHECK_EQ(a.getNrc(), "10005");
}

TEST(Aula_getIdMateria_setIdMateria) {
    Aula a;
    a.setIdMateria("FISI101");
    CHECK_EQ(a.getIdMateria(), "FISI101");
}

TEST(Aula_getCedulaProfesor_setCedulaProfesor) {
    Aula a;
    a.setCedulaProfesor("0605293885");
    CHECK_EQ(a.getCedulaProfesor(), "0605293885");
}

TEST(Aula_getDia_setDia) {
    Aula a;
    a.setDia(VIERNES);
    CHECK_EQ(a.getDia(), VIERNES);
}

TEST(Aula_getHorario_setHorario) {
    Aula a;
    a.setHorario(IntervaloHorario::franja(7));
    CHECK_EQ(a.getHorario().toString(), "19:00 - 21:00");
}

TEST(Aula_asignarProfesor) {
    Aula a("10001", "ALGE101", "0605293885", LUNES, IntervaloHorario::franja(1));
    a.asignarProfesor("1710034065");
    CHECK_EQ(a.getCedulaProfesor(), "1710034065");
}

TEST(Aula_tieneCruce) {   // mismo profesor + mismo dia + franjas traslapadas
    Aula a("10001", "ALGE101", "0605293885", LUNES, IntervaloHorario::franja(1));   // Lunes 07-09
    CHECK(a.tieneCruce("0605293885", LUNES, IntervaloHorario::franja(1)));
    CHECK(a.tieneCruce("0605293885", LUNES, IntervaloHorario(480, 600)));           // 08:00-10:00
    CHECK_FALSE(a.tieneCruce("1710034065", LUNES, IntervaloHorario::franja(1)));    // otro profesor
    CHECK_FALSE(a.tieneCruce("0605293885", MARTES, IntervaloHorario::franja(1)));   // otro dia
    CHECK_FALSE(a.tieneCruce("0605293885", LUNES, IntervaloHorario::franja(2)));    // contigua (09-11)
}

TEST(Aula_ocupaHorario) {   // mismo dia + franjas traslapadas, sin importar el profesor
    Aula a("10001", "ALGE101", "0605293885", LUNES, IntervaloHorario::franja(1));   // Lunes 07-09
    CHECK(a.ocupaHorario(LUNES, IntervaloHorario::franja(1)));
    CHECK(a.ocupaHorario(LUNES, IntervaloHorario(480, 600)));       // 08:00-10:00
    CHECK_FALSE(a.ocupaHorario(MARTES, IntervaloHorario::franja(1)));
    CHECK_FALSE(a.ocupaHorario(LUNES, IntervaloHorario::franja(2)));// contigua
    CHECK_FALSE(a.ocupaHorario(LUNES, IntervaloHorario::franja(7)));
}

// =====================================================================
//  RegistroAsistencia
// =====================================================================
static RegistroAsistencia registroEjemplo() {
    return RegistroAsistencia("2026-11-02", "07:05", "0605293885", "10001", "ENTRADA", "PUNTUAL");
}

TEST(RegistroAsistencia_constructores) {
    RegistroAsistencia vacio;
    CHECK_EQ(vacio.getFecha(), "");
    CHECK_EQ(vacio.getHora(), "");
    CHECK_EQ(vacio.getCedula(), "");
    CHECK_EQ(vacio.getNrc(), "");
    CHECK_EQ(vacio.getTipo(), "");
    CHECK_EQ(vacio.getEstado(), "");
    RegistroAsistencia r = registroEjemplo();
    CHECK_EQ(r.getFecha(), "2026-11-02");
    CHECK_EQ(r.getNrc(), "10001");
}

TEST(RegistroAsistencia_getFecha) { CHECK_EQ(registroEjemplo().getFecha(), "2026-11-02"); }
TEST(RegistroAsistencia_getHora) { CHECK_EQ(registroEjemplo().getHora(), "07:05"); }
TEST(RegistroAsistencia_getCedula) { CHECK_EQ(registroEjemplo().getCedula(), "0605293885"); }
TEST(RegistroAsistencia_getNrc) { CHECK_EQ(registroEjemplo().getNrc(), "10001"); }
TEST(RegistroAsistencia_getTipo) { CHECK_EQ(registroEjemplo().getTipo(), "ENTRADA"); }
TEST(RegistroAsistencia_getEstado) { CHECK_EQ(registroEjemplo().getEstado(), "PUNTUAL"); }

TEST(RegistroAsistencia_toCsv) {
    CHECK_EQ(registroEjemplo().toCsv(), "2026-11-02,07:05,0605293885,10001,ENTRADA,PUNTUAL");
    RegistroAsistencia vacio;
    CHECK_EQ(vacio.toCsv(), ",,,,,");
}

TEST(RegistroAsistencia_toString) {
    RegistroAsistencia r("2026-11-02", "08:55", "0605293885", "10001", "SALIDA", "REGISTRADA");
    std::string s = r.toString();
    CHECK_CONTAINS(s, "2026-11-02");
    CHECK_CONTAINS(s, "08:55");
    CHECK_CONTAINS(s, "NRC: 10001");
    CHECK_CONTAINS(s, "SALIDA");
    CHECK_CONTAINS(s, "REGISTRADA");
}
