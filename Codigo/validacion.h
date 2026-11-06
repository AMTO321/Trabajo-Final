#ifndef VALIDACION_H
#define VALIDACION_H

#include <string>
#include <vector>

class Validacion {
public:
   
    static std::string trim(const std::string& texto);
    static std::string normalizarEspacios(const std::string& texto); 
    static std::string aMayusculas(const std::string& texto);
    static std::vector<std::string> dividir(const std::string& linea, char separador);

    
    static bool soloNumeros(const std::string& texto);
    static bool soloLetras(const std::string& texto);          
    static bool validarNombrePersona(const std::string& texto);
    static bool validarTextoLibre(const std::string& texto, size_t minimo, size_t maximo);
                                                                
    static bool validarCedula(const std::string& cedula);     

    // Validaciones del dominio academico
    static bool validarIdMateria(const std::string& id);       
    static bool validarNrc(const std::string& nrc);            


    static bool validarFecha(const std::string& fecha);        
    static bool parsearHora(const std::string& texto, int& minutos); 
    static bool parsearHoraFlexible(const std::string& texto, int& minutos);
};

#endif
