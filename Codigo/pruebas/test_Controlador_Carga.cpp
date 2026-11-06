#include "mini_test.h"
#include "fixture.h"

// Una prueba por funcion publica. La carga/guardado de archivos y las funciones
// privadas se prueban una por una en test_Controlador_Privadas.cpp.

TEST(Controlador_constructor) {   // carga los cuatro archivos al iniciar
    ArchivosTemporales tmp;
    {   // sin archivos: todo vacio
        Controlador c = crear();
        CHECK(c.getProfesores().estaVacia());
        CHECK(c.getMaterias().estaVacia());
        CHECK(c.getCursos().estaVacia());
        CHECK(c.getRegistros().estaVacia());
    }
    {   // datos validos
        datosBasicos();
        ArchivosTemporales::escribir("asistencias.txt", "2026-11-02,07:05,0605293885,10001,ENTRADA,PUNTUAL\n");
        Controlador c = crear();
        CHECK_EQ(c.getProfesores().getTamanio(), 2u);
        CHECK_EQ(c.getMaterias().getTamanio(), 3u);
        CHECK_EQ(c.getCursos().getTamanio(), 3u);
        CHECK_EQ(c.getRegistros().getTamanio(), 1u);
        const Aula* a = buscarCurso(c, "10002");
        CHECK(a != nullptr);
        if (a) {
            CHECK_EQ(a->getIdMateria(), "EDIF201");
            CHECK_EQ(a->getCedulaProfesor(), "0605293885");
            CHECK_EQ(a->getDia(), LUNES);
            CHECK_EQ(a->getHorario().toString(), "09:00 - 11:00");
        }
    }
    {   // el prefijo separa los archivos: otro prefijo no ve estos datos
        Controlador otro("otro_prefijo_inexistente_");
        CHECK(otro.getProfesores().estaVacia());
        CHECK(otro.getCursos().estaVacia());
    }
    {   // los cursos solo se cargan si su profesor y su materia existen
        ArchivosTemporales::borrar();
        ArchivosTemporales::escribir("cursos.txt", "10001,ALGE101,0605293885,LUNES,420,540\n");
        Consola consola;
        Controlador c = crear();
        CHECK(c.getCursos().estaVacia());
        CHECK_CONTAINS(consola.texto(), "Aviso: 1 linea invalida");
    }
}

TEST(Controlador_getProfesores) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    CHECK_EQ(c.getProfesores().getTamanio(), 2u);
    CHECK_EQ(c.getProfesores().getCabeza()->getDato().getCedula(), "0605293885");
    CHECK_EQ(c.getProfesores().getCola()->getDato().getCedula(), "1710034065");
}

TEST(Controlador_getMaterias) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    CHECK_EQ(c.getMaterias().getTamanio(), 3u);
    CHECK_EQ(c.getMaterias().getCabeza()->getDato().getId(), "ALGE101");
    CHECK_EQ(c.getMaterias().getCola()->getDato().getId(), "EDIF201");
}

TEST(Controlador_getCursos) {
    ArchivosTemporales tmp;
    datosBasicos();
    Controlador c = crear();
    CHECK_EQ(c.getCursos().getTamanio(), 3u);
    CHECK_EQ(c.getCursos().getCabeza()->getDato().getNrc(), "10001");
    CHECK_EQ(c.getCursos().getCola()->getDato().getNrc(), "10003");
}

TEST(Controlador_getRegistros) {
    ArchivosTemporales tmp;
    datosBasicos();
    ArchivosTemporales::escribir("asistencias.txt", "2026-11-02,07:05,0605293885,10001,ENTRADA,PUNTUAL\n");
    Controlador c = crear();
    CHECK_EQ(c.getRegistros().getTamanio(), 1u);
    CHECK_EQ(c.getRegistros().getCabeza()->getDato().getNrc(), "10001");
}
