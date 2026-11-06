#include "controlador.h"
#include "validacion.h"
#include "Entrada.h"
#include "Reloj.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

namespace {
const char* ARCHIVO_PROFESORES  = "profesores.txt";
const char* ARCHIVO_MATERIAS    = "materias.txt";
const char* ARCHIVO_CURSOS      = "cursos.txt";
const char* ARCHIVO_ASISTENCIAS = "asistencias.txt";

// Materias que se ofrecen al crear un curso (el numero de creditos no importa).
struct MateriaCatalogo { const char* id; const char* nombre; };
const MateriaCatalogo CATALOGO_MATERIAS[] = {
    { "CALC101", "Calculo" },
    { "EDIF201", "Ecuaciones Diferenciales" },
    { "FISI101", "Fisica" },
    { "ALGE101", "Algebra Lineal" },
    { "CALV201", "Calculo Vectorial" },
    { "ESTA101", "Estadistica" },
    { "METN201", "Metodos Numericos" },
    { "QUIM101", "Quimica" }
};
const int TOTAL_CATALOGO = sizeof(CATALOGO_MATERIAS) / sizeof(CATALOGO_MATERIAS[0]);
const int CREDITOS_POR_DEFECTO = 4;   
void avisarLineasInvalidas(const std::string& archivo, int cantidad) {
    if (cantidad > 0) {
        std::cout << "Aviso: " << cantidad << " linea invalida o duplicada ignorada en "
                  << archivo << ".\n";
    }
}
}

Controlador::Controlador(const std::string& prefijoArchivos) : prefijoArchivos(prefijoArchivos) {
    cargarProfesores();
    cargarMaterias();
    cargarCursos();       
    cargarAsistencias();
}

std::string Controlador::ruta(const char* nombreBase) const {
    return prefijoArchivos + nombreBase;
}


void Controlador::cargarProfesores() {
    std::ifstream archivo(ruta(ARCHIVO_PROFESORES));
    if (!archivo.is_open()) return;

    std::string linea;
    int invalidas = 0;
    while (std::getline(archivo, linea)) {
        linea = Validacion::trim(linea);   
        if (linea.empty()) continue;

        std::vector<std::string> c = Validacion::dividir(linea, ',');
        if (c.size() != 3 || !Validacion::validarCedula(c[0]) ||
            !Validacion::validarNombrePersona(c[1]) || !Validacion::validarNombrePersona(c[2]) ||
            buscarProfesor(c[0]) != nullptr) {
            invalidas++;
            continue;
        }
        profesores.insertar(Profesor(c[0], c[1], c[2]));
    }
    avisarLineasInvalidas(ruta(ARCHIVO_PROFESORES), invalidas);
}
void Controlador::guardarProfesores() const {
    std::ofstream archivo(ruta(ARCHIVO_PROFESORES));
    if (!archivo.is_open()) {
        std::cout << " no se pudo escribir " << ruta(ARCHIVO_PROFESORES) << ".\n";
        return;
    }
    profesores.recorrer([&archivo](const Profesor& p) {
        archivo << p.getCedula() << "," << p.getNombres() << "," << p.getApellidos() << "\n";
    });
}
void Controlador::cargarMaterias() {
    std::ifstream archivo(ruta(ARCHIVO_MATERIAS));
    if (!archivo.is_open()) return;

    std::string linea;
    int invalidas = 0;
    while (std::getline(archivo, linea)) {
        linea = Validacion::trim(linea);
        if (linea.empty()) continue;

        std::vector<std::string> c = Validacion::dividir(linea, ',');
        if (c.size() != 3 || !Validacion::validarIdMateria(c[0]) ||
            !Validacion::validarTextoLibre(c[1], 3, 60) ||
            !Validacion::soloNumeros(c[2]) || c[2].size() > 2 ||
            std::stoi(c[2]) < 1 || buscarMateria(c[0]) != nullptr) {
            invalidas++;
            continue;
        }
        materias.insertar(Materia(c[0], c[1], std::stoi(c[2])));
    }
    avisarLineasInvalidas(ruta(ARCHIVO_MATERIAS), invalidas);
}
void Controlador::guardarMaterias() const {
    std::ofstream archivo(ruta(ARCHIVO_MATERIAS));
    if (!archivo.is_open()) {
        std::cout << " no se pudo escribir " << ruta(ARCHIVO_MATERIAS) << ".\n";
        return;
    }
    materias.recorrer([&archivo](const Materia& m) {
        archivo << m.getId() << "," << m.getNombre() << "," << m.getCreditos() << "\n";
    });
}

