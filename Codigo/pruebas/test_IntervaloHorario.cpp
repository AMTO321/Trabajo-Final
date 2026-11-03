#include "mini_test.h"
#include "../IntervaloHorario.h"

TEST(Intervalo_constructorPorDefectoYSetters) {
    IntervaloHorario h;
    CHECK_EQ(h.getHoraInicioMinutos(), 0);
    CHECK_EQ(h.getHoraFinMinutos(), 0);
    h.setHoraInicioMinutos(420);
    h.setHoraFinMinutos(540);
    CHECK_EQ(h.getHoraInicioMinutos(), 420);
    CHECK_EQ(h.getHoraFinMinutos(), 540);
}

TEST(Intervalo_franjasOficiales_sieteDeDosHoras) {
    CHECK_EQ(IntervaloHorario::totalFranjas(), 7);
    CHECK_EQ(IntervaloHorario::franja(1).toString(), "07:00 - 09:00");
    CHECK_EQ(IntervaloHorario::franja(2).toString(), "09:00 - 11:00");
    CHECK_EQ(IntervaloHorario::franja(4).toString(), "13:00 - 15:00");
    CHECK_EQ(IntervaloHorario::franja(7).toString(), "19:00 - 21:00");
    for (int i = 1; i <= 7; i++) {
        CHECK(IntervaloHorario::franja(i).esFranjaOficial());
        CHECK_EQ(IntervaloHorario::franja(i).getHoraFinMinutos() - IntervaloHorario::franja(i).getHoraInicioMinutos(), 120);
    }
}

TEST(Intervalo_esFranjaOficial_rechazaOtros) {
    CHECK_FALSE(IntervaloHorario(420, 530).esFranjaOficial());   // 07:00-08:50
    CHECK_FALSE(IntervaloHorario(430, 550).esFranjaOficial());   // desplazada
    CHECK_FALSE(IntervaloHorario(0, 0).esFranjaOficial());
    CHECK_FALSE(IntervaloHorario(1260, 1380).esFranjaOficial()); // 21:00-23:00 (octava)
    CHECK_FALSE(IntervaloHorario(420, 660).esFranjaOficial());   // 4 horas
}

TEST(Intervalo_textoHora_formatoDosDigitos) {
    CHECK_EQ(IntervaloHorario::textoHora(0), "00:00");
    CHECK_EQ(IntervaloHorario::textoHora(425), "07:05");
    CHECK_EQ(IntervaloHorario::textoHora(540), "09:00");
    CHECK_EQ(IntervaloHorario::textoHora(1439), "23:59");
}

TEST(Intervalo_estaEnFranja_inicioInclusivoFinExclusivo) {
    IntervaloHorario h = IntervaloHorario::franja(1);   // 420..540
    CHECK_FALSE(h.estaEnFranja(419));
    CHECK(h.estaEnFranja(420));
    CHECK(h.estaEnFranja(539));
    CHECK_FALSE(h.estaEnFranja(540));
}

TEST(Intervalo_esIngresoValido_puntualPrimeros10Min) {
    IntervaloHorario h = IntervaloHorario::franja(1);   // 07:00
    CHECK_FALSE(h.esIngresoValido(419));   // 06:59
    CHECK(h.esIngresoValido(420));         // 07:00
    CHECK(h.esIngresoValido(430));         // 07:10 (limite)
    CHECK_FALSE(h.esIngresoValido(431));   // 07:11 -> atraso
}

TEST(Intervalo_esIngresoPermitido_hastaAntesDeVentanaDeSalida) {
    IntervaloHorario h = IntervaloHorario::franja(1);   // 420..540
    CHECK_FALSE(h.esIngresoPermitido(419));
    CHECK(h.esIngresoPermitido(420));
    CHECK(h.esIngresoPermitido(529));      // fin - 11
    CHECK_FALSE(h.esIngresoPermitido(530));// fin - 10
    CHECK_FALSE(h.esIngresoPermitido(540));
}

TEST(Intervalo_esSalidaValido_ultimos10Min) {
    IntervaloHorario h = IntervaloHorario::franja(1);   // 420..540
    CHECK_FALSE(h.esSalidaValido(529));
    CHECK(h.esSalidaValido(530));          // 08:50
    CHECK(h.esSalidaValido(539));          // 08:59
    CHECK_FALSE(h.esSalidaValido(540));    // 09:00
}

TEST(Intervalo_seCruzaCon) {
    IntervaloHorario a(420, 540), b(540, 660), c(500, 600), d(420, 540), e(0, 100);
    CHECK_FALSE(a.seCruzaCon(b));   // contiguas: no se cruzan
    CHECK_FALSE(b.seCruzaCon(a));
    CHECK(a.seCruzaCon(c));         // se traslapan
    CHECK(c.seCruzaCon(a));
    CHECK(b.seCruzaCon(c));
    CHECK(a.seCruzaCon(d));         // identicas
    CHECK_FALSE(a.seCruzaCon(e));
}
