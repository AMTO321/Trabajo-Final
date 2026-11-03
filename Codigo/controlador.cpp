#include "controlador.h"
#include "validacion.h"
#include "Entrada.h"
#include "GeneradorNrc.h"
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

void avisarLineasInvalidas(const std::string& archivo, int cantidad) {
    if (cantidad > 0) {
        std::cout << "Aviso: " << cantidad << " linea(s) invalida(s) o duplicada(s) ignorada(s) en "
                  << archivo << ".\n";
    }
}
}

Controlador::Controlador(const std::string& prefijoArchivos) : prefijoArchivos(prefijoArchivos) {
    cargarProfesores();
    cargarMaterias();
    cargarCursos();       // depende de profesores y materias
    cargarAsistencias();
}

std::string Controlador::ruta(const char* nombreBase) const {
    return prefijoArchivos + nombreBase;
}

// =====================================================================
//  PERSISTENCIA
// =====================================================================

void Controlador::cargarProfesores() {
    std::ifstream archivo(ruta(ARCHIVO_PROFESORES));
    if (!archivo.is_open()) return;

    std::string linea;
    int invalidas = 0;
    while (std::getline(archivo, linea)) {
        linea = Validacion::trim(linea);   // elimina el '\r' de archivos con CRLF
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
        std::cout << "Error: no se pudo escribir " << ruta(ARCHIVO_PROFESORES) << ".\n";
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
        std::cout << "Error: no se pudo escribir " << ruta(ARCHIVO_MATERIAS) << ".\n";
        return;
    }
    materias.recorrer([&archivo](const Materia& m) {
        archivo << m.getId() << "," << m.getNombre() << "," << m.getCreditos() << "\n";
    });
}

// Formato: nrc,idMateria,cedulaProfesor,dia,inicioMin,finMin
// (tambien se acepta el formato antiguo de 8 columnas:
//  nrc,idMateria,idPeriodo,cedula,paralelo,dia,inicio,fin; se ignoran periodo y paralelo)
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
            c.erase(c.begin() + 4);   // paralelo
            c.erase(c.begin() + 2);   // periodo
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
        if (!horario.esFranjaOficial() || buscarCurso(c[0]) != nullptr) {
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

// Formato: fecha,hora,cedula,nrc,tipo,estado
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
            !Validacion::soloNumeros(c[3]) || (c[4] != "ENTRADA" && c[4] != "SALIDA")) {
            invalidas++;
            continue;
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

// =====================================================================
//  BUSQUEDAS Y SELECCION
// =====================================================================

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
    std::cout << "\n--- PROFESORES ---\n";
    int i = 1;
    profesores.recorrer([&i](const Profesor& p) {
        std::cout << i++ << ". " << p.getCedula() << " - " << p.getApellidos() << " " << p.getNombres() << "\n";
    });
    int op = Entrada::leerEntero("Seleccione un profesor (0 para cancelar): ", 0, (int)profesores.getTamanio());
    return op == 0 ? nullptr : profesores.obtenerPtr(op - 1);
}
Materia* Controlador::seleccionarMateria() {
    if (materias.estaVacia()) { std::cout << "No hay materias registradas.\n"; return nullptr; }
    std::cout << "\n--- MATERIAS ---\n";
    int i = 1;
    materias.recorrer([&i](const Materia& m) {
        std::cout << i++ << ". " << m.getId() << " - " << m.getNombre() << " (" << m.getCreditos() << " creditos)\n";
    });
    int op = Entrada::leerEntero("Seleccione una materia (0 para cancelar): ", 0, (int)materias.getTamanio());
    return op == 0 ? nullptr : materias.obtenerPtr(op - 1);
}
Aula* Controlador::seleccionarCurso() {
    if (cursos.estaVacia()) { std::cout << "No hay cursos registrados.\n"; return nullptr; }
    std::cout << "\n--- CURSOS ---\n";
    int i = 1;
    cursos.recorrer([&](const Aula& a) {
        std::cout << i++ << ". NRC " << a.getNrc() << " - " << nombreMateria(a.getIdMateria())
                  << " - " << nombreDia(a.getDia()) << " " << a.getHorario().toString()
                  << " - " << nombreProfesor(a.getCedulaProfesor()) << "\n";
    });
    int op = Entrada::leerEntero("Seleccione un curso (0 para cancelar): ", 0, (int)cursos.getTamanio());
    return op == 0 ? nullptr : cursos.obtenerPtr(op - 1);
}
Profesor* Controlador::identificarProfesor() {
    std::string cedula = Entrada::leerLinea("Ingrese su numero de cedula (0 para cancelar): ");
    if (cedula == "0") return nullptr;
    if (!Validacion::validarCedula(cedula)) {
        std::cout << "Error: la cedula ingresada no es valida.\n";
        return nullptr;
    }
    Profesor* p = buscarProfesor(cedula);
    if (!p) {
        std::cout << "Acceso denegado: Profesor no encontrado en el sistema.\n";
        return nullptr;
    }
    std::cout << "Bienvenido, " << p->getNombres() << " " << p->getApellidos() << "\n";
    return p;
}

