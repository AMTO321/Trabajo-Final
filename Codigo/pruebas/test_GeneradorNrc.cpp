#include "mini_test.h"
#include "../GeneradorNrc.h"
#include "../validacion.h"

static Aula curso(const std::string& nrc) {
    return Aula(nrc, "ALGE101", "0605293885", LUNES, IntervaloHorario::franja(1));
}

TEST(GeneradorNrc_sinCursos_empiezaEn10001) {
    ListaDoble<Aula> cursos;
    CHECK_EQ(GeneradorNrc::siguiente(cursos), "10001");
}

TEST(GeneradorNrc_incrementaElMasAlto) {
    ListaDoble<Aula> cursos;
    cursos.insertar(curso("10001"));
    CHECK_EQ(GeneradorNrc::siguiente(cursos), "10002");
    cursos.insertar(curso("10002"));
    cursos.insertar(curso("10003"));
    CHECK_EQ(GeneradorNrc::siguiente(cursos), "10004");
}

TEST(GeneradorNrc_conHuecos_usaMaxMasUno) {
    ListaDoble<Aula> cursos;
    cursos.insertar(curso("10001"));
    cursos.insertar(curso("10005"));
    cursos.insertar(curso("10003"));    // desordenado a proposito
    CHECK_EQ(GeneradorNrc::siguiente(cursos), "10006");
}

TEST(GeneradorNrc_nuncaReutilizaNrcEliminado) {
    ListaDoble<Aula> cursos;
    cursos.insertar(curso("10001"));
    cursos.insertar(curso("10002"));
    cursos.eliminar([](const Aula& a) { return a.getNrc() == "10001"; });
    CHECK_EQ(GeneradorNrc::siguiente(cursos), "10003");
}

TEST(GeneradorNrc_ignoraNrcInvalidos) {
    ListaDoble<Aula> cursos;
    cursos.insertar(curso("ABCDE"));
    cursos.insertar(curso("10000"));    // por debajo del inicial
    cursos.insertar(curso("123"));
    cursos.insertar(curso(""));
    CHECK_EQ(GeneradorNrc::siguiente(cursos), "10001");
}

TEST(GeneradorNrc_respetaNrcDeOtroRango) {
    ListaDoble<Aula> cursos;
    cursos.insertar(curso("20001"));    // datos heredados del esquema por periodos
    CHECK_EQ(GeneradorNrc::siguiente(cursos), "20002");
}

TEST(GeneradorNrc_limiteSuperior) {
    ListaDoble<Aula> cursos;
    cursos.insertar(curso("99998"));
    CHECK_EQ(GeneradorNrc::siguiente(cursos), "99999");
    cursos.insertar(curso("99999"));
    CHECK_EQ(GeneradorNrc::siguiente(cursos), "");      // secuencia agotada
}

TEST(GeneradorNrc_resultadoSiempreEsNrcValido) {
    ListaDoble<Aula> cursos;
    for (int i = 0; i < 50; i++) {
        std::string nrc = GeneradorNrc::siguiente(cursos);
        CHECK(Validacion::validarNrc(nrc));
        CHECK_FALSE(cursos.existe([&nrc](const Aula& a) { return a.getNrc() == nrc; }));  // sin duplicados
        cursos.insertar(curso(nrc));
    }
    CHECK_EQ(cursos.getTamanio(), 50u);
    CHECK_EQ(GeneradorNrc::siguiente(cursos), "10051");
}
