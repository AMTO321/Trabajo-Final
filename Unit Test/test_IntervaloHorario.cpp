#include "mini_test.h"
#include "IntervaloHorario.h"

// Todas las pruebas usan una clase de 07:00 a 09:00  (420 a 540 minutos).
static const int INICIO = 7 * 60;   // 420
static const int FIN = 9 * 60;      // 540

TEST_CASE("IntervaloHorario: el constructor por defecto es 0-0") {
    IntervaloHorario i;
    CHECK_EQ(i.getHoraInicioMinutos(), 0);
    CHECK_EQ(i.getHoraFinMinutos(), 0);
}

TEST_CASE("IntervaloHorario: el constructor con parametros guarda inicio y fin") {
    IntervaloHorario i(INICIO, FIN);
    CHECK_EQ(i.getHoraInicioMinutos(), 420);
    CHECK_EQ(i.getHoraFinMinutos(), 540);
}

TEST_CASE("IntervaloHorario: los setters modifican inicio y fin") {
    IntervaloHorario i;
    i.setHoraInicioMinutos(480);
    i.setHoraFinMinutos(600);
    CHECK_EQ(i.getHoraInicioMinutos(), 480);
    CHECK_EQ(i.getHoraFinMinutos(), 600);
}

// ---------- estaEnFranja (limites inclusivos) ----------

TEST_CASE("IntervaloHorario::estaEnFranja: un minuto antes del inicio esta fuera") {
    IntervaloHorario i(INICIO, FIN);
    CHECK(!i.estaEnFranja(INICIO - 1));   // 06:59
}

TEST_CASE("IntervaloHorario::estaEnFranja: el inicio exacto esta dentro") {
    IntervaloHorario i(INICIO, FIN);
    CHECK(i.estaEnFranja(INICIO));        // 07:00
}

TEST_CASE("IntervaloHorario::estaEnFranja: un minuto intermedio esta dentro") {
    IntervaloHorario i(INICIO, FIN);
    CHECK(i.estaEnFranja(8 * 60));        // 08:00
}

TEST_CASE("IntervaloHorario::estaEnFranja: el fin exacto esta dentro") {
    IntervaloHorario i(INICIO, FIN);
    CHECK(i.estaEnFranja(FIN));           // 09:00
}

TEST_CASE("IntervaloHorario::estaEnFranja: un minuto despues del fin esta fuera") {
    IntervaloHorario i(INICIO, FIN);
    CHECK(!i.estaEnFranja(FIN + 1));      // 09:01
}

// ---------- esIngresoValido (holgura de 10 minutos) ----------

TEST_CASE("IntervaloHorario::esIngresoValido: antes del inicio no es valido") {
    IntervaloHorario i(INICIO, FIN);
    CHECK(!i.esIngresoValido(INICIO - 1));   // 06:59
}

TEST_CASE("IntervaloHorario::esIngresoValido: es valido desde el inicio exacto") {
    IntervaloHorario i(INICIO, FIN);
    CHECK(i.esIngresoValido(INICIO));        // 07:00
}

TEST_CASE("IntervaloHorario::esIngresoValido: es valido hasta 10 minutos despues (limite inclusivo)") {
    IntervaloHorario i(INICIO, FIN);
    CHECK(i.esIngresoValido(INICIO + 5));    // 07:05
    CHECK(i.esIngresoValido(INICIO + 10));   // 07:10
}

TEST_CASE("IntervaloHorario::esIngresoValido: 11 minutos despues ya no es valido") {
    IntervaloHorario i(INICIO, FIN);
    CHECK(!i.esIngresoValido(INICIO + 11));  // 07:11
}

TEST_CASE("IntervaloHorario::esIngresoValido: durante la clase pero fuera de la holgura no es valido") {
    IntervaloHorario i(INICIO, FIN);
    CHECK(!i.esIngresoValido(8 * 60));       // 08:00
    CHECK(!i.esIngresoValido(FIN));          // 09:00
}

TEST_CASE("IntervaloHorario::esIngresoValido: la holgura se calcula desde el inicio de cada intervalo") {
    IntervaloHorario i(9 * 60, 11 * 60);     // 09:00 a 11:00
    CHECK(i.esIngresoValido(9 * 60 + 10));
    CHECK(!i.esIngresoValido(9 * 60 + 11));
    CHECK(!i.esIngresoValido(7 * 60 + 5));   // la franja anterior no sirve
}

// ---------- Defectos conocidos / propuestas ----------

DEFECT_CASE("[PROPUESTA] IntervaloHorario rechaza un intervalo con inicio posterior al fin") {
    CHECK_THROWS(IntervaloHorario(FIN, INICIO));
}

DEFECT_CASE("[PROPUESTA] IntervaloHorario rechaza minutos fuera del dia (negativos o >= 1440)") {
    CHECK_THROWS(IntervaloHorario(-1, 60));
    CHECK_THROWS(IntervaloHorario(0, 1440));
}