void Controlador::cargarCursos() {
    std::ifstream archivo(ruta(ARCHIVO_CURSOS));
    if (!archivo.is_open()) return;

    std::string linea;
    int invalidas = 0;
    while (std::getline(archivo, linea)) {
        linea = Validacion::trim(linea);
        if (linea.empty()) continue;

        std::vector<std::string> c = Validacion::dividir(linea, ',');
        if (c.size() == 8) {
            c.erase(c.begin() + 4);   
            c.erase(c.begin() + 2);   
        }
        if (c.size() != 6) { invalidas++; continue; }

        DiaSemana dia;
        if (!Validacion::validarNrc(c[0]) || !buscarMateria(c[1]) || !buscarProfesor(c[2]) ||
            !parseDia(c[3], dia) ||
            !Validacion::soloNumeros(c[4]) || !Validacion::soloNumeros(c[5]) ||
            c[4].size() > 4 || c[5].size() > 4) {
            invalidas++;
            continue;
        }
        IntervaloHorario horario(std::stoi(c[4]), std::stoi(c[5]));
        if (!horario.esFranjaOficial() || buscarCurso(c[0]) != nullptr ||
            buscarChoqueDeHorario(dia, horario, "") != nullptr) {   // mismo dia y hora que otro curso
            invalidas++;
            continue;
        }
        cursos.insertar(Aula(c[0], c[1], c[2], dia, horario));
    }
    avisarLineasInvalidas(ruta(ARCHIVO_CURSOS), invalidas);
}
void Controlador::guardarCursos() const {
    std::ofstream archivo(ruta(ARCHIVO_CURSOS));
    if (!archivo.is_open()) {
        std::cout << "Error: no se pudo escribir " << ruta(ARCHIVO_CURSOS) << ".\n";
        return;
    }
    cursos.recorrer([&archivo](const Aula& a) {
        archivo << a.getNrc() << "," << a.getIdMateria() << "," << a.getCedulaProfesor() << ","
                << nombreDia(a.getDia()) << ","
                << a.getHorario().getHoraInicioMinutos() << "," << a.getHorario().getHoraFinMinutos() << "\n";
    });
}

namespace {
bool estadoCoherente(const std::string& tipo, const std::string& estado) {
    if (tipo == "ENTRADA") return estado == "PUNTUAL" || estado == "ATRASO";
    if (tipo == "SALIDA") return estado == "REGISTRADA";
    return false;
}
}

void Controlador::cargarAsistencias() {
    std::ifstream archivo(ruta(ARCHIVO_ASISTENCIAS));
    if (!archivo.is_open()) return;

    std::string linea;
    int invalidas = 0;
    while (std::getline(archivo, linea)) {
        linea = Validacion::trim(linea);
        if (linea.empty()) continue;

        std::vector<std::string> c = Validacion::dividir(linea, ',');
        int minutos = 0;
        if (c.size() != 6 || !Validacion::validarFecha(c[0]) || !Validacion::parsearHora(c[1], minutos) ||
            !Validacion::validarCedula(c[2]) || !Validacion::validarNrc(c[3]) ||
            !estadoCoherente(c[4], c[5]) ||
            buscarRegistro(c[0], c[3], c[4]) != nullptr) {      // mismo marcado repetido
            invalidas++;
            continue;
        }
        if (c[4] == "ENTRADA") {
            
            const Aula* curso = buscarCurso(c[3]);
            if (curso && buscarIngresoDeOtroProfesor(c[0], c[2], curso->getHorario()) != nullptr) {
                invalidas++;
                continue;
            }
        }
        registros.insertar(RegistroAsistencia(c[0], c[1], c[2], c[3], c[4], c[5]));
    }
    avisarLineasInvalidas(ruta(ARCHIVO_ASISTENCIAS), invalidas);
}
void Controlador::guardarAsistenciaArchivo(const RegistroAsistencia& r) const {
    std::ofstream archivo(ruta(ARCHIVO_ASISTENCIAS), std::ios::app);
    if (!archivo.is_open()) {
        std::cout << "Error: no se pudo escribir " << ruta(ARCHIVO_ASISTENCIAS) << ".\n";
        return;
    }
    archivo << r.toCsv() << "\n";
}


