#include "mini_test.h"
#include "../validacion.h"

// ---------- utilidades de texto ----------
TEST(Validacion_trim_quitaEspaciosYSaltos) {
    CHECK_EQ(Validacion::trim("  hola  "), "hola");
    CHECK_EQ(Validacion::trim("\thola\r\n"), "hola");
    CHECK_EQ(Validacion::trim("   "), "");
    CHECK_EQ(Validacion::trim(""), "");
    CHECK_EQ(Validacion::trim("a b"), "a b");
}

TEST(Validacion_normalizarEspacios_colapsaEspaciosDobles) {
    CHECK_EQ(Validacion::normalizarEspacios("  Juan    Carlos  "), "Juan Carlos");
    CHECK_EQ(Validacion::normalizarEspacios("A\t\tB"), "A B");
    CHECK_EQ(Validacion::normalizarEspacios(""), "");
}

TEST(Validacion_aMayusculas_soloAscii) {
    CHECK_EQ(Validacion::aMayusculas("alge101"), "ALGE101");
    CHECK_EQ(Validacion::aMayusculas("AbC-1"), "ABC-1");
    // los bytes de tildes/enie (>= 0x80) no se alteran
    CHECK_EQ(Validacion::aMayusculas("a\xC3\xB1" "o"), "A\xC3\xB1" "O");
}

TEST(Validacion_dividir_separaPorComaYRecorta) {
    std::vector<std::string> p = Validacion::dividir("a, b ,c", ',');
    CHECK_EQ(p.size(), 3u);
    CHECK_EQ(p[0], "a");
    CHECK_EQ(p[1], "b");
    CHECK_EQ(p[2], "c");
}

TEST(Validacion_dividir_camposVaciosYSinSeparador) {
    std::vector<std::string> p = Validacion::dividir("a,,c,", ',');
    CHECK_EQ(p.size(), 4u);
    CHECK_EQ(p[1], "");
    CHECK_EQ(p[3], "");
    CHECK_EQ(Validacion::dividir("solo", ',').size(), 1u);
    CHECK_EQ(Validacion::dividir("", ',').size(), 1u);
}

// ---------- validaciones generales ----------
TEST(Validacion_soloNumeros) {
    CHECK(Validacion::soloNumeros("0123456789"));
    CHECK_FALSE(Validacion::soloNumeros(""));
    CHECK_FALSE(Validacion::soloNumeros("12a"));
    CHECK_FALSE(Validacion::soloNumeros("-1"));
    CHECK_FALSE(Validacion::soloNumeros("1 2"));
}

TEST(Validacion_soloLetras_aceptaTildesYEspacios) {
    CHECK(Validacion::soloLetras("Juan Carlos"));
    CHECK(Validacion::soloLetras("Mu\xC3\xB1oz"));       // Munoz con enie (UTF-8)
    CHECK(Validacion::soloLetras("Jos\xC3\xA9"));        // Jose con tilde
    CHECK_FALSE(Validacion::soloLetras(""));
    CHECK_FALSE(Validacion::soloLetras("   "));          // solo espacios
    CHECK_FALSE(Validacion::soloLetras("Juan1"));
    CHECK_FALSE(Validacion::soloLetras("Ana-Maria"));
}

TEST(Validacion_validarNombrePersona_limites) {
    CHECK(Validacion::validarNombrePersona("Al"));
    CHECK_FALSE(Validacion::validarNombrePersona("A"));
    CHECK(Validacion::validarNombrePersona(std::string(50, 'a')));
    CHECK_FALSE(Validacion::validarNombrePersona(std::string(51, 'a')));
    CHECK_FALSE(Validacion::validarNombrePersona("Juan123"));
}

TEST(Validacion_validarTextoLibre) {
    CHECK(Validacion::validarTextoLibre("Calculo 1", 3, 60));
    CHECK(Validacion::validarTextoLibre("abc", 3, 3));
    CHECK_FALSE(Validacion::validarTextoLibre("ab", 3, 60));                // muy corto
    CHECK_FALSE(Validacion::validarTextoLibre(std::string(61, 'a'), 3, 60));// muy largo
    CHECK_FALSE(Validacion::validarTextoLibre("Calculo, 1", 3, 60));        // sin comas (CSV)
    CHECK_FALSE(Validacion::validarTextoLibre("   ", 3, 60));               // sin alfanumericos
    CHECK_FALSE(Validacion::validarTextoLibre("Calc@", 3, 60));
}

// ---------- cedula ecuatoriana ----------
TEST(Validacion_validarCedula_validas) {
    CHECK(Validacion::validarCedula("0605293885"));
    CHECK(Validacion::validarCedula("1710034065"));
    CHECK(Validacion::validarCedula("1701234567"));
    CHECK(Validacion::validarCedula("0102030400"));
    CHECK(Validacion::validarCedula("3000000012"));   // provincia 30 (ecuatorianos en el exterior)
}

