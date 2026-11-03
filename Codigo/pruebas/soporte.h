#ifndef SOPORTE_H
#define SOPORTE_H

// Utilidades compartidas por las pruebas: simulacion de teclado/pantalla,
// archivos temporales y control del reloj.

#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "../Reloj.h"

// Reemplaza std::cin por un texto fijo y captura todo lo que se escribe en std::cout.
// Al destruirse restaura ambos flujos.
class Consola {
    std::istringstream entrada;
    std::ostringstream salida;
    std::streambuf* cinOriginal;
    std::streambuf* coutOriginal;

public:
    explicit Consola(const std::string& teclas = "")
        : entrada(teclas), cinOriginal(std::cin.rdbuf(entrada.rdbuf())),
          coutOriginal(std::cout.rdbuf(salida.rdbuf())) {
        std::cin.clear();
    }
    ~Consola() {
        std::cin.rdbuf(cinOriginal);
        std::cout.rdbuf(coutOriginal);
        std::cin.clear();
    }
    std::string texto() const { return salida.str(); }
};

// Archivos de datos temporales con prefijo propio (no toca los archivos reales).
struct ArchivosTemporales {
    static const char* prefijo() { return "ut_"; }

    ArchivosTemporales() { borrar(); }
    ~ArchivosTemporales() { borrar(); }

    static void borrar() {
        const char* nombres[] = { "profesores.txt", "materias.txt", "cursos.txt", "asistencias.txt" };
        for (int i = 0; i < 4; i++) std::remove((std::string(prefijo()) + nombres[i]).c_str());
    }
    static void escribir(const std::string& nombre, const std::string& contenido) {
        std::ofstream f((std::string(prefijo()) + nombre).c_str(), std::ios::binary);
        f << contenido;
    }
    static std::string leer(const std::string& nombre) {
        std::ifstream f((std::string(prefijo()) + nombre).c_str(), std::ios::binary);
        std::stringstream ss;
        ss << f.rdbuf();
        return ss.str();
    }
    static bool existe(const std::string& nombre) {
        std::ifstream f((std::string(prefijo()) + nombre).c_str());
        return f.is_open();
    }
};

// Fija el reloj simulado y lo restaura (hora real) al salir del bloque.
struct RelojFijo {
    RelojFijo(const std::string& fecha, int hora, int minuto) { Reloj::simular(fecha, hora * 60 + minuto); }
    ~RelojFijo() { Reloj::detenerSimulacion(); }
};

// Datos de ejemplo usados en varias pruebas
const char* const CEDULA_1 = "0605293885";
const char* const CEDULA_2 = "1710034065";
const char* const LUNES_FECHA = "2026-11-02";    // lunes
const char* const MARTES_FECHA = "2026-11-03";   // martes
const char* const SABADO_FECHA = "2026-11-07";   // sabado

#endif