// Lista los cursos del profesor y deja elegir uno.
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
    std::cout << "\n--- SUS CURSOS ---\n";
    for (size_t i = 0; i < propios.size(); i++) {
        std::cout << (i + 1) << ". NRC " << propios[i]->getNrc() << " - " << nombreMateria(propios[i]->getIdMateria())
                  << " - " << nombreDia(propios[i]->getDia()) << " "
                  << propios[i]->getHorario().toString() << "\n";
    }
    int op = Entrada::leerEntero("Seleccione el curso (0 para cancelar): ", 0, (int)propios.size());
    return op == 0 ? nullptr : propios[op - 1];
}

// Pide dia y franja y verifica que el profesor no tenga otro curso que se cruce.
bool Controlador::pedirDiaYFranja(const std::string& cedula, const std::string& nrcExcluido,
                                  DiaSemana& dia, IntervaloHorario& horario) {
    while (true) {
        std::cout << "\nDias: 1=Lunes 2=Martes 3=Miercoles 4=Jueves 5=Viernes\n";
        int d = Entrada::leerEntero("Dia de clase (0 para cancelar): ", 0, 5);
        if (d == 0) return false;

        std::cout << "Franjas horarias:\n";
        for (int i = 1; i <= IntervaloHorario::totalFranjas(); i++) {
            std::cout << "  " << i << ". " << IntervaloHorario::franja(i).toString() << "\n";
        }
        int f = Entrada::leerEntero("Franja (0 para cancelar): ", 0, IntervaloHorario::totalFranjas());
        if (f == 0) return false;

        DiaSemana diaElegido = static_cast<DiaSemana>(d - 1);
        IntervaloHorario horarioElegido = IntervaloHorario::franja(f);

        const Aula* choque = cursos.buscarPtr([&](const Aula& a) {
            return a.getNrc() != nrcExcluido && a.tieneCruce(cedula, diaElegido, horarioElegido);
        });
        if (choque) {
            std::cout << "Error: el profesor ya tiene el NRC " << choque->getNrc() << " ("
                      << nombreMateria(choque->getIdMateria()) << ") el " << nombreDia(choque->getDia())
                      << " de " << choque->getHorario().toString() << ". Elija otro horario.\n";
            continue;
        }
        dia = diaElegido;
        horario = horarioElegido;
        return true;
    }
}

// =====================================================================
//  ADMINISTRACION
// =====================================================================

