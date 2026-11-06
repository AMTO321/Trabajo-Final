#include "mini_test.h"
#include "../GeneradorNrc.h"

// Una prueba por funcion.

static ListaDoble<Aula> cursosConNrc(std::initializer_list<const char*> nrcs) {
    ListaDoble<Aula> l;
    for (std::initializer_list<const char*>::iterator it = nrcs.begin(); it != nrcs.end(); ++it) {
        l.insertar(Aula(*it, "ALGE101", "0605293885", LUNES, IntervaloHorario::franja(1)));
    }
    return l;
}

TEST(GeneradorNrc_siguiente) {
    // sin cursos: empieza en NRC_INICIAL
    CHECK_EQ(GeneradorNrc::siguiente(ListaDoble<Aula>()), "1");
    // el siguiente es el mayor + 1, sin importar el orden ni los huecos
    CHECK_EQ(GeneradorNrc::siguiente(cursosConNrc({"1", "2", "3"})), "4");
    CHECK_EQ(GeneradorNrc::siguiente(cursosConNrc({"10001", "10002"})), "10003");
    CHECK_EQ(GeneradorNrc::siguiente(cursosConNrc({"50", "7", "20"})), "51");
    // ignora los NRC invalidos
    CHECK_EQ(GeneradorNrc::siguiente(cursosConNrc({"abc", "0123", "9", ""})), "10");
    CHECK_EQ(GeneradorNrc::siguiente(cursosConNrc({"abc"})), "1");
    // en el maximo ya no hay NRC disponibles
    CHECK_EQ(GeneradorNrc::siguiente(cursosConNrc({"999999999"})), "");
    CHECK_EQ(GeneradorNrc::siguiente(cursosConNrc({"999999998"})), "999999999");
}
