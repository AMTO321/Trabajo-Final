#include "mini_test.h"
#include "entorno_prueba.h"
#include "controlador.h"
#include <string>

// Estas pruebas usan una carpeta temporal para que el Controlador lea/escriba
// sus archivos (profesores.txt, cursos.txt) sin tocar los reales, y reemplazan
// std::cin / std::cout por texto prefabricado para simular al usuario.
//
// IMPORTANTE: el texto de entrada debe contener TODO lo que el programa va a
// pedir; si faltara una linea, algunos bucles del Controlador no terminarian.

static bool contiene(const std::string& texto, const std::string& parte) {
    return texto.find(parte) != std::string::npos;
}

// ---------- Carga de archivos (constructor) ----------

TEST_CASE("Controlador: sin archivos arranca con listas vacias") {
    DirectorioTemporal dir;
    Controlador c;
    CHECK_EQ(c.getProfesores().getTamanio(), 0);
    CHECK_EQ(c.getCursos().getTamanio(), 0);
    CHECK_EQ(c.getRegistros().getTamanio(), 0);
}

TEST_CASE("Controlador: carga los profesores de profesores.txt en el orden del archivo") {
    DirectorioTemporal dir;
    escribirArchivo("profesores.txt",
                    "0605293885,adrian michael,Toapanta orejuela\n"
                    "1710034065,Ana,Gomez\n");
    Controlador c;
    ListaDoble<Profesor> lista = c.getProfesores();
    REQUIRE_EQ(lista.getTamanio(), 2);
    CHECK_EQ(lista.getCabeza()->getDato().getCedula(), std::string("0605293885"));
    CHECK_EQ(lista.getCabeza()->getDato().getNombres(), std::string("adrian michael"));
    CHECK_EQ(lista.getCabeza()->getDato().getApellidos(), std::string("Toapanta orejuela"));
    CHECK_EQ(lista.getCola()->getDato().getCedula(), std::string("1710034065"));
}

TEST_CASE("Controlador: ignora lineas de profesores incompletas o vacias") {
    DirectorioTemporal dir;
    escribirArchivo("profesores.txt",
                    "0605293885,SoloNombre\n"      // faltan apellidos
                    "\n"                            // linea vacia
                    "1710034065,Ana,Gomez\n");
    Controlador c;
    CHECK_EQ(c.getProfesores().getTamanio(), 1);
}

TEST_CASE("Controlador: carga los cursos de cursos.txt (nrc, materia, cedula del profesor)") {
    DirectorioTemporal dir;
    escribirArchivo("cursos.txt", "4,edo,0605293885\n");
    Controlador c;
    ListaDoble<Aula> lista = c.getCursos();
    REQUIRE_EQ(lista.getTamanio(), 1);
    CHECK_EQ(lista.getCabeza()->getDato().getNrc(), std::string("4"));
    CHECK_EQ(lista.getCabeza()->getDato().getMateria(), std::string("edo"));
    CHECK_EQ(lista.getCabeza()->getDato().getCedulaProfesor(), std::string("0605293885"));
}

TEST_CASE("Controlador: ignora lineas de cursos incompletas") {
    DirectorioTemporal dir;
    escribirArchivo("cursos.txt", "4,edo\n5,fisica,1710034065\n");
    Controlador c;
    CHECK_EQ(c.getCursos().getTamanio(), 1);
}

TEST_CASE("Controlador: setProfesores y getProfesores se corresponden") {
    DirectorioTemporal dir;
    Controlador c;
    ListaDoble<Profesor> lista;
    lista.insertar(Profesor("0605293885", "A", "B"));
    c.setProfesores(lista);
    CHECK_EQ(c.getProfesores().getTamanio(), 1);
}

// ---------- registrarProfesor ----------

TEST_CASE("Controlador::registrarProfesor guarda un profesor valido en la lista y en profesores.txt") {
    DirectorioTemporal dir;
    Controlador c;
    {
        mini::EntradaSimulada in("0605293885\nJuan Carlos\nPerez Lopez\n");
        mini::CapturaSalida out;
        c.registrarProfesor();
    }
    REQUIRE_EQ(c.getProfesores().getTamanio(), 1);
    CHECK_EQ(c.getProfesores().getCabeza()->getDato().getNombres(), std::string("Juan Carlos"));
    CHECK_EQ(c.getProfesores().getCabeza()->getDato().getApellidos(), std::string("Perez Lopez"));
    CHECK_EQ(leerArchivo("profesores.txt"), std::string("0605293885,Juan Carlos,Perez Lopez\n"));
}

