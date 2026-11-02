// entorno_prueba.h - Utilidades para pruebas que usan archivos de texto.
#ifndef ENTORNO_PRUEBA_H
#define ENTORNO_PRUEBA_H

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

// Crea una carpeta temporal vacia, entra en ella y, al destruirse, vuelve a la
// carpeta original y la borra. Asi las pruebas nunca tocan tus archivos reales
// (profesores.txt, cursos.txt, asistencias.txt).
class DirectorioTemporal {
    std::filesystem::path original_;
    std::filesystem::path temporal_;
public:
    DirectorioTemporal() : original_(std::filesystem::current_path()) {
        static int contador = 0;
        temporal_ = std::filesystem::temp_directory_path() /
                    ("pruebas_asistencia_" + std::to_string(++contador) + "_" +
                     std::to_string(reinterpret_cast<std::size_t>(this)));
        std::filesystem::remove_all(temporal_);
        std::filesystem::create_directories(temporal_);
        std::filesystem::current_path(temporal_);
    }
    ~DirectorioTemporal() {
        std::filesystem::current_path(original_);
        std::error_code ec;
        std::filesystem::remove_all(temporal_, ec);
    }
};

inline void escribirArchivo(const std::string& nombre, const std::string& contenido) {
    std::ofstream f(nombre, std::ios::trunc);
    f << contenido;
}

inline std::string leerArchivo(const std::string& nombre) {
    std::ifstream f(nombre);
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

inline bool existeArchivo(const std::string& nombre) {
    return std::filesystem::exists(nombre);
}

#endif
