#include "mini_test.h"
#include "soporte.h"
#include "../Entrada.h"
#include <stdexcept>

// Una prueba por funcion.

TEST(Entrada_leerLinea) {
    {   // muestra el mensaje y recorta espacios
        Consola c("   hola mundo  \n");
        CHECK_EQ(Entrada::leerLinea("Nombre: "), "hola mundo");
        CHECK_CONTAINS(c.texto(), "Nombre: ");
    }
    {   // una linea vacia es valida
        Consola c("\n");
        CHECK_EQ(Entrada::leerLinea("x"), "");
    }
    {   // entrada cerrada (Ctrl+Z / Ctrl+D): excepcion
        Consola c("");
        CHECK_THROWS(Entrada::leerLinea("x"));
    }
}

TEST(Entrada_leerEntero) {
    {   // valor valido
        Consola c("5\n");
        CHECK_EQ(Entrada::leerEntero("n: ", 1, 10), 5);
    }
    {   // limites inclusivos
        Consola c("1\n10\n0\n");
        CHECK_EQ(Entrada::leerEntero("n: ", 1, 10), 1);
        CHECK_EQ(Entrada::leerEntero("n: ", 1, 10), 10);
        CHECK_EQ(Entrada::leerEntero("n: ", 0, 3), 0);
    }
    {   // repite hasta recibir un valor valido
        Consola c("abc\n\n-3\n11\n0\n3.5\n1234567890\n7\n");
        CHECK_EQ(Entrada::leerEntero("n: ", 1, 10), 7);
        std::string s = c.texto();
        CHECK_CONTAINS(s, "ingrese un numero entero entre 1 y 10");   // abc, vacio, -3, 3.5, 10 digitos
        CHECK_CONTAINS(s, "el valor debe estar entre 1 y 10");        // 11 y 0
    }
    {   // entrada cerrada
        Consola c("abc\n");
        CHECK_THROWS(Entrada::leerEntero("n: ", 1, 10));
    }
}

TEST(Entrada_confirmar) {
    {   // afirmativas
        Consola c("s\nS\nsi\nSI\nSi\n");
        for (int i = 0; i < 5; i++) CHECK(Entrada::confirmar("Seguro"));
        CHECK_CONTAINS(c.texto(), "Seguro (s/n): ");
    }
    {   // negativas
        Consola c("n\nN\nno\nNO\n");
        for (int i = 0; i < 4; i++) CHECK_FALSE(Entrada::confirmar("Seguro"));
    }
    {   // repite si la respuesta es invalida
        Consola c("quizas\n\n1\ns\n");
        CHECK(Entrada::confirmar("Seguro"));
        CHECK_CONTAINS(c.texto(), "responda 's' o 'n'.");
    }
    {   // entrada cerrada
        Consola c("");
        CHECK_THROWS(Entrada::confirmar("Seguro"));
    }
}