void Controlador::registrarProfesor() {
    std::cout << "\n--- REGISTRAR NUEVO PROFESOR (0 para cancelar) ---\n";
    std::string cedula, nombres, apellidos;

    while (true) {
        cedula = Entrada::leerLinea("Ingrese numero de cedula (10 digitos): ");
        if (cedula == "0") return;
        if (!Validacion::validarCedula(cedula)) {
            std::cout << "Error: La cedula ingresada no es valida en Ecuador.\n";
            continue;
        }
        if (buscarProfesor(cedula)) {
            std::cout << "Error: Ya existe un profesor registrado con esa cedula.\n";
            continue;
        }
        break;
    }

    while (true) {
        nombres = Validacion::normalizarEspacios(Entrada::leerLinea("Ingrese nombres: "));
        if (nombres == "0") return;
        if (!Validacion::validarNombrePersona(nombres)) {
            std::cout << "Error: el nombre solo debe contener letras (2 a 50 caracteres).\n";
            continue;
        }
        break;
    }

    while (true) {
        apellidos = Validacion::normalizarEspacios(Entrada::leerLinea("Ingrese apellidos: "));
        if (apellidos == "0") return;
        if (!Validacion::validarNombrePersona(apellidos)) {
            std::cout << "Error: el apellido solo debe contener letras (2 a 50 caracteres).\n";
            continue;
        }
        break;
    }

    profesores.insertar(Profesor(cedula, nombres, apellidos));
    guardarProfesores();
    std::cout << "Profesor registrado y guardado en " << ruta(ARCHIVO_PROFESORES) << " con exito!\n";
}
void Controlador::registrarMateria() {
    std::cout << "\n--- REGISTRAR MATERIA (0 para cancelar) ---\n";
    std::string id, nombre;

    while (true) {
        id = Validacion::aMayusculas(Entrada::leerLinea("Codigo de la materia (ej. ALGE101): "));
        if (id == "0") return;
        if (!Validacion::validarIdMateria(id)) {
            std::cout << "Error: el codigo debe tener 4 a 10 letras/numeros sin espacios.\n";
            continue;
        }
        if (buscarMateria(id)) {
            std::cout << "Error: ya existe una materia con ese codigo.\n";
            continue;
        }
        break;
    }

    while (true) {
        nombre = Validacion::normalizarEspacios(Entrada::leerLinea("Nombre de la materia: "));
        if (nombre == "0") return;
        if (!Validacion::validarTextoLibre(nombre, 3, 60)) {
            std::cout << "Error: el nombre debe tener 3 a 60 caracteres (letras, numeros y espacios).\n";
            continue;
        }
        std::string mayus = Validacion::aMayusculas(nombre);
        bool repetido = materias.existe([&mayus](const Materia& m) {
            return Validacion::aMayusculas(m.getNombre()) == mayus;
        });
        if (repetido) {
            std::cout << "Error: ya existe una materia con ese nombre.\n";
            continue;
        }
        break;
    }

    int creditos = Entrada::leerEntero("Creditos (1-10): ", 1, 10);
    materias.insertar(Materia(id, nombre, creditos));
    guardarMaterias();
    std::cout << "Materia " << id << " - " << nombre << " registrada con exito.\n";
}