Profesor* Controlador::buscarProfesor(const std::string& cedula) {
    return profesores.buscarPtr([&cedula](const Profesor& p) { return p.getCedula() == cedula; });
}
Materia* Controlador::buscarMateria(const std::string& id) {
    return materias.buscarPtr([&id](const Materia& m) { return m.getId() == id; });
}
Aula* Controlador::buscarCurso(const std::string& nrc) {
    return cursos.buscarPtr([&nrc](const Aula& a) { return a.getNrc() == nrc; });
}
const RegistroAsistencia* Controlador::buscarRegistro(const std::string& fecha, const std::string& nrc,
                                                      const std::string& tipo) const {
    return registros.buscarPtr([&](const RegistroAsistencia& r) {
        return r.getFecha() == fecha && r.getNrc() == nrc && r.getTipo() == tipo;
    });
}
const Aula* Controlador::buscarChoqueDeHorario(DiaSemana dia, const IntervaloHorario& h,
                                               const std::string& nrcExcluido) const {
    return cursos.buscarPtr([&](const Aula& a) {
        return a.getNrc() != nrcExcluido && a.ocupaHorario(dia, h);
    });
}
const RegistroAsistencia* Controlador::buscarIngresoDeOtroProfesor(const std::string& fecha, const std::string& cedula,
                                                                   const IntervaloHorario& h) const {
    return registros.buscarPtr([&](const RegistroAsistencia& r) -> bool {
        if (r.getFecha() != fecha || r.getTipo() != "ENTRADA" || r.getCedula() == cedula) return false;
        const Aula* a = cursos.buscarPtr([&r](const Aula& c) { return c.getNrc() == r.getNrc(); });
        return a != nullptr && a->getHorario().seCruzaCon(h);
    });
}
std::string Controlador::nombreMateria(const std::string& id) {
    Materia* m = buscarMateria(id);
    return m ? m->getNombre() : "(materia desconocida)";
}
std::string Controlador::nombreProfesor(const std::string& cedula) {
    Profesor* p = buscarProfesor(cedula);
    return p ? p->getApellidos() + " " + p->getNombres() : "(profesor desconocido)";
}
Profesor* Controlador::seleccionarProfesor() {
    if (profesores.estaVacia()) { std::cout << "No hay profesores registrados.\n"; return nullptr; }
    std::cout << "\n Profesores \n";
    int i = 1;
    profesores.recorrer([&i](const Profesor& p) {
        std::cout << i++ << ". " << p.getCedula() << " - " << p.getApellidos() << " " << p.getNombres() << "\n";
    });
    int op = Entrada::leerEntero("Seleccione un profesor : ", 0, (int)profesores.getTamanio());
    return op == 0 ? nullptr : profesores.obtenerPtr(op - 1);
}

bool Controlador::seleccionarMateriaDeCatalogo(Materia& materia) {
    std::cout << "\n Materias\n";
    for (int i = 0; i < TOTAL_CATALOGO; i++) {
        std::cout << (i + 1) << ". " << CATALOGO_MATERIAS[i].nombre << "\n";
    }
    int op = Entrada::leerEntero("Seleccione una materia : ", 0, TOTAL_CATALOGO);
    if (op == 0) return false;

    const MateriaCatalogo& elegida = CATALOGO_MATERIAS[op - 1];
    Materia* existente = buscarMateria(elegida.id);
    materia = existente ? *existente : Materia(elegida.id, elegida.nombre, CREDITOS_POR_DEFECTO);
    return true;
}
Aula* Controlador::seleccionarCurso() {
    if (cursos.estaVacia()) { std::cout << "No hay cursos registrados.\n"; return nullptr; }
    std::cout << "\n Cursos\n";
    int i = 1;
    cursos.recorrer([&](const Aula& a) {
        std::cout << i++ << ". NRC " << a.getNrc() << " - " << nombreMateria(a.getIdMateria())
                  << " - " << nombreDia(a.getDia()) << " " << a.getHorario().toString()
                  << " - " << nombreProfesor(a.getCedulaProfesor()) << "\n";
    });
    int op = Entrada::leerEntero("Seleccione un curso : ", 0, (int)cursos.getTamanio());
    return op == 0 ? nullptr : cursos.obtenerPtr(op - 1);
}
Profesor* Controlador::identificarProfesor() {
    std::string cedula = Entrada::leerLinea("Ingrese su numero de cedula : ");
    if (cedula == "0") return nullptr;
    if (!Validacion::validarCedula(cedula)) {
        std::cout << " la cedula ingresada no es valida.\n";
        return nullptr;
    }
    Profesor* p = buscarProfesor(cedula);
    if (!p) {
        std::cout << " Profesor no encontrado en el sistema.\n";
        return nullptr;
    }
    std::cout << "bienvenido, " << p->getNombres() << " " << p->getApellidos() << "\n";
    return p;
}


Aula* Controlador::seleccionarCursoDeProfesor(const Profesor& prof) {
    std::vector<Aula*> propios;
    Nodo<Aula>* nodo = cursos.getCabeza();
    while (nodo) {
        Aula* a = nodo->getDatoPtr();
        if (a->getCedulaProfesor() == prof.getCedula()) propios.push_back(a);
        nodo = nodo->getSiguiente();
    }
    if (propios.empty()) {
        std::cout << "No tiene cursos asignados.\n";
        return nullptr;
    }
    std::cout << "\n sus cursos\n";
    for (size_t i = 0; i < propios.size(); i++) {
        std::cout << (i + 1) << ". NRC " << propios[i]->getNrc() << " - " << nombreMateria(propios[i]->getIdMateria())
                  << " - " << nombreDia(propios[i]->getDia()) << " "
                  << propios[i]->getHorario().toString() << "\n";
    }
    int op = Entrada::leerEntero("Seleccione el curso : ", 0, (int)propios.size());
    return op == 0 ? nullptr : propios[op - 1];
}


