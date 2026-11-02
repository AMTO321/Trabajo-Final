#include "controlador.h"
#include "validacion.h"
#include <iostream>
#include <fstream>
#include <sstream>

Controlador::Controlador() {
    cargarProfesores();
    cargarCursos();
}

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

    // Delegamos la lógica de asistencia al objeto correspondiente
    RegistroAsistencia registro;
    registro.registrarIngreso(cedula, nrcAula);
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
    
}