TEST(Validacion_validarCedula_invalidas) {
    CHECK_FALSE(Validacion::validarCedula(""));
    CHECK_FALSE(Validacion::validarCedula("060529388"));     // 9 digitos
    CHECK_FALSE(Validacion::validarCedula("06052938855"));   // 11 digitos
    CHECK_FALSE(Validacion::validarCedula("060529388A"));    // letra
    CHECK_FALSE(Validacion::validarCedula("0605293886"));    // digito verificador incorrecto
    CHECK_FALSE(Validacion::validarCedula("2500000019"));    // provincia 25 no existe
    CHECK_FALSE(Validacion::validarCedula("0000000000"));    // provincia 00
    CHECK_FALSE(Validacion::validarCedula("1760000008"));    // tercer digito 6 (no es persona natural)
}

// ---------- dominio academico ----------
TEST(Validacion_validarIdMateria) {
    CHECK(Validacion::validarIdMateria("ALGE101"));
    CHECK(Validacion::validarIdMateria("ABCD"));
    CHECK(Validacion::validarIdMateria("ABCDEFGHIJ"));        // 10
    CHECK_FALSE(Validacion::validarIdMateria("ABC"));         // 3
    CHECK_FALSE(Validacion::validarIdMateria("ABCDEFGHIJK")); // 11
    CHECK_FALSE(Validacion::validarIdMateria("alge101"));     // minusculas
    CHECK_FALSE(Validacion::validarIdMateria("ALGE 101"));    // espacio
    CHECK_FALSE(Validacion::validarIdMateria("ALGE-101"));
}

TEST(Validacion_validarNrc_cincoDigitosDesde10001) {
    CHECK(Validacion::validarNrc("10001"));
    CHECK(Validacion::validarNrc("10002"));
    CHECK(Validacion::validarNrc("55555"));
    CHECK(Validacion::validarNrc("99999"));
}

TEST(Validacion_validarNrc_rechazaFormatosIncorrectos) {
    CHECK_FALSE(Validacion::validarNrc(""));
    CHECK_FALSE(Validacion::validarNrc("10000"));    // por debajo del inicial
    CHECK_FALSE(Validacion::validarNrc("00001"));
    CHECK_FALSE(Validacion::validarNrc("1000"));     // 4 digitos
    CHECK_FALSE(Validacion::validarNrc("100001"));   // 6 digitos
    CHECK_FALSE(Validacion::validarNrc("1000A"));
    CHECK_FALSE(Validacion::validarNrc("-1001"));
    CHECK_FALSE(Validacion::validarNrc("10 01"));
}

// ---------- fechas y horas ----------
TEST(Validacion_validarFecha_validas) {
    CHECK(Validacion::validarFecha("2026-11-02"));
    CHECK(Validacion::validarFecha("2000-01-01"));
    CHECK(Validacion::validarFecha("2100-12-31"));
    CHECK(Validacion::validarFecha("2026-04-30"));
}

TEST(Validacion_validarFecha_bisiestos) {
    CHECK(Validacion::validarFecha("2024-02-29"));      // divisible entre 4
    CHECK(Validacion::validarFecha("2000-02-29"));      // divisible entre 400
    CHECK_FALSE(Validacion::validarFecha("2023-02-29"));
    CHECK_FALSE(Validacion::validarFecha("2100-02-29")); // divisible entre 100 pero no 400
    CHECK(Validacion::validarFecha("2023-02-28"));
}

TEST(Validacion_validarFecha_invalidas) {
    CHECK_FALSE(Validacion::validarFecha(""));
    CHECK_FALSE(Validacion::validarFecha("2026/11/02"));
    CHECK_FALSE(Validacion::validarFecha("2026-1-02"));
    CHECK_FALSE(Validacion::validarFecha("2026-11-2"));
    CHECK_FALSE(Validacion::validarFecha("2026-13-01"));
    CHECK_FALSE(Validacion::validarFecha("2026-00-10"));
    CHECK_FALSE(Validacion::validarFecha("2026-11-00"));
    CHECK_FALSE(Validacion::validarFecha("2026-11-31"));   // noviembre tiene 30
    CHECK_FALSE(Validacion::validarFecha("1999-12-31"));   // fuera de rango
    CHECK_FALSE(Validacion::validarFecha("2101-01-01"));   // fuera de rango
    CHECK_FALSE(Validacion::validarFecha("abcd-ef-gh"));
}

TEST(Validacion_parsearHora_validas) {
    int m = -1;
    CHECK(Validacion::parsearHora("00:00", m)); CHECK_EQ(m, 0);
    CHECK(Validacion::parsearHora("07:05", m)); CHECK_EQ(m, 425);
    CHECK(Validacion::parsearHora("12:30", m)); CHECK_EQ(m, 750);
    CHECK(Validacion::parsearHora("23:59", m)); CHECK_EQ(m, 1439);
}

TEST(Validacion_parsearHora_invalidas_noModificanResultado) {
    int m = 777;
    CHECK_FALSE(Validacion::parsearHora("24:00", m));
    CHECK_FALSE(Validacion::parsearHora("07:60", m));
    CHECK_FALSE(Validacion::parsearHora("7:05", m));
    CHECK_FALSE(Validacion::parsearHora("07-05", m));
    CHECK_FALSE(Validacion::parsearHora("ab:cd", m));
    CHECK_FALSE(Validacion::parsearHora("", m));
    CHECK_FALSE(Validacion::parsearHora("07:5", m));
    CHECK_EQ(m, 777);
}