bool Controlador::pedirDiaYFranja(const std::string& cedula, const std::string& nrcExcluido,
                                  DiaSemana& dia, IntervaloHorario& horario) {
    while (true) {
        std::cout << "\nDias: 1=Lunes 2=Martes 3=Miercoles 4=Jueves 5=Viernes\n";
        int d = Entrada::leerEntero("Dia de clase : ", 0, 5);
        if (d == 0) return false;
        DiaSemana diaElegido = static_cast<DiaSemana>(d - 1);

        std::cout << "Franjas horarias del " << nombreDia(diaElegido) << ":\n";
        int libres = 0;
        for (int i = 1; i <= IntervaloHorario::totalFranjas(); i++) {
            IntervaloHorario fr = IntervaloHorario::franja(i);
            const Aula* ocupa = buscarChoqueDeHorario(diaElegido, fr, nrcExcluido);
            std::cout << "  " << i << ". " << fr.toString();
            if (ocupa) std::cout << "  [Ocupado - NRC " << ocupa->getNrc() << "]";
            else libres++;
            std::cout << "\n";
        }
        if (libres == 0) {
            std::cout << "Error: el " << nombreDia(diaElegido) << " no tiene franjas libres. Elija otro dia.\n";
            continue;
        }
        int f = Entrada::leerEntero("Franja : ", 0, IntervaloHorario::totalFranjas());
        if (f == 0) return false;

        IntervaloHorario horarioElegido = IntervaloHorario::franja(f);

        const Aula* choque = buscarChoqueDeHorario(diaElegido, horarioElegido, nrcExcluido);
        if (choque) {
            if (choque->getCedulaProfesor() == cedula) {
                std::cout << "Error: el profesor ya tiene el NRC " << choque->getNrc() << " ("
                          << nombreMateria(choque->getIdMateria()) << ") el " << nombreDia(choque->getDia())
                          << " de " << choque->getHorario().toString() << ". Elija otro horario.\n";
            } else {
                std::cout << "Error: el " << nombreDia(choque->getDia()) << " de " << choque->getHorario().toString()
                          << " ya esta ocupado por el NRC " << choque->getNrc() << " ("
                          << nombreMateria(choque->getIdMateria()) << ") del profesor "
                          << nombreProfesor(choque->getCedulaProfesor())
                          << ". Dos profesores no pueden coincidir el mismo dia a la misma hora. Elija otro horario.\n";
            }
            continue;
        }
        dia = diaElegido;
        horario = horarioElegido;
        return true;
    }
}


void Controlador::registrarProfesor() {
    std::cout << "\n Registrar nuevo profesor \n";
    std::string cedula, nombres, apellidos;

    while (true) {
        cedula = Entrada::leerLinea("Ingrese numero de cedula : ");
        if (cedula == "0") return;
        if (!Validacion::validarCedula(cedula)) {
            std::cout << " La cedula ingresada no es valida .\n";
            continue;
        }
        if (buscarProfesor(cedula)) {
            std::cout << "Ya existe un profesor registrado con esa cedula.\n";
            continue;
        }
        break;
    }

    while (true) {
        nombres = Validacion::normalizarEspacios(Entrada::leerLinea("Ingrese nombres: "));
        if (nombres == "0") return;
        if (!Validacion::validarNombrePersona(nombres)) {
            std::cout << " el nombre solo debe contener letras .\n";
            continue;
        }
        break;
    }

    while (true) {
        apellidos = Validacion::normalizarEspacios(Entrada::leerLinea("Ingrese apellidos: "));
        if (apellidos == "0") return;
        if (!Validacion::validarNombrePersona(apellidos)) {
            std::cout << " el apellido solo debe contener letras .\n";
            continue;
        }
        break;
    }

    profesores.insertar(Profesor(cedula, nombres, apellidos));
    guardarProfesores();
    std::cout << "Profesor registrado y guardado en " << ruta(ARCHIVO_PROFESORES) << " con exito!\n";
}

