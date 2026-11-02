#include "controlador.h"
#include "validacion.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime> // Necesario para la hora automática del sistema

Controlador::Controlador() {
    cargarProfesores();
    cargarCursos();
}

// Función auxiliar robusta para leer números y evitar bucles infinitos en cin
int leerNumeroSeguro(const std::string& mensaje) {
    std::string entrada;
    while (true) {
        std::cout << mensaje;
        std::cin >> entrada;
        if (Validacion::soloNumeros(entrada)) {
            return std::stoi(entrada);
        }
        std::cout << "Error: Por favor ingrese solo numeros validos.\n";
    }
}

void Controlador::cargarProfesores() {
    std::ifstream archivo("profesores.txt");
    if (!archivo.is_open()) return;

    std::string linea;
    while (std::getline(archivo, linea)) {
        std::stringstream ss(linea);
        std::string cedula, nombres, apellidos;
        if (std::getline(ss, cedula, ',') &&
            std::getline(ss, nombres, ',') &&
            std::getline(ss, apellidos, ',')) {
            Profesor p(cedula, nombres, apellidos);
            profesores.insertar(p);
        }
    }
    archivo.close();
}

void Controlador::guardarProfesorArchivo(const Profesor& p) {
    std::ofstream archivo("profesores.txt", std::ios::app);
    if (archivo.is_open()) {
        archivo << p.getCedula() << "," << p.getNombres() << "," << p.getApellidos() << "\n";
        archivo.close();
    }
}

void Controlador::cargarCursos() {
    std::ifstream archivo("cursos.txt");
    if (!archivo.is_open()) return;

    std::string linea;
    while (std::getline(archivo, linea)) {
        std::stringstream ss(linea);
        std::string nrc, materia, cedulaProfesor;
        if (std::getline(ss, nrc, ',') &&
            std::getline(ss, materia, ',') &&
            std::getline(ss, cedulaProfesor, ',')) {
            Aula curso(nrc, materia, cedulaProfesor);
            cursos.insertar(curso);
        }
    }
    archivo.close();
}

void Controlador::guardarCursoArchivo(const Aula& a) {
    std::ofstream archivo("cursos.txt", std::ios::app);
    if (archivo.is_open()) {
        archivo << a.getNrc() << "," << a.getMateria() << "," << a.getCedulaProfesor() << "\n";
        archivo.close();
    }
}

void Controlador::guardarAsistenciaArchivo(const std::string& cedula, const std::string& nrc, const std::string& estado, const std::string& hora) {
    std::ofstream archivo("asistencias.txt", std::ios::app);
    if (archivo.is_open()) {
        archivo << "Cedula: " << cedula << " | NRC: " << nrc << " | Estado: " << estado << " | Hora: " << hora << "\n";
        archivo.close();
    }
}

void Controlador::registrarProfesor() {
    std::string cedula, nombres, apellidos;
    std::cout << "\n--- REGISTRAR NUEVO PROFESOR ---\n";
    
    while (true) {
        std::cout << "Ingrese numero de cedula (10 digitos): ";
        std::cin >> cedula;

        if (!Validacion::validarCedula(cedula)) {
            std::cout << "Error: La cedula ingresada no es valida en Ecuador.\n";
            continue;
        }

        bool existe = false;
        profesores.recorrer([&cedula, &existe](Profesor p) {
            if (p.getCedula() == cedula) existe = true;
        });

        if (existe) {
            std::cout << "Error: Ya existe un profesor registrado con esa cedula.\n";
        } else {
            break;
        }
    }

    while (true) {
        std::cout << "Ingrese nombres: ";
        std::cin.ignore();
        std::getline(std::cin, nombres);
        if (!Validacion::soloLetras(nombres)) {
            std::cout << "Error: El nombre solo debe contener letras.\n";
            continue;
        }
        break;
    }

    while (true) {
        std::cout << "Ingrese apellidos: ";
        std::getline(std::cin, apellidos);
        if (!Validacion::soloLetras(apellidos)) {
            std::cout << "Error: El apellido solo debe contener letras.\n";
            continue;
        }
        break;
    }

    Profesor nuevoProfesor(cedula, nombres, apellidos);
    profesores.insertar(nuevoProfesor);
    guardarProfesorArchivo(nuevoProfesor);
    std::cout << "¡Profesor registrado y guardado en profesores.txt con exito!\n";
}