// El administrador elige materia, docente, dia y franja; el sistema genera el NRC.
void Controlador::crearCursoConNrc() {
    std::cout << "\n--- CREAR CURSO Y GENERAR NRC ---\n";
    if (materias.estaVacia() || profesores.estaVacia()) {
        std::cout << "Error: necesita tener al menos una materia y un profesor registrados.\n";
        return;
    }

    Materia* materia = seleccionarMateria();
    if (!materia) return;
    Profesor* profesor = seleccionarProfesor();
    if (!profesor) return;

    DiaSemana dia = LUNES;
    IntervaloHorario horario;
    if (!pedirDiaYFranja(profesor->getCedula(), "", dia, horario)) return;

    std::string nrc = GeneradorNrc::siguiente(cursos);
    if (nrc.empty()) {
        std::cout << "Error: se agotaron los NRC disponibles (maximo 99999).\n";
        return;
    }

    std::cout << "\n--- RESUMEN ---\n"
              << "NRC generado : " << nrc << "\n"
              << "Materia      : " << materia->getId() << " - " << materia->getNombre() << "\n"
              << "Docente      : " << profesor->getApellidos() << " " << profesor->getNombres() << "\n"
              << "Horario      : " << nombreDia(dia) << " " << horario.toString() << "\n";
    if (!Entrada::confirmar("Guardar el curso")) {
        std::cout << "Operacion cancelada. No se creo ningun curso.\n";
        return;
    }

    cursos.insertar(Aula(nrc, materia->getId(), profesor->getCedula(), dia, horario));
    guardarCursos();
    std::cout << "Curso creado correctamente con NRC " << nrc << " y guardado en " << ruta(ARCHIVO_CURSOS) << ".\n";
}
bool Controlador::agregarHorario(const std::string& nrc, DiaSemana dia, const IntervaloHorario& f) {
    Aula* curso = buscarCurso(nrc);
    if (!curso || !f.esFranjaOficial()) return false;

    const Aula* choque = cursos.buscarPtr([&](const Aula& a) {
        return a.getNrc() != nrc && a.tieneCruce(curso->getCedulaProfesor(), dia, f);
    });
    if (choque) return false;

    curso->setDia(dia);
    curso->setHorario(f);
    guardarCursos();
    return true;
}
void Controlador::cambiarHorarioCurso() {
    std::cout << "\n--- CAMBIAR HORARIO DE UN CURSO ---\n";
    Aula* curso = seleccionarCurso();
    if (!curso) return;

    DiaSemana dia = LUNES;
    IntervaloHorario horario;
    if (!pedirDiaYFranja(curso->getCedulaProfesor(), curso->getNrc(), dia, horario)) return;

    if (agregarHorario(curso->getNrc(), dia, horario)) {
        std::cout << "Horario del NRC " << curso->getNrc() << " actualizado a " << nombreDia(dia)
                  << " " << horario.toString() << ".\n";
    } else {
        std::cout << "Error: no se pudo actualizar el horario.\n";
    }
}
void Controlador::eliminarCurso() {
    std::cout << "\n--- ELIMINAR CURSO ---\n";
    Aula* curso = seleccionarCurso();
    if (!curso) return;

    std::string nrc = curso->getNrc();
    bool conAsistencias = registros.existe([&nrc](const RegistroAsistencia& r) { return r.getNrc() == nrc; });
    if (conAsistencias) {
        std::cout << "Error: el NRC " << nrc << " tiene asistencias registradas y no se puede eliminar.\n";
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
    std::cout << "\n--- ELIMINAR PROFESOR ---\n";
    Profesor* prof = seleccionarProfesor();
    if (!prof) return;

    std::string cedula = prof->getCedula();
    size_t asignados = cursos.contar([&cedula](const Aula& a) { return a.getCedulaProfesor() == cedula; });
    if (asignados > 0) {
        std::cout << "Error: el profesor tiene " << asignados << " curso(s) asignado(s). Eliminelos o reasignelos primero.\n";
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

// =====================================================================
//  LISTADOS Y REPORTES
// =====================================================================

void Controlador::listarProfesores() {
    std::cout << "\n--- PROFESORES (" << profesores.getTamanio() << ") ---\n";
    if (profesores.estaVacia()) { std::cout << "No hay profesores registrados.\n"; return; }

    ListaDoble<Profesor> copia = profesores;   // se ordena la copia, no la original
    copia.ordenar([](const Profesor& a, const Profesor& b) {
        if (a.getApellidos() != b.getApellidos()) return a.getApellidos() < b.getApellidos();
        return a.getNombres() < b.getNombres();
    });
    copia.recorrer([](const Profesor& p) {
        std::cout << p.getCedula() << " | " << p.getApellidos() << " " << p.getNombres() << "\n";
    });
}
void Controlador::listarMaterias() {
    std::cout << "\n--- MATERIAS (" << materias.getTamanio() << ") ---\n";
    if (materias.estaVacia()) { std::cout << "No hay materias registradas.\n"; return; }

    ListaDoble<Materia> copia = materias;
    copia.ordenar([](const Materia& a, const Materia& b) { return a.getId() < b.getId(); });
    copia.recorrer([](const Materia& m) {
        std::cout << std::left << std::setw(11) << m.getId() << std::setw(30) << m.getNombre()
                  << m.getCreditos() << " creditos\n";
    });
}
void Controlador::listarCursos() {
    std::cout << "\n--- CURSOS / NRC (" << cursos.getTamanio() << ") ---\n";
    if (cursos.estaVacia()) { std::cout << "No hay cursos registrados.\n"; return; }

    ListaDoble<Aula> copia = cursos;
    copia.ordenar([](const Aula& a, const Aula& b) { return a.getNrc() < b.getNrc(); });
    std::cout << std::left << std::setw(7) << "NRC" << std::setw(26) << "Materia"
              << std::setw(11) << "Dia" << std::setw(15) << "Horario" << "Docente\n";
    copia.recorrer([&](const Aula& a) {
        std::cout << std::left << std::setw(7) << a.getNrc() << std::setw(26) << nombreMateria(a.getIdMateria())
                  << std::setw(11) << nombreDia(a.getDia()) << std::setw(15) << a.getHorario().toString()
                  << nombreProfesor(a.getCedulaProfesor()) << "\n";
    });
}
void Controlador::reporteAsistencias() {
    std::cout << "\n--- REPORTE DE ASISTENCIAS POR FECHA ---\n";
    Instante hoy = Reloj::ahora();

    std::string fecha;
    while (true) {
        fecha = Entrada::leerLinea("Fecha AAAA-MM-DD [Enter = " + hoy.fecha + ", 0 para cancelar]: ");
        if (fecha == "0") return;
        if (fecha.empty()) { fecha = hoy.fecha; break; }
        if (Validacion::validarFecha(fecha)) break;
        std::cout << "Error: fecha invalida. Ejemplo: 2026-11-02\n";
    }

    DiaSemana dia;
    if (!Reloj::aDiaSemana(Reloj::diaDeSemanaDeFecha(fecha), dia)) {
        std::cout << "El " << fecha << " cae en fin de semana: no hay clases.\n";
        return;
    }

    ListaDoble<Aula> copia = cursos;
    copia.ordenar([](const Aula& a, const Aula& b) {
        if (a.getHorario().getHoraInicioMinutos() != b.getHorario().getHoraInicioMinutos())
            return a.getHorario().getHoraInicioMinutos() < b.getHorario().getHoraInicioMinutos();
        return a.getNrc() < b.getNrc();
    });

    std::cout << "\nClases del " << nombreDia(dia) << " " << fecha << "\n";
    std::cout << std::left << std::setw(14) << "Horario" << std::setw(7) << "NRC" << std::setw(24) << "Materia"
              << std::setw(24) << "Docente" << std::setw(8) << "Entrada" << std::setw(8) << "Salida" << "Estado\n";

    int puntuales = 0, atrasos = 0, ausentes = 0, pendientes = 0, total = 0;
    copia.recorrer([&](const Aula& a) {
        if (a.getDia() != dia) return;
        total++;

        const RegistroAsistencia* entrada = buscarRegistro(fecha, a.getNrc(), "ENTRADA");
        const RegistroAsistencia* salida = buscarRegistro(fecha, a.getNrc(), "SALIDA");
        std::string estado;
        if (entrada) {
            estado = entrada->getEstado();
            if (estado == "PUNTUAL") puntuales++; else atrasos++;
            if (!salida) estado += " (sin salida)";
        } else {
            bool yaTermino = fecha < hoy.fecha || (fecha == hoy.fecha && hoy.minutos >= a.getHorario().getHoraFinMinutos());
            if (yaTermino) { estado = nombreEstado(AUSENTE); ausentes++; }
            else { estado = "PENDIENTE"; pendientes++; }
        }
        std::cout << std::left << std::setw(14) << a.getHorario().toString() << std::setw(7) << a.getNrc()
                  << std::setw(24) << nombreMateria(a.getIdMateria()).substr(0, 23)
                  << std::setw(24) << nombreProfesor(a.getCedulaProfesor()).substr(0, 23)
                  << std::setw(8) << (entrada ? entrada->getHora() : "--")
                  << std::setw(8) << (salida ? salida->getHora() : "--") << estado << "\n";
    });

    if (total == 0) {
        std::cout << "No hay clases programadas para ese dia.\n";
        return;
    }
    std::cout << "\nResumen: " << total << " clase(s) | " << puntuales << " puntual(es) | " << atrasos
              << " atraso(s) | " << ausentes << " ausente(s) | " << pendientes << " pendiente(s)\n";
}

// =====================================================================
//  PROFESOR: INGRESO / SALIDA
// =====================================================================

void Controlador::registrarMarcado(bool esIngreso) {
    std::cout << (esIngreso ? "\n=== REGISTRO DE INGRESO ===\n" : "\n=== REGISTRO DE SALIDA ===\n");
    mostrarEstadoReloj();

    Profesor* prof = identificarProfesor();
    if (!prof) return;
    Aula* curso = seleccionarCursoDeProfesor(*prof);
    if (!curso) return;

    Instante t = Reloj::ahora();
    DiaSemana hoy;
    if (!Reloj::aDiaSemana(t.diaSemana, hoy)) {
        std::cout << "Error: hoy es fin de semana, no hay clases.\n";
        return;
    }
    if (hoy != curso->getDia()) {
        std::cout << "Error: el NRC " << curso->getNrc() << " se dicta los " << nombreDia(curso->getDia())
                  << " y hoy es " << nombreDia(hoy) << ".\n";
        return;
    }

    const IntervaloHorario h = curso->getHorario();
    const std::string nrc = curso->getNrc();
    const std::string horaTexto = IntervaloHorario::textoHora(t.minutos);
    const RegistroAsistencia* entradaHoy = buscarRegistro(t.fecha, nrc, "ENTRADA");

    if (esIngreso) {
        if (entradaHoy) {
            std::cout << "Error: ya registro su ingreso hoy en el NRC " << nrc << " a las " << entradaHoy->getHora() << ".\n";
            return;
        }
        if (t.minutos < h.getHoraInicioMinutos()) {
            std::cout << "Error: aun no es hora. La clase inicia a las " << IntervaloHorario::textoHora(h.getHoraInicioMinutos()) << ".\n";
            return;
        }
        if (!h.esIngresoPermitido(t.minutos)) {
            std::cout << "Error: el plazo para registrar ingreso termino a las "
                      << IntervaloHorario::textoHora(h.getHoraFinMinutos() - 11) << ". Quedara como AUSENTE.\n";
            return;
        }
        std::string estado = h.esIngresoValido(t.minutos) ? nombreEstado(PUNTUAL) : nombreEstado(ATRASO);
        RegistroAsistencia r(t.fecha, horaTexto, prof->getCedula(), nrc, "ENTRADA", estado);
        registros.insertar(r);
        guardarAsistenciaArchivo(r);
        if (estado == "PUNTUAL") std::cout << "Ingreso registrado a las " << horaTexto << ". Estado: PUNTUAL.\n";
        else std::cout << "Ingreso registrado a las " << horaTexto << " con ATRASO (paso la holgura de 10 min).\n";
    } else {
        if (!entradaHoy) {
            std::cout << "Error: primero debe registrar su ingreso en el NRC " << nrc << " hoy.\n";
            return;
        }
        if (buscarRegistro(t.fecha, nrc, "SALIDA")) {
            std::cout << "Error: ya registro su salida hoy en el NRC " << nrc << ".\n";
            return;
        }
        if (!h.esSalidaValido(t.minutos)) {
            std::cout << "Error: la salida se registra entre " << IntervaloHorario::textoHora(h.getHoraFinMinutos() - 10)
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
    std::cout << "\n=== MIS CURSOS ===\n";
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

// =====================================================================
//  MENUS
// =====================================================================

void Controlador::menuListados() {
    int opcion = 0;
    do {
        std::cout << "\n=== LISTADOS ===\n"
                  << "1. Profesores\n2. Materias\n3. Cursos (NRC)\n4. Volver\n";
        opcion = Entrada::leerEntero("Seleccione una opcion: ", 1, 4);
        switch (opcion) {
            case 1: listarProfesores(); break;
            case 2: listarMaterias(); break;
            case 3: listarCursos(); break;
        }
    } while (opcion != 4);
}
void Controlador::menuAdmin() {
    int opcion = 0;
    do {
        std::cout << "\n=== MENU ADMINISTRADOR ===\n"
                  << "1. Registrar profesor\n"
                  << "2. Registrar materia\n"
                  << "3. Crear curso (genera el NRC automaticamente)\n"
                  << "4. Cambiar horario de un curso\n"
                  << "5. Eliminar curso\n"
                  << "6. Eliminar profesor\n"
                  << "7. Listados\n"
                  << "8. Reporte de asistencias por fecha\n"
                  << "9. Volver al menu principal\n";
        opcion = Entrada::leerEntero("Seleccione una opcion: ", 1, 9);

        switch (opcion) {
            case 1: registrarProfesor(); break;
            case 2: registrarMateria(); break;
            case 3: crearCursoConNrc(); break;
            case 4: cambiarHorarioCurso(); break;
            case 5: eliminarCurso(); break;
            case 6: eliminarProfesor(); break;
            case 7: menuListados(); break;
            case 8: reporteAsistencias(); break;
            case 9: break;
        }
    } while (opcion != 9);
}
void Controlador::menuProfesor() {
    int opcion = 0;
    do {
        std::cout << "\n=== MENU PROFESOR ===\n"
                  << "1. Registrar ingreso\n"
                  << "2. Registrar salida / desloguearse\n"
                  << "3. Ver mis cursos\n"
                  << "4. Volver al menu principal\n";
        opcion = Entrada::leerEntero("Seleccione una opcion: ", 1, 4);

        switch (opcion) {
            case 1: loginProfesor(); break;
            case 2: logoutProfesor(); break;
            case 3: verMisCursos(); break;
            case 4: break;
        }
    } while (opcion != 4);
}

// =====================================================================
//  GETTERS
// =====================================================================

const ListaDoble<Profesor>& Controlador::getProfesores() const { return profesores; }
const ListaDoble<Materia>& Controlador::getMaterias() const { return materias; }
const ListaDoble<Aula>& Controlador::getCursos() const { return cursos; }
const ListaDoble<RegistroAsistencia>& Controlador::getRegistros() const { return registros; }
