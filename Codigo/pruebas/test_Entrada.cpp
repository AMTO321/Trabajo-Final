#include "mini_test.h"
#include "soporte.h"
#include "../Entrada.h"
#include <stdexcept>

TEST(Entrada_leerLinea_muestraMensajeYRecortaEspacios) {
    Consola c("   hola mundo  \n");
    std::string r = Entrada::leerLinea("Nombre: ");
    CHECK_EQ(r, "hola mundo");
    CHECK_CONTAINS(c.texto(), "Nombre: ");
}

TEST(Entrada_leerLinea_lineaVaciaEsValida) {
    Consola c("\n");
    CHECK_EQ(Entrada::leerLinea("x"), "");
}

TEST(Entrada_leerLinea_entradaCerradaLanzaExcepcion) {
    Consola c("");
    CHECK_THROWS(Entrada::leerLinea("x"));
}

TEST(Entrada_leerEntero_valorValido) {
    Consola c("5\n");
    CHECK_EQ(Entrada::leerEntero("n: ", 1, 10), 5);
}

TEST(Entrada_leerEntero_limitesInclusivos) {
    Consola c("1\n10\n0\n");
    CHECK_EQ(Entrada::leerEntero("n: ", 1, 10), 1);
    CHECK_EQ(Entrada::leerEntero("n: ", 1, 10), 10);
    CHECK_EQ(Entrada::leerEntero("n: ", 0, 3), 0);
}

TEST(Entrada_leerEntero_repiteHastaRecibirValorValido) {
    Consola c("abc\n\n-3\n11\n0\n3.5\n1234567890\n7\n");
    CHECK_EQ(Entrada::leerEntero("n: ", 1, 10), 7);
    std::string s = c.texto();
    CHECK_CONTAINS(s, "Error: ingrese un numero entero entre 1 y 10");   // abc, vacio, -3, 3.5, 10 digitos
    CHECK_CONTAINS(s, "Error: el valor debe estar entre 1 y 10");        // 11 y 0
}

TEST(Entrada_leerEntero_entradaCerradaLanzaExcepcion) {
    Consola c("abc\n");
    CHECK_THROWS(Entrada::leerEntero("n: ", 1, 10));
}

TEST(Entrada_confirmar_respuestasAfirmativas) {
    Consola c("s\nS\nsi\nSI\nSi\n");
    for (int i = 0; i < 5; i++) CHECK(Entrada::confirmar("Seguro"));
    CHECK_CONTAINS(c.texto(), "Seguro (s/n): ");
}

TEST(Entrada_confirmar_respuestasNegativas) {
    Consola c("n\nN\nno\nNO\n");
    for (int i = 0; i < 4; i++) CHECK_FALSE(Entrada::confirmar("Seguro"));
}

TEST(Entrada_confirmar_repiteSiRespuestaInvalida) {
    Consola c("quizas\n\n1\ns\n");
    CHECK(Entrada::confirmar("Seguro"));
    CHECK_CONTAINS(c.texto(), "Error: responda 's' o 'n'.");
}

TEST(Entrada_confirmar_entradaCerradaLanzaExcepcion) {
    Consola c("");
    CHECK_THROWS(Entrada::confirmar("Seguro"));
}