void Controlador::crearCursoConNrc() {
    std::cout << "\n Registrar Curso \n";
    if (profesores.estaVacia()) {
        std::cout << " necesita tener al menos un profesor registrado.\n";
        return;
    }

    Materia materia;
    if (!seleccionarMateriaDeCatalogo(materia)) return;
    const bool materiaNueva = (buscarMateria(materia.getId()) == nullptr);
    Profesor* profesor = seleccionarProfesor();
    if (!profesor) return;

    std::string nrc;
    while (true) {
        nrc = Entrada::leerLinea("NRC del curso ");
        if (nrc == "0") return;
        if (!Validacion::validarNrc(nrc)) {
            std::cout << " el NRC debe contener solo numeros .\n";
            continue;
        }
        if (buscarCurso(nrc)) {
            std::cout << " ya existe un curso con el NRC " << nrc << ".\n";
            continue;
        }
        break;
    }

    DiaSemana dia = LUNES;
    IntervaloHorario horario;
    if (!pedirDiaYFranja(profesor->getCedula(), "", dia, horario)) return;

    std::cout << "\n Resumen \n"
              << "NRC          : " << nrc << "\n"
              << "Materia      : " << materia.getId() << " - " << materia.getNombre()
              << (materiaNueva ? " (se registrara con el curso)" : "") << "\n"
              << "Docente      : " << profesor->getApellidos() << " " << profesor->getNombres() << "\n"
              << "Horario      : " << nombreDia(dia) << " " << horario.toString() << "\n";
    if (!Entrada::confirmar("Guardar el curso")) {
        std::cout << "Operacion cancelada. No se creo ningun curso.\n";
        return;
    }

    if (materiaNueva) {
        materias.insertar(materia);
        guardarMaterias();
        std::cout << "Materia " << materia.getNombre() << " registrada con exito.\n";
    }
    cursos.insertar(Aula(nrc, materia.getId(), profesor->getCedula(), dia, horario));
    guardarCursos();
    std::cout << "Curso creado correctamente con NRC " << nrc << " y guardado en " << ruta(ARCHIVO_CURSOS) << ".\n";
}
void Controlador::eliminarCurso() {
    std::cout << "\n eliminar curso \n";
    Aula* curso = seleccionarCurso();
    if (!curso) return;

    std::string nrc = curso->getNrc();
    bool conAsistencias = registros.existe([&nrc](const RegistroAsistencia& r) { return r.getNrc() == nrc; });
    if (conAsistencias) {
        std::cout << " El NRC " << nrc << " tiene asistencias registradas y no se puede eliminar.\n";
        return;
    }
    if (!Entrada::confirmar("Eliminar el NRC " + nrc)) {
        std::cout << "Operacion cancelada.\n";
        return;
    }
    cursos.eliminar([&nrc](const Aula& a) { return a.getNrc() == nrc; });
    guardarCursos();
    std::cout << "Curso " << nrc << " eliminado.\n";
}
void Controlador::eliminarProfesor() {
    std::cout << "\n Eliminar profesor \n";
    Profesor* prof = seleccionarProfesor();
    if (!prof) return;

    std::string cedula = prof->getCedula();
    size_t asignados = cursos.contar([&cedula](const Aula& a) { return a.getCedulaProfesor() == cedula; });
    if (asignados > 0) {
        std::cout << " el profesor tiene " << asignados << " curso asignado. Eliminelos o reasignelos primero.\n";
        return;
    }
    if (!Entrada::confirmar("Eliminar al profesor " + cedula)) {
        std::cout << "Operacion cancelada.\n";
        return;
    }
    profesores.eliminar([&cedula](const Profesor& p) { return p.getCedula() == cedula; });
    guardarProfesores();
    std::cout << "Profesor eliminado.\n";
}
void Controlador::mostrarEstadoReloj() const {
    Instante t = Reloj::ahora();
    DiaSemana d;
    std::cout << (Reloj::estaSimulando() ? "[SIMULADO] " : "[REAL] ") << t.fecha << " "
              << IntervaloHorario::textoHora(t.minutos) << " ("
              << (Reloj::aDiaSemana(t.diaSemana, d) ? nombreDia(d) : std::string("FIN DE SEMANA")) << ")\n";
}


