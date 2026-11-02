#include "RegistroAsistencia.h"
#include <iostream>
#include <fstream>
#include <ctime>

RegistroAsistencia::RegistroAsistencia() : cedula(""), nrc(""), estado(""), hora("") {}

RegistroAsistencia::RegistroAsistencia(std::string cedula, std::string nrc, std::string estado, std::string hora)
    : cedula(cedula), nrc(nrc), estado(estado), hora(hora) {}

void RegistroAsistencia::registrarIngreso(std::string cedulaProfesor, std::string nrcCurso) {
    cedula = cedulaProfesor;
    nrc = nrcCurso;

    // Obtener la hora automáticamente del sistema
    time_t ahora = time(0);
    tm* tiempoLocal = localtime(&ahora);
    int horaIngreso = tiempoLocal->tm_hour;   
    int minutoIngreso = tiempoLocal->tm_min; 

    int horaActualMinutos = (horaIngreso * 60) + minutoIngreso;
    hora = (horaIngreso < 10 ? "0" : "") + std::to_string(horaIngreso) + ":" + (minutoIngreso < 10 ? "0" : "") + std::to_string(minutoIngreso);

    // Validar franja horaria (07:00 a 21:00 -> 420 a 1259 minutos)
    if (horaActualMinutos < 420 || horaActualMinutos > 1259) {
        std::cout << "Fuera del horario institucional de clases (07:00 a 21:00).\n";
        estado = "FUERA_DE_HORA";
        return;
    }

    // Cálculo de la franja horaria de 2 horas
    int inicioFranja = 420 + ((horaActualMinutos - 420) / 120) * 120;
    int finFranja = inicioFranja + 119;

    // VALIDACIÓN DE CRUCE DE CLASES: Revisar si ya registró asistencia en este mismo bloque
    std::ifstream archivoLectura("asistencias.txt");
    if (archivoLectura.is_open()) {
        std::string linea;
        while (std::getline(archivoLectura, linea)) {
            if (linea.find("Cedula: " + cedula) != std::string::npos) {
                size_t posHora = linea.find("Hora: ");
                if (posHora != std::string::npos) {
                    std::string horaRegStr = linea.substr(posHora + 6, 5); 
                    size_t dosPuntos = horaRegStr.find(':');
                    int hReg = std::stoi(horaRegStr.substr(0, dosPuntos));
                    int mReg = std::stoi(horaRegStr.substr(dosPuntos + 1));
                    int minRegTotal = (hReg * 60) + mReg;

                    if (minRegTotal >= inicioFranja && minRegTotal <= finFranja) {
                        std::cout << "Error de Cruce: Ya tienes una asistencia registrada en esta misma franja horaria para otro curso.\n";
                        archivoLectura.close();
                        return;
                    }
                }
            }
        }
        archivoLectura.close();
    }

    // Validación de puntualidad (10 minutos de holgura)
    if (horaActualMinutos >= inicioFranja && horaActualMinutos <= inicioFranja + 10) {
        estado = "PUNTUAL";
        std::cout << "¡Registro exitoso! Estado: PUNTUAL (Dentro de los 10 min de holgura).\n";
    } else if (horaActualMinutos > inicioFranja + 10 && horaActualMinutos <= finFranja) {
        estado = "ATRASO";
        std::cout << "Registro exitoso con ATRASO (Fuera del margen de holgura).\n";
    } else {
        std::cout << "No se puede registrar asistencia. Fuera de la franja horaria.\n";
        return;
    }

    // Guardar automáticamente en el archivo asistencias.txt
    std::ofstream archivo("asistencias.txt", std::ios::app);
    if (archivo.is_open()) {
        archivo << toString() << "\n";
        archivo.close();
        std::cout << "El registro de asistencia se guardo en asistencias.txt correctamente.\n";
    }
}

void RegistroAsistencia::registrarSalida() {
    std::cout << "Salida registrada para el NRC: " << nrc << "\n";
}

std::string RegistroAsistencia::toString() const {
    return "Cedula: " + cedula + " | NRC: " + nrc + " | Estado: " + estado + " | Hora: " + hora;
}