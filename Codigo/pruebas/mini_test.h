#ifndef MINI_TEST_H
#define MINI_TEST_H

// Framework de pruebas minimo (un solo archivo, sin dependencias externas).
//   TEST(nombre) { ... }          define una prueba
//   CHECK(cond)                   verifica que cond sea verdadera
//   CHECK_FALSE(cond)             verifica que cond sea falsa
//   CHECK_EQ(a, b)                verifica a == b (muestra ambos valores si falla)
//   CHECK_CONTAINS(texto, sub)    verifica que texto contenga sub
//   CHECK_NOT_CONTAINS(texto, sub)
//   CHECK_THROWS(expr)            verifica que expr lance una excepcion
// Una verificacion fallida NO detiene la prueba: se reporta y se continua.

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace mt {

struct Caso {
    const char* nombre;
    void (*funcion)();
};

inline std::vector<Caso>& casos() {
    static std::vector<Caso> lista;
    return lista;
}

inline std::vector<std::string>& fallosActuales() {
    static std::vector<std::string> lista;
    return lista;
}

inline int& verificaciones() {
    static int total = 0;
    return total;
}

struct Registro {
    Registro(const char* nombre, void (*f)()) {
        Caso c = { nombre, f };
        casos().push_back(c);
    }
};

inline void fallar(const char* archivo, int linea, const std::string& mensaje) {
    std::ostringstream ss;
    ss << archivo << ":" << linea << ": " << mensaje;
    fallosActuales().push_back(ss.str());
}

template <typename A, typename B>
void comprobarIgual(const char* archivo, int linea, const char* ea, const char* eb,
                    const A& a, const B& b) {
    verificaciones()++;
    if (!(a == b)) {
        std::ostringstream ss;
        ss << "CHECK_EQ(" << ea << ", " << eb << ") fallo: [" << a << "] != [" << b << "]";
        fallar(archivo, linea, ss.str());
    }
}

inline void comprobarContiene(const char* archivo, int linea, const std::string& texto,
                              const std::string& sub, bool debeContener) {
    verificaciones()++;
    bool contiene = texto.find(sub) != std::string::npos;
    if (contiene != debeContener) {
        std::ostringstream ss;
        ss << (debeContener ? "se esperaba encontrar" : "NO se esperaba encontrar")
           << " [" << sub << "] en la salida:\n----\n" << texto << "\n----";
        fallar(archivo, linea, ss.str());
    }
}

inline void comprobar(const char* archivo, int linea, const char* expr, bool valor, bool esperado) {
    verificaciones()++;
    if (valor != esperado) {
        fallar(archivo, linea, std::string(esperado ? "CHECK(" : "CHECK_FALSE(") + expr + ") fallo");
    }
}

inline int ejecutarTodas(const std::string& filtro) {
    int ok = 0, mal = 0, omitidas = 0;
    std::vector<std::string> nombresFallidos;
    for (size_t i = 0; i < casos().size(); i++) {
        const Caso& c = casos()[i];
        if (!filtro.empty() && std::string(c.nombre).find(filtro) == std::string::npos) { omitidas++; continue; }
        fallosActuales().clear();
        try {
            c.funcion();
        } catch (const std::exception& e) {
            fallar("(excepcion)", 0, std::string("excepcion no esperada: ") + e.what());
        } catch (...) {
            fallar("(excepcion)", 0, "excepcion desconocida");
        }
        if (fallosActuales().empty()) {
            ok++;
            std::cout << "[  OK  ] " << c.nombre << "\n";
        } else {
            mal++;
            nombresFallidos.push_back(c.nombre);
            std::cout << "[FALLO ] " << c.nombre << "\n";
            for (size_t k = 0; k < fallosActuales().size(); k++)
                std::cout << "         " << fallosActuales()[k] << "\n";
        }
    }
    std::cout << "\n=====================================================\n"
              << "Pruebas: " << (ok + mal) << " ejecutadas | " << ok << " correctas | " << mal << " con fallos";
    if (omitidas) std::cout << " | " << omitidas << " omitidas por el filtro";
    std::cout << "\nVerificaciones totales: " << verificaciones() << "\n";
    if (mal) {
        std::cout << "Fallidas:\n";
        for (size_t i = 0; i < nombresFallidos.size(); i++) std::cout << "  - " << nombresFallidos[i] << "\n";
    }
    std::cout << "=====================================================\n";
    return mal == 0 ? 0 : 1;
}

} // namespace mt

#define TEST(nombre) \
    static void nombre(); \
    static mt::Registro registro_##nombre(#nombre, nombre); \
    static void nombre()

#define CHECK(cond)        mt::comprobar(__FILE__, __LINE__, #cond, (cond), true)
#define CHECK_FALSE(cond)  mt::comprobar(__FILE__, __LINE__, #cond, (cond), false)
#define CHECK_EQ(a, b)     mt::comprobarIgual(__FILE__, __LINE__, #a, #b, (a), (b))
#define CHECK_CONTAINS(texto, sub)      mt::comprobarContiene(__FILE__, __LINE__, (texto), (sub), true)
#define CHECK_NOT_CONTAINS(texto, sub)  mt::comprobarContiene(__FILE__, __LINE__, (texto), (sub), false)
#define CHECK_THROWS(expr) \
    do { bool lanzo_ = false; try { expr; } catch (...) { lanzo_ = true; } \
         mt::comprobar(__FILE__, __LINE__, "lanza excepcion: " #expr, lanzo_, true); } while (0)

#endif
