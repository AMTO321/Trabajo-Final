#ifndef VALIDACION_H
#define VALIDACION_H

#include <string>
#include <vector>

class Validacion {
public:
    // Utilidades de texto
    static std::string trim(const std::string& texto);
    static std::string normalizarEspacios(const std::string& texto); // trim + colapsa espacios dobles
    static std::string aMayusculas(const std::string& texto);
    static std::vector<std::string> dividir(const std::string& linea, char separador);

    // Validaciones generales
    static bool soloNumeros(const std::string& texto);
    static bool soloLetras(const std::string& texto);          // letras (con tildes/enie) y espacios
    static bool validarNombrePersona(const std::string& texto);// soloLetras + 2..50 caracteres
    static bool validarTextoLibre(const std::string& texto, size_t minimo, size_t maximo);
                                                                // letras, numeros, espacios; sin comas
    static bool validarCedula(const std::string& cedula);      // cedula ecuatoriana (modulo 10)

    // Validaciones del dominio academico
    static bool validarIdMateria(const std::string& id);       // 4..10 letras/numeros en mayuscula
    static bool validarNrc(const std::string& nrc);            // 5 digitos, 10001..99999

    // Fechas y horas
    static bool validarFecha(const std::string& fecha);        // AAAA-MM-DD real (con bisiestos)
    static bool parsearHora(const std::string& texto, int& minutos); // "HH:MM" -> minutos
};

#endif