void Controlador::listarProfesores() {
    std::cout << "\n Profesores (" << profesores.getTamanio() << ") \n";
    if (profesores.estaVacia()) { std::cout << "No hay profesores registrados.\n"; return; }

    ListaDoble<Profesor> copia = profesores;   
    copia.ordenar([](const Profesor& a, const Profesor& b) {
        if (a.getApellidos() != b.getApellidos()) return a.getApellidos() < b.getApellidos();
        return a.getNombres() < b.getNombres();
    });
    copia.recorrer([](const Profesor& p) {
        std::cout << p.getCedula() << " | " << p.getApellidos() << " " << p.getNombres() << "\n";
    });
}
void Controlador::listarMaterias() {
    std::cout << "\n Materias (" << materias.getTamanio() << ") \n";
    if (materias.estaVacia()) { std::cout << "No hay materias registradas.\n"; return; }

    ListaDoble<Materia> copia = materias;
    copia.ordenar([](const Materia& a, const Materia& b) { return a.getId() < b.getId(); });
    copia.recorrer([](const Materia& m) {
        std::cout << std::left << std::setw(11) << m.getId() << std::setw(30) << m.getNombre()
                  << m.getCreditos() << " creditos\n";
    });
}
void Controlador::listarCursos() {
    std::cout << "\n Cursos (" << cursos.getTamanio() << ") ---\n";
    if (cursos.estaVacia()) { std::cout << "No hay cursos registrados.\n"; return; }

    ListaDoble<Aula> copia = cursos;
    copia.ordenar([](const Aula& a, const Aula& b) {      
        if (a.getNrc().size() != b.getNrc().size()) return a.getNrc().size() < b.getNrc().size();
        return a.getNrc() < b.getNrc();
    });
    std::cout << std::left << std::setw(11) << "NRC" << std::setw(26) << "Materia"
              << std::setw(11) << "Dia" << std::setw(15) << "Horario" << "Docente\n";
    copia.recorrer([&](const Aula& a) {
        std::cout << std::left << std::setw(11) << a.getNrc() << std::setw(26) << nombreMateria(a.getIdMateria())
                  << std::setw(11) << nombreDia(a.getDia()) << std::setw(15) << a.getHorario().toString()
                  << nombreProfesor(a.getCedulaProfesor()) << "\n";
    });
}

void Controlador::registrarMarcado(bool esIngreso) {
    std::cout << (esIngreso ? "\n Registro de ingreso \n" : "\n Registro de salida\n");
    mostrarEstadoReloj();

    Profesor* prof = identificarProfesor();
    if (!prof) return;
    Aula* curso = seleccionarCursoDeProfesor(*prof);
    if (!curso) return;

    Instante t = Reloj::ahora();
    DiaSemana hoy;
    if (!Reloj::aDiaSemana(t.diaSemana, hoy)) {
        std::cout << " hoy es fin de semana, no hay clases.\n";
        return;
    }
    if (hoy != curso->getDia()) {
        std::cout << " el NRC " << curso->getNrc() << " se dicta los " << nombreDia(curso->getDia())
                  << " y hoy es " << nombreDia(hoy) << ".\n";
        return;
    }

    const IntervaloHorario h = curso->getHorario();
    const std::string nrc = curso->getNrc();
    const std::string horaTexto = IntervaloHorario::textoHora(t.minutos);
    const RegistroAsistencia* entradaHoy = buscarRegistro(t.fecha, nrc, "ENTRADA");

    if (esIngreso) {
        if (entradaHoy) {
            std::cout << "ya registro su ingreso hoy en el NRC " << nrc << " a las " << entradaHoy->getHora() << ".\n";
            return;
        }
        if (t.minutos < h.getHoraInicioMinutos()) {
            std::cout << "aun no es hora. La clase inicia a las " << IntervaloHorario::textoHora(h.getHoraInicioMinutos()) << ".\n";
            return;
        }
        if (!h.esIngresoPermitido(t.minutos)) {
            std::cout << " el plazo para registrar ingreso termino a las "
                      << IntervaloHorario::textoHora(h.getHoraFinMinutos() - 11) << ". Quedara como AUSENTE.\n";
            return;
        }
        const RegistroAsistencia* otro = buscarIngresoDeOtroProfesor(t.fecha, prof->getCedula(), h);
        if (otro) {
            std::cout << " El profesor " << nombreProfesor(otro->getCedula()) << " ya registro su ingreso hoy a las "
                      << otro->getHora() << " (NRC " << otro->getNrc() << ") en ese mismo horario. "
                      << "Dos profesores no pueden coincidir el mismo dia a la misma hora.\n";
            return;
        }
        std::string estado = h.esIngresoValido(t.minutos) ? nombreEstado(PUNTUAL) : nombreEstado(ATRASO);
        RegistroAsistencia r(t.fecha, horaTexto, prof->getCedula(), nrc, "ENTRADA", estado);
        registros.insertar(r);
        guardarAsistenciaArchivo(r);
        if (estado == "PUNTUAL") std::cout << "Ingreso registrado a las " << horaTexto << ". Estado: PUNTUAL.\n";
        else std::cout << "Ingreso registrado a las " << horaTexto << " con ATRASO .\n";
    } else {
        if (!entradaHoy) {
            std::cout << "primero debe registrar su ingreso en el NRC " << nrc << " hoy.\n";
            return;
        }
        if (buscarRegistro(t.fecha, nrc, "SALIDA")) {
            std::cout << "Error: ya registro su salida hoy en el NRC " << nrc << ".\n";
            return;
        }
        if (!h.esSalidaValido(t.minutos)) {
            std::cout << " la salida se registra entre " << IntervaloHorario::textoHora(h.getHoraFinMinutos() - 10)
                      << " y " << IntervaloHorario::textoHora(h.getHoraFinMinutos() - 1) << ".\n";
            return;
        }
        RegistroAsistencia r(t.fecha, horaTexto, prof->getCedula(), nrc, "SALIDA", "REGISTRADA");
        registros.insertar(r);
        guardarAsistenciaArchivo(r);
        std::cout << "Salida registrada a las " << horaTexto << " para el NRC " << nrc << ".\n";
    }
}
void Controlador::loginProfesor() { registrarMarcado(true); }
void Controlador::logoutProfesor() { registrarMarcado(false); }
void Controlador::verMisCursos() {
    std::cout << "\n Mis cursos \n";
    Profesor* prof = identificarProfesor();
    if (!prof) return;
    std::string cedula = prof->getCedula();

    ListaDoble<Aula> copia = cursos;
    copia.ordenar([](const Aula& a, const Aula& b) {
        if (a.getDia() != b.getDia()) return a.getDia() < b.getDia();
        return a.getHorario().getHoraInicioMinutos() < b.getHorario().getHoraInicioMinutos();
    });
    int encontrados = 0;
    copia.recorrer([&](const Aula& a) {
        if (a.getCedulaProfesor() != cedula) return;
        encontrados++;
        std::cout << "NRC " << a.getNrc() << " | " << nombreMateria(a.getIdMateria()) << " | "
                  << nombreDia(a.getDia()) << " " << a.getHorario().toString() << "\n";
    });
    if (encontrados == 0) std::cout << "No tiene cursos asignados.\n";
}


