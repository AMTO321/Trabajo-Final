#include "mini_test.h"
#include "soporte.h"
#include "../validacion.h"

// Una prueba por funcion.

TEST(Reloj_ahora) {
    {   // simulado: devuelve exactamente lo configurado
        RelojFijo fijo(LUNES_FECHA, 7, 5);
        Instante t = Reloj::ahora();
        CHECK_EQ(t.fecha, "2026-11-02");
        CHECK_EQ(t.minutos, 425);
        CHECK_EQ(t.diaSemana, 1);   // lunes
    }
    // real: valores coherentes
    Instante t = Reloj::ahora();
    CHECK(Validacion::validarFecha(t.fecha));
    CHECK(t.minutos >= 0 && t.minutos <= 1439);
    CHECK(t.diaSemana >= 0 && t.diaSemana <= 6);
}

TEST(Reloj_simular) {
    RelojFijo fijo(LUNES_FECHA, 7, 5);
    Reloj::simular(MARTES_FECHA, 600);          // se puede cambiar varias veces
    Instante t = Reloj::ahora();
    CHECK_EQ(t.fecha, "2026-11-03");
    CHECK_EQ(t.minutos, 600);
    CHECK_EQ(t.diaSemana, 2);
}

TEST(Reloj_detenerSimulacion) {
    Reloj::simular("2026-11-02", 425);
    CHECK(Reloj::estaSimulando());
    Reloj::detenerSimulacion();
    CHECK_FALSE(Reloj::estaSimulando());
}

TEST(Reloj_estaSimulando) {
    CHECK_FALSE(Reloj::estaSimulando());
    {
        RelojFijo fijo(LUNES_FECHA, 7, 5);
        CHECK(Reloj::estaSimulando());
    }
    CHECK_FALSE(Reloj::estaSimulando());
}

TEST(Reloj_diaDeSemanaDeFecha) {
    CHECK_EQ(Reloj::diaDeSemanaDeFecha("2026-11-01"), 0);   // domingo
    CHECK_EQ(Reloj::diaDeSemanaDeFecha("2026-11-02"), 1);   // lunes
    CHECK_EQ(Reloj::diaDeSemanaDeFecha("2026-11-06"), 5);   // viernes
    CHECK_EQ(Reloj::diaDeSemanaDeFecha("2026-11-07"), 6);   // sabado
    CHECK_EQ(Reloj::diaDeSemanaDeFecha("2028-02-29"), 2);   // bisiesto, martes
    CHECK_EQ(Reloj::diaDeSemanaDeFecha("2026-12-31"), 4);   // cambio de anio
}

TEST(Reloj_aDiaSemana) {
    DiaSemana d = VIERNES;
    CHECK(Reloj::aDiaSemana(1, d)); CHECK_EQ(d, LUNES);
    CHECK(Reloj::aDiaSemana(2, d)); CHECK_EQ(d, MARTES);
    CHECK(Reloj::aDiaSemana(3, d)); CHECK_EQ(d, MIERCOLES);
    CHECK(Reloj::aDiaSemana(4, d)); CHECK_EQ(d, JUEVES);
    CHECK(Reloj::aDiaSemana(5, d)); CHECK_EQ(d, VIERNES);
    // fin de semana y valores fuera de rango: false y no modifica el resultado
    d = JUEVES;
    CHECK_FALSE(Reloj::aDiaSemana(0, d));   // domingo
    CHECK_FALSE(Reloj::aDiaSemana(6, d));   // sabado
    CHECK_FALSE(Reloj::aDiaSemana(7, d));
    CHECK_FALSE(Reloj::aDiaSemana(-1, d));
    CHECK_EQ(d, JUEVES);
}
