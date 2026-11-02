// mini_test.h - Mini framework de pruebas unitarias de un solo archivo.
// Los nombres de las macros (TEST_CASE, CHECK, CHECK_EQ...) imitan a doctest,
// para poder migrar a doctest/Catch2 mas adelante casi sin cambios.
//
//  TEST_CASE("nombre")   -> prueba normal: si falla, el programa de pruebas falla.
//  DEFECT_CASE("nombre") -> prueba que describe el comportamiento CORRECTO esperado
//                           de un defecto conocido del codigo. Mientras el defecto
//                           exista se informa como DEFECTO CONFIRMADO (no rompe la
//                           ejecucion). Cuando se corrige el codigo pasa a
//                           DEFECTO CORREGIDO y conviene cambiarla a TEST_CASE.
#ifndef MINI_TEST_H
#define MINI_TEST_H

#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

namespace mini {

struct Caso {
    std::string nombre;
    std::function<void()> cuerpo;
    bool esDefecto;
};

inline std::vector<Caso>& casos() {
    static std::vector<Caso> v;
    return v;
}

struct Registro {
    Registro(const char* nombre, std::function<void()> cuerpo, bool esDefecto) {
        casos().push_back({nombre, cuerpo, esDefecto});
    }
};

struct AbortarPrueba {};  // se lanza con REQUIRE cuando la condicion falla

inline std::vector<std::string>& mensajes() {
    static std::vector<std::string> m;
    return m;
}

inline void registrarFallo(const std::string& detalle, const char* archivo, int linea) {
    std::ostringstream os;
    os << "      " << archivo << ":" << linea << "  " << detalle;
    mensajes().push_back(os.str());
}

template <typename T>
std::string texto(const T& v) {
    std::ostringstream os;
    if constexpr (std::is_enum<T>::value) {
        os << static_cast<long long>(v);
    } else {
        os << v;
    }
    return os.str();
}
inline std::string texto(const std::string& v) { return "\"" + v + "\""; }
inline std::string texto(const char* v) { return std::string("\"") + v + "\""; }
inline std::string texto(bool v) { return v ? "true" : "false"; }

template <typename A, typename B>
bool iguales(const A& a, const B& b) {
    if constexpr (std::is_integral<A>::value && std::is_integral<B>::value) {
        return static_cast<long long>(a) == static_cast<long long>(b);
    } else {
        return a == b;
    }
}

template <typename A, typename B>
bool comprobarIgual(const A& a, const B& b, const char* ea, const char* eb,
                    const char* archivo, int linea) {
    if (iguales(a, b)) return true;
    registrarFallo(std::string("CHECK_EQ(") + ea + ", " + eb + ")  obtenido: " +
                       texto(a) + "  esperado: " + texto(b),
                   archivo, linea);
    return false;
}

inline bool comprobar(bool cond, const char* expr, const char* archivo, int linea) {
    if (cond) return true;
    registrarFallo(std::string("CHECK(") + expr + ") es falso", archivo, linea);
    return false;
}

// Redirige std::cout a un buffer mientras el objeto exista.
class CapturaSalida {
    std::ostringstream buffer_;
    std::streambuf* original_;
public:
    CapturaSalida() : original_(std::cout.rdbuf(buffer_.rdbuf())) {}
    ~CapturaSalida() { std::cout.rdbuf(original_); }
    std::string texto() const { return buffer_.str(); }
};

// Reemplaza std::cin por un texto prefabricado mientras el objeto exista.
class EntradaSimulada {
    std::istringstream flujo_;
    std::streambuf* original_;
public:
    explicit EntradaSimulada(const std::string& contenido)
        : flujo_(contenido), original_(std::cin.rdbuf(flujo_.rdbuf())) {
        std::cin.clear();
    }
    ~EntradaSimulada() {
        std::cin.rdbuf(original_);
        std::cin.clear();
    }
};

inline int correrTodas() {
    int ok = 0, fallidas = 0, defectosConfirmados = 0, defectosCorregidos = 0;
    std::vector<std::string> listaDefectos;
    for (auto& c : casos()) {
        mensajes().clear();
        std::string excepcion;
        try {
            c.cuerpo();
        } catch (const AbortarPrueba&) {
        } catch (const std::exception& e) {
            excepcion = std::string("excepcion inesperada: ") + e.what();
        } catch (...) {
            excepcion = "excepcion inesperada (tipo desconocido)";
        }
        bool fallo = !mensajes().empty() || !excepcion.empty();
        if (c.esDefecto) {
            if (fallo) {
                ++defectosConfirmados;
                listaDefectos.push_back(c.nombre);
                std::cout << "[DEFECTO CONFIRMADO] " << c.nombre << "\n";
                for (auto& m : mensajes()) std::cout << m << "\n";
                if (!excepcion.empty()) std::cout << "      " << excepcion << "\n";
            } else {
                ++defectosCorregidos;
                std::cout << "[DEFECTO CORREGIDO]  " << c.nombre
                          << "  (cambiar a TEST_CASE)\n";
            }
        } else if (fallo) {
            ++fallidas;
            std::cout << "[FALLA] " << c.nombre << "\n";
            for (auto& m : mensajes()) std::cout << m << "\n";
            if (!excepcion.empty()) std::cout << "      " << excepcion << "\n";
        } else {
            ++ok;
            std::cout << "[OK]    " << c.nombre << "\n";
        }
    }
    std::cout << "\n=========================================\n"
              << "Pruebas normales : " << ok << " correctas, " << fallidas << " fallidas\n"
              << "Defectos         : " << defectosConfirmados << " confirmados, "
              << defectosCorregidos << " corregidos\n"
              << "=========================================\n";
    return fallidas == 0 ? 0 : 1;
}

}  // namespace mini

