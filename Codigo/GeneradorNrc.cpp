#include "GeneradorNrc.h"
#include "validacion.h"

std::string GeneradorNrc::siguiente(const ListaDoble<Aula>& cursos) {
    int mayor = 0;
    cursos.recorrer([&mayor](const Aula& a) {
        if (Validacion::validarNrc(a.getNrc())) {
            int valor = std::stoi(a.getNrc());
            if (valor > mayor) mayor = valor;
        }
    });

    int candidato = (mayor < NRC_INICIAL) ? NRC_INICIAL : mayor + 1;
    if (candidato > NRC_MAXIMO) return "";
    return std::to_string(candidato);
}