void Controlador::asignarCursoANrc() {
    std::string nrc, materia, cedulaProfesor;
    std::cout << "\n--- ASIGNAR CURSO (NRC) A PROFESOR ---\n";

    // 1. Validar NRC único y numérico
    while (true) {
        std::cout << "Ingrese el NRC del curso (solo numeros): ";
        std::cin >> nrc;

        if (!Validacion::soloNumeros(nrc)) {
            std::cout << "Error: El NRC debe contener unicamente numeros.\n";
            continue;
        }

        bool nrcExiste = false;
        cursos.recorrer([&nrc, &nrcExiste](Aula a) {
            if (a.getNrc() == nrc) nrcExiste = true;
        });

        if (nrcExiste) {
            std::cout << "Error: Este NRC ya esta registrado y asignado a un curso previamente.\n";
        } else {
            break;
        }
    }

    // 2. Ingresar materia (solo letras)
    while (true) {
        std::cout << "Ingrese el nombre de la materia: ";
        std::cin.ignore();
        std::getline(std::cin, materia);

        if (!Validacion::soloLetras(materia)) {
            std::cout << "Error: La materia debe contener solo letras y espacios.\n";
            continue;
        }
        break;
    }

    // 3. Validar que el profesor exista por su cédula
    while (true) {
        std::cout << "Ingrese la cedula del profesor a asignar: ";
        std::cin >> cedulaProfesor;

        bool profesorExiste = false;
        profesores.recorrer([&cedulaProfesor, &profesorExiste](Profesor p) {
            if (p.getCedula() == cedulaProfesor) profesorExiste = true;
        });

        if (!profesorExiste) {
            std::cout << "Error: No existe un profesor registrado con esa cedula. Registrelo primero.\n";
            return;
        } else {
            break;
        }
    }

    Aula nuevoCurso(nrc, materia, cedulaProfesor);
    cursos.insertar(nuevoCurso);
    guardarCursoArchivo(nuevoCurso);
    std::cout << "¡Curso (NRC) asignado correctamente y guardado en cursos.txt!\n";
}

