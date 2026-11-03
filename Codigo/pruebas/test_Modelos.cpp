#include "mini_test.h"
#include "../enumeracion.h"
#include "../Profesor.h"
#include "../Materia.h"
#include "../Aula.h"
#include "../RegistroAsistencia.h"

// ---------- enumeracion.h ----------
TEST(Enum_nombreDia) {
    CHECK_EQ(nombreDia(LUNES), "LUNES");
    CHECK_EQ(nombreDia(MARTES), "MARTES");
    CHECK_EQ(nombreDia(MIERCOLES), "MIERCOLES");
    CHECK_EQ(nombreDia(JUEVES), "JUEVES");
    CHECK_EQ(nombreDia(VIERNES), "VIERNES");
}

TEST(Enum_parseDia_ida_y_vuelta) {
    for (int i = LUNES; i <= VIERNES; i++) {
        DiaSemana d = LUNES;
        CHECK(parseDia(nombreDia(static_cast<DiaSemana>(i)), d));
        CHECK_EQ((int)d, i);
    }
}

TEST(Enum_parseDia_rechazaTextoInvalido) {
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

// ---------- Profesor ----------
TEST(Profesor_constructoresYSetters) {
    Profesor vacio;
    CHECK_EQ(vacio.getCedula(), "");
    Profesor p("0605293885", "Adrian", "Toa");
    CHECK_EQ(p.getCedula(), "0605293885");
    CHECK_EQ(p.getNombres(), "Adrian");
    CHECK_EQ(p.getApellidos(), "Toa");
    p.setCedula("1710034065"); p.setNombres("Ana"); p.setApellidos("Perez");
    CHECK_EQ(p.getCedula(), "1710034065");
    CHECK_EQ(p.getNombres(), "Ana");
    CHECK_EQ(p.getApellidos(), "Perez");
}

// ---------- Materia ----------
TEST(Materia_constructoresYSetters) {
    Materia vacia;
    CHECK_EQ(vacia.getId(), "");
    CHECK_EQ(vacia.getCreditos(), 0);
    Materia m("ALGE101", "Algebra Lineal", 4);
    CHECK_EQ(m.getId(), "ALGE101");
    CHECK_EQ(m.getNombre(), "Algebra Lineal");
    CHECK_EQ(m.getCreditos(), 4);
    m.setId("CALC101"); m.setNombre("Calculo 1"); m.setCreditos(5);
    CHECK_EQ(m.getId(), "CALC101");
    CHECK_EQ(m.getNombre(), "Calculo 1");
    CHECK_EQ(m.getCreditos(), 5);
}

// ---------- Aula (curso identificado por NRC) ----------
TEST(Aula_constructorPorDefecto) {
    Aula a;
    CHECK_EQ(a.getNrc(), "");
    CHECK_EQ(a.getIdMateria(), "");
    CHECK_EQ(a.getCedulaProfesor(), "");
    CHECK_EQ(a.getDia(), LUNES);
}

TEST(Aula_constructorConParametros_sinPeriodoNiParalelo) {
    Aula a("10001", "ALGE101", "0605293885", MARTES, IntervaloHorario::franja(2));
    CHECK_EQ(a.getNrc(), "10001");
    CHECK_EQ(a.getIdMateria(), "ALGE101");
    CHECK_EQ(a.getCedulaProfesor(), "0605293885");
    CHECK_EQ(a.getDia(), MARTES);
    CHECK_EQ(a.getHorario().toString(), "09:00 - 11:00");
}

TEST(Aula_settersYAsignarProfesor) {
    Aula a;
    a.setNrc("10005"); a.setIdMateria("FISI101"); a.setDia(VIERNES);
    a.setHorario(IntervaloHorario::franja(7));
    a.setCedulaProfesor("0605293885");
    CHECK_EQ(a.getNrc(), "10005");
    CHECK_EQ(a.getIdMateria(), "FISI101");
    CHECK_EQ(a.getDia(), VIERNES);
    CHECK_EQ(a.getHorario().toString(), "19:00 - 21:00");
    a.asignarProfesor("1710034065");
    CHECK_EQ(a.getCedulaProfesor(), "1710034065");
}

TEST(Aula_tieneCruce_mismoProfesorDiaYHorarioTraslapado) {
    Aula a("10001", "ALGE101", "0605293885", LUNES, IntervaloHorario::franja(1));  // 07-09
    CHECK(a.tieneCruce("0605293885", LUNES, IntervaloHorario::franja(1)));
    CHECK(a.tieneCruce("0605293885", LUNES, IntervaloHorario(480, 600)));          // 08:00-10:00
}

TEST(Aula_tieneCruce_noHayChoque) {
    Aula a("10001", "ALGE101", "0605293885", LUNES, IntervaloHorario::franja(1));
    CHECK_FALSE(a.tieneCruce("1710034065", LUNES, IntervaloHorario::franja(1)));   // otro profesor
    CHECK_FALSE(a.tieneCruce("0605293885", MARTES, IntervaloHorario::franja(1)));  // otro dia
    CHECK_FALSE(a.tieneCruce("0605293885", LUNES, IntervaloHorario::franja(2)));   // contigua (09-11)
}

// ---------- RegistroAsistencia ----------
TEST(Registro_gettersYCsv) {
    RegistroAsistencia r("2026-11-02", "07:05", "0605293885", "10001", "ENTRADA", "PUNTUAL");
    CHECK_EQ(r.getFecha(), "2026-11-02");
    CHECK_EQ(r.getHora(), "07:05");
    CHECK_EQ(r.getCedula(), "0605293885");
    CHECK_EQ(r.getNrc(), "10001");
    CHECK_EQ(r.getTipo(), "ENTRADA");
    CHECK_EQ(r.getEstado(), "PUNTUAL");
    CHECK_EQ(r.toCsv(), "2026-11-02,07:05,0605293885,10001,ENTRADA,PUNTUAL");
}

TEST(Registro_toStringYDefecto) {
    RegistroAsistencia vacio;
    CHECK_EQ(vacio.getNrc(), "");
    CHECK_EQ(vacio.toCsv(), ",,,,,");
    RegistroAsistencia r("2026-11-02", "08:55", "0605293885", "10001", "SALIDA", "REGISTRADA");
    std::string s = r.toString();
    CHECK_CONTAINS(s, "2026-11-02");
    CHECK_CONTAINS(s, "08:55");
    CHECK_CONTAINS(s, "NRC: 10001");
    CHECK_CONTAINS(s, "SALIDA");
    CHECK_CONTAINS(s, "REGISTRADA");
}