TEST_CASE("Controlador::registrarProfesor rechaza una cedula invalida y vuelve a pedirla") {
    DirectorioTemporal dir;
    Controlador c;
    std::string salida;
    {
        mini::EntradaSimulada in("1234567890\n0605293885\nJuan\nPerez\n");
        mini::CapturaSalida out;
        c.registrarProfesor();
        salida = out.texto();
    }
    CHECK(contiene(salida, "no es valida"));
    REQUIRE_EQ(c.getProfesores().getTamanio(), 1);
    CHECK_EQ(c.getProfesores().getCabeza()->getDato().getCedula(), std::string("0605293885"));
}

TEST_CASE("Controlador::registrarProfesor rechaza una cedula ya registrada") {
    DirectorioTemporal dir;
    escribirArchivo("profesores.txt", "0605293885,Adrian,Toapanta\n");
    Controlador c;
    std::string salida;
    {
        mini::EntradaSimulada in("0605293885\n1710034065\nAna\nGomez\n");
        mini::CapturaSalida out;
        c.registrarProfesor();
        salida = out.texto();
    }
    CHECK(contiene(salida, "Ya existe un profesor"));
    CHECK_EQ(c.getProfesores().getTamanio(), 2);   // el original + el nuevo, no un duplicado
    CHECK_EQ(leerArchivo("profesores.txt"),
             std::string("0605293885,Adrian,Toapanta\n1710034065,Ana,Gomez\n"));
}

DEFECT_CASE("[DEFECTO] registrarProfesor conserva intacto el nombre tras un intento invalido") {
    // Escribe "12" (invalido) y luego "Juan". El cin.ignore() dentro del bucle
    // se ejecuta de nuevo y se come la 'J' del segundo intento.
    DirectorioTemporal dir;
    Controlador c;
    {
        mini::EntradaSimulada in("0605293885\n12\nJuan\nPerez\n");
        mini::CapturaSalida out;
        c.registrarProfesor();
    }
    REQUIRE_EQ(c.getProfesores().getTamanio(), 1);
    CHECK_EQ(c.getProfesores().getCabeza()->getDato().getNombres(), std::string("Juan"));
}

// ---------- asignarCursoANrc ----------

TEST_CASE("Controlador::asignarCursoANrc registra un curso para un profesor existente") {
    DirectorioTemporal dir;
    escribirArchivo("profesores.txt", "0605293885,Adrian,Toapanta\n");
    Controlador c;
    {
        mini::EntradaSimulada in("4\nMatematica\n0605293885\n");
        mini::CapturaSalida out;
        c.asignarCursoANrc();
    }
    REQUIRE_EQ(c.getCursos().getTamanio(), 1);
    CHECK_EQ(c.getCursos().getCabeza()->getDato().getNrc(), std::string("4"));
    CHECK_EQ(leerArchivo("cursos.txt"), std::string("4,Matematica,0605293885\n"));
}

TEST_CASE("Controlador::asignarCursoANrc rechaza un NRC con letras y lo vuelve a pedir") {
    DirectorioTemporal dir;
    escribirArchivo("profesores.txt", "0605293885,Adrian,Toapanta\n");
    Controlador c;
    std::string salida;
    {
        mini::EntradaSimulada in("abc\n8\nBiologia\n0605293885\n");
        mini::CapturaSalida out;
        c.asignarCursoANrc();
        salida = out.texto();
    }
    CHECK(contiene(salida, "unicamente numeros"));
    REQUIRE_EQ(c.getCursos().getTamanio(), 1);
    CHECK_EQ(c.getCursos().getCabeza()->getDato().getNrc(), std::string("8"));
}

TEST_CASE("Controlador::asignarCursoANrc rechaza un NRC repetido") {
    DirectorioTemporal dir;
    escribirArchivo("profesores.txt", "0605293885,Adrian,Toapanta\n");
    escribirArchivo("cursos.txt", "4,edo,0605293885\n");
    Controlador c;
    std::string salida;
    {
        mini::EntradaSimulada in("4\n5\nFisica\n0605293885\n");
        mini::CapturaSalida out;
        c.asignarCursoANrc();
        salida = out.texto();
    }
    CHECK(contiene(salida, "ya esta registrado"));
    CHECK_EQ(c.getCursos().getTamanio(), 2);
}

TEST_CASE("Controlador::asignarCursoANrc no crea el curso si el profesor no existe") {
    DirectorioTemporal dir;
    Controlador c;
    std::string salida;
    {
        mini::EntradaSimulada in("7\nQuimica\n1710034065\n");
        mini::CapturaSalida out;
        c.asignarCursoANrc();
        salida = out.texto();
    }
    CHECK(contiene(salida, "No existe un profesor"));
    CHECK_EQ(c.getCursos().getTamanio(), 0);
    CHECK(!existeArchivo("cursos.txt"));
}