void Controlador::loginProfesor() {
    std::string cedula;
    std::cout << "\n=== LOGIN DE PROFESOR ===\n";
    std::cout << "Ingrese su numero de cedula: ";
    std::cin >> cedula;

    bool profesorEncontrado = false;
    Profesor profesorActual;
    profesores.recorrer([&cedula, &profesorEncontrado, &profesorActual](Profesor p) {
        if (p.getCedula() == cedula) {
            profesorEncontrado = true;
            profesorActual = p;
        }
    });

    if (!profesorEncontrado) {
        std::cout << "Acceso denegado: Profesor no encontrado en el sistema.\n";
        return;
    }

    std::cout << "Bienvenido, " << profesorActual.getNombres() << " " << profesorActual.getApellidos() << "\n";

    std::string nrcAula;
    std::cout << "Ingrese el NRC del curso al que desea registrar asistencia: ";
    std::cin >> nrcAula;

    // Validar que el NRC exista y pertenezca al profesor logueado
    bool cursoValido = false;
    cursos.recorrer([&nrcAula, &cedula, &cursoValido](Aula a) {
        if (a.getNrc() == nrcAula && a.getCedulaProfesor() == cedula) {
            cursoValido = true;
        }
    });

    if (!cursoValido) {
        std::cout << "Error: El NRC ingresado no existe o no esta asignado a su cedula.\n";
        return;
    }

    // OBTENER LA HORA AUTOMÁTICAMENTE DEL SISTEMA (Sin opción a manipulación manual)
    time_t ahora = time(0);
    tm* tiempoLocal = localtime(&ahora);
    int horaIngreso = tiempoLocal->tm_hour;   
    int minutoIngreso = tiempoLocal->tm_min; 

    int horaActualMinutos = (horaIngreso * 60) + minutoIngreso;

    std::cout << "[Hora del Sistema Capturada Automaticamente: " 
              << (horaIngreso < 10 ? "0" : "") << horaIngreso << ":" 
              << (minutoIngreso < 10 ? "0" : "") << minutoIngreso << "]\n";

    if (horaActualMinutos < 420 || horaActualMinutos > 1259) {
        std::cout << "Fuera del horario institucional de clases (07:00 a 21:00).\n";
        return;
    }

    // Cálculo de la franja horaria actual de 2 horas (07:00 - 08:59, 09:00 - 10:59, etc.)
    int inicioFranja = 420 + ((horaActualMinutos - 420) / 120) * 120;
    int finFranja = inicioFranja + 119;

    // VALIDACIÓN DE CRUCE DE CLASES: Evita que la misma cédula se registre en dos NRCs a la misma hora
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

    IntervaloHorario franjaActual(inicioFranja, finFranja);
    std::string estadoAsistencia = "";
    std::string horaTexto = (horaIngreso < 10 ? "0" : "") + std::to_string(horaIngreso) + ":" + (minutoIngreso < 10 ? "0" : "") + std::to_string(minutoIngreso);

    if (franjaActual.esIngresoValido(horaActualMinutos)) {
        estadoAsistencia = "PUNTUAL";
        std::cout << "¡Registro exitoso! Estado: PUNTUAL (Dentro de los 10 min de holgura).\n";
    } else if (horaActualMinutos > inicioFranja + 10 && horaActualMinutos <= finFranja) {
        estadoAsistencia = "ATRASO";
        std::cout << "Registro exitoso con ATRASO (Fuera del margen de holgura).\n";
    } else {
        std::cout << "No se puede registrar asistencia. Fuera de la franja horaria correspondiente.\n";
        return;
    }

    guardarAsistenciaArchivo(cedula, nrcAula, estadoAsistencia, horaTexto);
    std::cout << "El registro de asistencia se guardo en asistencias.txt correctamente.\n";
}

void Controlador::menuAdmin() {
    int opcion = 0;
    do {
        std::cout << "\n=== MENU ADMINISTRADOR ===" << std::endl;
        std::cout << "1. Registrar Profesor (Guarda en profesores.txt)" << std::endl;
        std::cout << "2. Asignar Curso / NRC a Profesor (Guarda en cursos.txt)" << std::endl;
        std::cout << "3. Volver al menu principal" << std::endl;
        opcion = leerNumeroSeguro("Seleccione una opcion: ");

        switch (opcion) {
            case 1:
                registrarProfesor();
                break;
            case 2:
                asignarCursoANrc();
                break;
            case 3:
                break;
            default:
                std::cout << "Opcion invalida.\n";
        }
    } while (opcion != 3);
}

void Controlador::menuProfesor() {
    int opcion = 0;
    do {
        std::cout << "\n=== MENU PROFESOR ===" << std::endl;
        std::cout << "1. Realizar Login y Registrar Asistencia" << std::endl;
        std::cout << "2. Volver al menu principal" << std::endl;
        opcion = leerNumeroSeguro("Seleccione una opcion: ");

        switch (opcion) {
            case 1:
                loginProfesor();
                break;
            case 2:
                break;
            default:
                std::cout << "Opcion invalida.\n";
        }
    } while (opcion != 2);
}

ListaDoble<Profesor> Controlador::getProfesores() const { return profesores; }
void Controlador::setProfesores(ListaDoble<Profesor> newProfesores) { profesores = newProfesores; }

ListaDoble<Aula> Controlador::getCursos() const { return cursos; }
void Controlador::setCursos(ListaDoble<Aula> newCursos) { cursos = newCursos; }

ListaDoble<RegistroAsistencia> Controlador::getRegistros() const { return registros; }
void Controlador::setRegistros(ListaDoble<RegistroAsistencia> newRegistros) { registros = newRegistros; }

void Controlador::agregarHorario(std::string nrc, DiaSemana dia, IntervaloHorario f) {
    // Lógica adicional de horarios si se requiere
}