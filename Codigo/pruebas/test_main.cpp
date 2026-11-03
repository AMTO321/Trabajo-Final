#include "mini_test.h"

// Uso: ./pruebas [filtro]   -> ejecuta solo las pruebas cuyo nombre contenga el filtro
int main(int argc, char** argv) {
    std::string filtro = (argc > 1) ? argv[1] : "";
    return mt::ejecutarTodas(filtro);
}
