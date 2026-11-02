#include "mini_test.h"
#include "validacion.h"
#include <string>

// ---------- soloNumeros ----------

TEST_CASE("soloNumeros: acepta cadenas formadas solo por digitos") {
    CHECK(Validacion::soloNumeros("12345"));
    CHECK(Validacion::soloNumeros("0"));
    CHECK(Validacion::soloNumeros("007"));
}

TEST_CASE("soloNumeros: rechaza la cadena vacia") {
    CHECK(!Validacion::soloNumeros(""));
}

TEST_CASE("soloNumeros: rechaza letras, signos, puntos y espacios") {
    CHECK(!Validacion::soloNumeros("12a45"));
    CHECK(!Validacion::soloNumeros("abc"));
    CHECK(!Validacion::soloNumeros("-5"));
    CHECK(!Validacion::soloNumeros("1.5"));
    CHECK(!Validacion::soloNumeros(" 123"));
    CHECK(!Validacion::soloNumeros("123 "));
    CHECK(!Validacion::soloNumeros("12 3"));
}

// ---------- soloLetras ----------

TEST_CASE("soloLetras: acepta letras y espacios entre palabras") {
    CHECK(Validacion::soloLetras("Juan"));
    CHECK(Validacion::soloLetras("Juan Carlos"));
    CHECK(Validacion::soloLetras("adrian michael"));
}

TEST_CASE("soloLetras: rechaza la cadena vacia") {
    CHECK(!Validacion::soloLetras(""));
}

TEST_CASE("soloLetras: rechaza digitos y signos") {
    CHECK(!Validacion::soloLetras("Juan1"));
    CHECK(!Validacion::soloLetras("12"));
    CHECK(!Validacion::soloLetras("Juan-Carlos"));
    CHECK(!Validacion::soloLetras("Juan_Carlos"));
    CHECK(!Validacion::soloLetras("Juan.")); 
}

DEFECT_CASE("[DEFECTO] soloLetras rechaza una cadena que solo tiene espacios") {
    CHECK(!Validacion::soloLetras("   "));
}

// ---------- validarCedula: formato ----------

TEST_CASE("validarCedula: rechaza cedulas con longitud distinta de 10") {
    CHECK(!Validacion::validarCedula(""));
    CHECK(!Validacion::validarCedula("060529388"));     // 9 digitos
    CHECK(!Validacion::validarCedula("06052938851"));   // 11 digitos
}

TEST_CASE("validarCedula: rechaza caracteres que no son digitos") {
    CHECK(!Validacion::validarCedula("060529388a"));
    CHECK(!Validacion::validarCedula("0605 93885"));
    CHECK(!Validacion::validarCedula(" 605293885"));
    CHECK(!Validacion::validarCedula("06-5293885"));
}

// ---------- validarCedula: validez ----------

TEST_CASE("validarCedula: acepta la cedula real usada en profesores.txt") {
    CHECK(Validacion::validarCedula("0605293885"));
}

TEST_CASE("validarCedula: acepta cedulas validas de distintas provincias") {
    // Digitos verificadores calculados por separado (fuera del programa).
    CHECK(Validacion::validarCedula("0101234565"));  // provincia 01
    CHECK(Validacion::validarCedula("0901234567"));  // provincia 09
    CHECK(Validacion::validarCedula("1701234567"));  // provincia 17
    CHECK(Validacion::validarCedula("1710034065"));  // provincia 17
    CHECK(Validacion::validarCedula("2401234568"));  // provincia 24 (limite superior)
    CHECK(Validacion::validarCedula("3001234560"));  // provincia 30 (ecuatorianos en el exterior)
}

TEST_CASE("validarCedula: acepta cuando el digito verificador es 0 (residuo 0)") {
    CHECK(Validacion::validarCedula("1700000050"));
    CHECK(Validacion::validarCedula("3001234560"));
}

TEST_CASE("validarCedula: rechaza si el digito verificador es incorrecto") {
    // Para cada prefijo de 9 digitos solo UN digito final (de 0 a 9) puede ser valido.
    int validos = 0;
    for (int d = 0; d <= 9; ++d) {
        if (Validacion::validarCedula("060529388" + std::to_string(d))) ++validos;
    }
    CHECK_EQ(validos, 1);
    CHECK(!Validacion::validarCedula("0605293884"));
    CHECK(!Validacion::validarCedula("0605293886"));
}

TEST_CASE("validarCedula: rechaza un cambio en cualquier digito de una cedula valida") {
    std::string valida = "0605293885";
    for (size_t i = 0; i < 9; ++i) {
        std::string alterada = valida;
        alterada[i] = (alterada[i] == '9') ? '8' : static_cast<char>(alterada[i] + 1);
        // Un cambio de 1 en un digito siempre altera la suma modulo 10 (o rompe provincia/tercer digito).
        CHECK(!Validacion::validarCedula(alterada));
    }
}

TEST_CASE("validarCedula: rechaza provincias fuera de 01-24 y distintas de 30") {
    // Todas con digito verificador correcto: solo la provincia las hace invalidas.
    CHECK(!Validacion::validarCedula("0001234566"));  // 00
    CHECK(!Validacion::validarCedula("2501234567"));  // 25
    CHECK(!Validacion::validarCedula("3101234569"));  // 31
    CHECK(!Validacion::validarCedula("9901234568"));  // 99
}

TEST_CASE("validarCedula: el tercer digito debe ser menor que 6") {
    CHECK(Validacion::validarCedula("1751234566"));   // tercer digito 5 -> valido
    CHECK(!Validacion::validarCedula("1761234564"));  // tercer digito 6, DV correcto -> rechazada
}