namespace {

bool pedirHoraFicticia(const std::string& mensaje, int& minutos) {
    while (true) {
        std::string texto = Validacion::trim(Entrada::leerLinea(mensaje + " : "));
        if (texto == "0") return false;
        int t = 0;
        if (!Validacion::parsearHoraFlexible(texto, t)) {
            std::cout << " hora invalida. \n";
            continue;
        }
        if (t < 7 * 60 || t > 21 * 60) {
            std::cout << " el horario de clases va de 07:00 a 21:00. Ingrese una hora dentro de ese rango.\n";
            continue;
        }
        minutos = t;
        return true;
    }
}

bool evaluarIngresoPrueba(const IntervaloHorario& h, int t, bool& puntual) {
    const std::string hora = IntervaloHorario::textoHora(t);
    if (t < h.getHoraInicioMinutos()) {
        std::cout  << hora << "  aun no es hora. La clase inicia a las "
                  << IntervaloHorario::textoHora(h.getHoraInicioMinutos()) << ".\n";
        return false;
    }
    if (!h.esIngresoPermitido(t)) {
        std::cout << hora << "  el plazo para registrar ingreso termino a las "
                  << IntervaloHorario::textoHora(h.getHoraFinMinutos() - 11) << ". Quedaria como ausente.\n";
        return false;
    }
    puntual = h.esIngresoValido(t);
    if (puntual)
        std::cout  << hora << "  ingreso a tiempo .\n";
    else
        std::cout  << hora << "  ingreso con atraso.\n";
    return true;
}


bool evaluarSalidaPrueba(const IntervaloHorario& h, int t) {
    const std::string hora = IntervaloHorario::textoHora(t);
    if (!h.esSalidaValido(t)) {
        std::cout << hora << "  la salida se registra entre "
                  << IntervaloHorario::textoHora(h.getHoraFinMinutos() - 10) << " y "
                  << IntervaloHorario::textoHora(h.getHoraFinMinutos() - 1) << ".\n";
        return false;
    }
    std::cout  << hora << "  salida a tiempo .\n";
    return true;
}

}