#define MT_UNIR2(a, b) a##b
#define MT_UNIR(a, b) MT_UNIR2(a, b)

#define MT_DEFINIR(nombre, esDefecto)                                              \
    static void MT_UNIR(mt_fn_, __LINE__)();                                       \
    static mini::Registro MT_UNIR(mt_reg_, __LINE__)(nombre, MT_UNIR(mt_fn_, __LINE__), esDefecto); \
    static void MT_UNIR(mt_fn_, __LINE__)()

#define TEST_CASE(nombre) MT_DEFINIR(nombre, false)
#define DEFECT_CASE(nombre) MT_DEFINIR(nombre, true)

#define CHECK(cond) mini::comprobar(static_cast<bool>(cond), #cond, __FILE__, __LINE__)
#define REQUIRE(cond) do { if (!mini::comprobar(static_cast<bool>(cond), #cond, __FILE__, __LINE__)) throw mini::AbortarPrueba(); } while (0)
#define CHECK_EQ(a, b) mini::comprobarIgual((a), (b), #a, #b, __FILE__, __LINE__)
#define REQUIRE_EQ(a, b) do { if (!mini::comprobarIgual((a), (b), #a, #b, __FILE__, __LINE__)) throw mini::AbortarPrueba(); } while (0)

#define CHECK_THROWS(expr)                                                         \
    do { bool mt_lanzo = false;                                                    \
         try { (void)(expr); } catch (...) { mt_lanzo = true; }                    \
         mini::comprobar(mt_lanzo, "se esperaba excepcion en: " #expr, __FILE__, __LINE__); \
    } while (0)

#define CHECK_NOTHROW(expr)                                                        \
    do { try { expr; } catch (const std::exception& mt_e) {                        \
             mini::registrarFallo(std::string("no se esperaba excepcion en: " #expr ", pero se lanzo: ") + mt_e.what(), __FILE__, __LINE__); \
         } catch (...) {                                                           \
             mini::registrarFallo("no se esperaba excepcion en: " #expr, __FILE__, __LINE__); } \
    } while (0)

#endif