TEST_CASE("Controlador::asignarCursoANrc rechaza una materia con numeros") {
    DirectorioTemporal dir;
    escribirArchivo("profesores.txt", "0605293885,Adrian,Toapanta\n");
    Controlador c;
    std::string salida;
    {
        // Tras la primera materia invalida se vuelve a pedir; se antepone un
        // caracter de relleno 'x' porque el cin.ignore() del bucle consume el primero.
        mini::EntradaSimulada in("4\nFisica2\nxFisica\n0605293885\n");
        mini::CapturaSalida out;
        c.asignarCursoANrc();
        salida = out.texto();
    }
    CHECK(contiene(salida, "solo letras"));
    CHECK_EQ(c.getCursos().getTamanio(), 1);
}

// ---------- loginProfesor (solo los rechazos, que no dependen del reloj) ----------

TEST_CASE("Controlador::loginProfesor deniega el acceso a una cedula no registrada") {
    DirectorioTemporal dir;
    Controlador c;
    std::string salida;
    {
        mini::EntradaSimulada in("1710034065\n");
        mini::CapturaSalida out;
        c.loginProfesor();
        salida = out.texto();
    }
    CHECK(contiene(salida, "Acceso denegado"));
    CHECK(!existeArchivo("asistencias.txt"));
}

TEST_CASE("Controlador::loginProfesor rechaza un NRC que no existe") {
    DirectorioTemporal dir;
    escribirArchivo("profesores.txt", "0605293885,Adrian,Toapanta\n");
    Controlador c;
    std::string salida;
    {
        mini::EntradaSimulada in("0605293885\n99\n");
        mini::CapturaSalida out;
        c.loginProfesor();
        salida = out.texto();
    }
    CHECK(contiene(salida, "Bienvenido, Adrian Toapanta"));
    CHECK(contiene(salida, "no existe o no esta asignado"));
    CHECK(!existeArchivo("asistencias.txt"));
}

TEST_CASE("Controlador::loginProfesor rechaza un NRC asignado a OTRO profesor") {
    DirectorioTemporal dir;
    escribirArchivo("profesores.txt",
                    "0605293885,Adrian,Toapanta\n1710034065,Ana,Gomez\n");
    escribirArchivo("cursos.txt", "4,edo,1710034065\n");
    Controlador c;
    std::string salida;
    {
        mini::EntradaSimulada in("0605293885\n4\n");
        mini::CapturaSalida out;
        c.loginProfesor();
        salida = out.texto();
    }
    CHECK(contiene(salida, "no existe o no esta asignado"));
    CHECK(!existeArchivo("asistencias.txt"));
}

// ---------- Menus ----------

TEST_CASE("Controlador::menuAdmin: la opcion 3 vuelve al menu principal") {
    DirectorioTemporal dir;
    Controlador c;
    std::string salida;
    {
        mini::EntradaSimulada in("3\n");
        mini::CapturaSalida out;
        c.menuAdmin();
        salida = out.texto();
    }
    CHECK(contiene(salida, "MENU ADMINISTRADOR"));
}

TEST_CASE("Controlador::menuAdmin: una opcion fuera de rango muestra 'Opcion invalida' y sigue") {
    DirectorioTemporal dir;
    Controlador c;
    std::string salida;
    {
        mini::EntradaSimulada in("9\n3\n");
        mini::CapturaSalida out;
        c.menuAdmin();
        salida = out.texto();
    }
    CHECK(contiene(salida, "Opcion invalida"));
}

TEST_CASE("Controlador::menuAdmin: una opcion no numerica se rechaza y se vuelve a pedir") {
    DirectorioTemporal dir;
    Controlador c;
    std::string salida;
    {
        mini::EntradaSimulada in("abc\n3\n");
        mini::CapturaSalida out;
        c.menuAdmin();
        salida = out.texto();
    }
    CHECK(contiene(salida, "solo numeros validos"));
}

TEST_CASE("Controlador::menuProfesor: la opcion 2 vuelve al menu principal") {
    DirectorioTemporal dir;
    Controlador c;
    std::string salida;
    {
        mini::EntradaSimulada in("2\n");
        mini::CapturaSalida out;
        c.menuProfesor();
        salida = out.texto();
    }
    CHECK(contiene(salida, "MENU PROFESOR"));
}

DEFECT_CASE("[DEFECTO] los menus no se caen si se escribe un numero demasiado grande") {
    // leerNumeroSeguro llama a std::stoi sin controlar std::out_of_range.
    DirectorioTemporal dir;
    Controlador c;
    {
        mini::EntradaSimulada in("99999999999999\n3\n");
        mini::CapturaSalida out;
        CHECK_NOTHROW(c.menuAdmin());
    }
}
