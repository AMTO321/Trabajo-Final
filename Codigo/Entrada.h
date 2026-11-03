#ifndef ENTRADA_H
#define ENTRADA_H

#include <string>

// Lectura segura desde teclado. Todo se lee con getline (nunca se mezcla
// cin >> con getline) y se detecta el cierre de la entrada (Ctrl+Z / Ctrl+D).
class Entrada {
public:
    // Lanza std::runtime_error si la entrada se cerro.
    static std::string leerLinea(const std::string& mensaje);
    // Repite hasta recibir un entero en [minimo, maximo].
    static int leerEntero(const std::string& mensaje, int minimo, int maximo);
    // Repite hasta recibir s/n.
    static bool confirmar(const std::string& mensaje);
};

#endif