void Controlador::pruebaDeHora(bool esIngreso) {
    std::cout << (esIngreso ? "\n prueba de login \n" : "\n prueba de deslogin \n");
    Profesor* prof = identificarProfesor();
    if (!prof) return;
    Aula* curso = seleccionarCursoDeProfesor(*prof);
    if (!curso) return;

    int opcion = 1;
    do {
        const IntervaloHorario h = curso->getHorario();
        const int inicio = h.getHoraInicioMinutos();
        const int fin = h.getHoraFinMinutos();

        std::cout << "\nNRC " << curso->getNrc() << " - " << nombreMateria(curso->getIdMateria())
                  << " (" << nombreDia(curso->getDia()) << " " << h.toString() << ")\n";
        if (esIngreso) {
            std::cout << "Ingreso a tiempo : " << IntervaloHorario::textoHora(inicio) << " a "
                      << IntervaloHorario::textoHora(inicio + 10) << "\n"
                      << "Ingreso con atraso: hasta las " << IntervaloHorario::textoHora(fin - 11) << "\n";
        } else {
            std::cout << "Salida a tiempo : " << IntervaloHorario::textoHora(fin - 10) << " a "
                      << IntervaloHorario::textoHora(fin - 1) << " \n";
        }
        std::cout << " no se guarda nada y solo se evalua la hora no el dia \n\n";

        int t = 0;
        bool puntual = false;
        while (true) {
            if (esIngreso) {
                if (!pedirHoraFicticia("Hora de ingreso", t)) return;
                if (evaluarIngresoPrueba(h, t, puntual)) break;
                std::cout << "Intente con otra hora de ingreso.\n";
            } else {
                if (!pedirHoraFicticia("Hora de salida ", t)) return;
                if (evaluarSalidaPrueba(h, t)) break;
                std::cout << "Intente con otra hora de salida.\n";
            }
        }

        std::cout << "\n Resultado de la prueba\n";
        if (esIngreso) {
            std::cout << "Ingreso : " << IntervaloHorario::textoHora(t) << " -> "
                      << (puntual ? "a tiempo " : "con atraso ") << "\n";
        } else {
            std::cout << "Salida  : " << IntervaloHorario::textoHora(t) << " a tiempo \n";
        }

        std::cout << "\n" << (esIngreso ? "prueba de login" : "prueba de deslogin") << " \n"
                  << "1. Repetir la prueba con este curso\n"
                  << "2. Probar con otro curso\n"
                  << "3. Volver\n";
        opcion = Entrada::leerEntero("Seleccione una opcion: ", 1, 3);
        if (opcion == 2) {
            Aula* otro = seleccionarCursoDeProfesor(*prof);
            if (otro) curso = otro;    
        }
    } while (opcion != 3);
}
void Controlador::pruebaLogin() { pruebaDeHora(true); }
void Controlador::pruebaLogout() { pruebaDeHora(false); }

void Controlador::menuListados() {
    int opcion = 0;
    do {
        std::cout << "\n Listados \n"
                  << "1. Profesores\n2. Materias\n3. Cursos (NRC)\n4. Volver\n";
        opcion = Entrada::leerEntero("Seleccione una opcion: ", 1, 4);
        switch (opcion) {
            case 1: listarProfesores(); break;
            case 2: listarMaterias(); break;
            case 3: listarCursos(); break;
        }
    } while (opcion != 4);
}
void Controlador::menuHorasDePrueba() {
    int opcion = 0;
    do {
        std::cout << "\n Horas de prueba \n"
                  << "1. Prueba de login \n"
                  << "2. Prueba de deslogin \n"
                  << "3. Volver\n";
        opcion = Entrada::leerEntero("Seleccione una opcion: ", 1, 3);
        switch (opcion) {
            case 1: pruebaLogin(); break;
            case 2: pruebaLogout(); break;
            case 3: break;
        }
    } while (opcion != 3);
}
void Controlador::menuAdmin() {
    int opcion = 0;
    do {
        std::cout << "\n Menu de administrador \n"
                  << "1. Registrar profesor\n"
                  << "2. Registrar curso \n"
                  << "3. Eliminar curso\n"
                  << "4. Eliminar profesor\n"
                  << "5. Listados\n"
                  << "6. Volver al menu principal\n";
        opcion = Entrada::leerEntero("Seleccione una opcion: ", 1, 6);

        switch (opcion) {
            case 1: registrarProfesor(); break;
            case 2: crearCursoConNrc(); break;
            case 3: eliminarCurso(); break;
            case 4: eliminarProfesor(); break;
            case 5: menuListados(); break;
            case 6: break;
        }
    } while (opcion != 6);
}
void Controlador::menuProfesor() {
    int opcion = 0;
    do {
        std::cout << "\n Menu de profesor \n"
                  << "1. Registrar ingreso\n"
                  << "2. Registrar salida \n"
                  << "3. Ver mis cursos\n"
                  << "4. Horas de prueba\n"
                  << "5. Volver al menu principal\n";
        opcion = Entrada::leerEntero("Seleccione una opcion: ", 1, 5);

        switch (opcion) {
            case 1: loginProfesor(); break;
            case 2: logoutProfesor(); break;
            case 3: verMisCursos(); break;
            case 4: menuHorasDePrueba(); break;
            case 5: break;
        }
    } while (opcion != 5);
}


const ListaDoble<Profesor>& Controlador::getProfesores() const { return profesores; }
const ListaDoble<Materia>& Controlador::getMaterias() const { return materias; }
const ListaDoble<Aula>& Controlador::getCursos() const { return cursos; }
const ListaDoble<RegistroAsistencia>& Controlador::getRegistros() const { return registros; }
