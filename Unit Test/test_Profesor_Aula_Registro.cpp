#include "mini_test.h"
#include "Profesor.h"
#include "Aula.h"
#include "RegistroAsistencia.h"
#include <string>

// ---------- Profesor ----------

TEST_CASE("Profesor: el constructor por defecto deja los campos vacios") {
    Profesor p;
    CHECK_EQ(p.getCedula(), std::string(""));
    CHECK_EQ(p.getNombres(), std::string(""));
    CHECK_EQ(p.getApellidos(), std::string(""));
}

TEST_CASE("Profesor: el constructor con parametros guarda cedula, nombres y apellidos") {
    Profesor p("0605293885", "Adrian Michael", "Toapanta Orejuela");
    CHECK_EQ(p.getCedula(), std::string("0605293885"));
    CHECK_EQ(p.getNombres(), std::string("Adrian Michael"));
    CHECK_EQ(p.getApellidos(), std::string("Toapanta Orejuela"));
}

TEST_CASE("Profesor: los setters reemplazan cada campo sin afectar a los demas") {
    Profesor p("0605293885", "Adrian", "Toapanta");
    p.setNombres("Michael");
    CHECK_EQ(p.getNombres(), std::string("Michael"));
    CHECK_EQ(p.getCedula(), std::string("0605293885"));
    CHECK_EQ(p.getApellidos(), std::string("Toapanta"));
    p.setCedula("1710034065");
    p.setApellidos("Orejuela");
    CHECK_EQ(p.getCedula(), std::string("1710034065"));
    CHECK_EQ(p.getApellidos(), std::string("Orejuela"));
}

TEST_CASE("Profesor: una copia es independiente del original") {
    Profesor a("0605293885", "Adrian", "Toapanta");
    Profesor b = a;
    b.setNombres("Otro");
    CHECK_EQ(a.getNombres(), std::string("Adrian"));
    CHECK_EQ(b.getNombres(), std::string("Otro"));
}

// ---------- Aula (en el codigo actual representa un curso: NRC + materia + profesor) ----------

TEST_CASE("Aula: el constructor por defecto deja los campos vacios") {
    Aula a;
    CHECK_EQ(a.getNrc(), std::string(""));
    CHECK_EQ(a.getMateria(), std::string(""));
    CHECK_EQ(a.getCedulaProfesor(), std::string(""));
}

TEST_CASE("Aula: el constructor con parametros guarda nrc, materia y cedula del profesor") {
    Aula a("4", "edo", "0605293885");
    CHECK_EQ(a.getNrc(), std::string("4"));
    CHECK_EQ(a.getMateria(), std::string("edo"));
    CHECK_EQ(a.getCedulaProfesor(), std::string("0605293885"));
}

TEST_CASE("Aula: los setters reemplazan cada campo") {
    Aula a;
    a.setNrc("12");
    a.setMateria("Fisica");
    a.setCedulaProfesor("1710034065");
    CHECK_EQ(a.getNrc(), std::string("12"));
    CHECK_EQ(a.getMateria(), std::string("Fisica"));
    CHECK_EQ(a.getCedulaProfesor(), std::string("1710034065"));
}

TEST_CASE("Aula::asignarProfesor cambia la cedula del profesor asignado") {
    Aula a("4", "edo", "0605293885");
    a.asignarProfesor("1710034065");
    CHECK_EQ(a.getCedulaProfesor(), std::string("1710034065"));
    CHECK_EQ(a.getNrc(), std::string("4"));   // no toca los demas campos
}

// ---------- RegistroAsistencia: solo lo que no depende del reloj ni de archivos ----------

TEST_CASE("RegistroAsistencia::toString del registro por defecto") {
    RegistroAsistencia r;
    CHECK_EQ(r.toString(), std::string("Cedula:  | NRC:  | Estado:  | Hora: "));
}

TEST_CASE("RegistroAsistencia::toString con todos los datos") {
    RegistroAsistencia r("0605293885", "4", "PUNTUAL", "07:05");
    CHECK_EQ(r.toString(),
             std::string("Cedula: 0605293885 | NRC: 4 | Estado: PUNTUAL | Hora: 07:05"));
}

TEST_CASE("RegistroAsistencia::toString respeta el formato usado luego para leer asistencias.txt") {
    // registrarIngreso busca "Cedula: <cedula>" y "Hora: " en cada linea del archivo.
    RegistroAsistencia r("0605293885", "4", "ATRASO", "08:30");
    std::string linea = r.toString();
    CHECK(linea.find("Cedula: 0605293885") != std::string::npos);
    size_t pos = linea.find("Hora: ");
    REQUIRE(pos != std::string::npos);
    CHECK_EQ(linea.substr(pos + 6, 5), std::string("08:30"));
}

TEST_CASE("RegistroAsistencia::registrarSalida informa el NRC del registro") {
    RegistroAsistencia r("0605293885", "4", "PUNTUAL", "07:05");
    mini::CapturaSalida captura;
    r.registrarSalida();
    CHECK_EQ(captura.texto(), std::string("Salida registrada para el NRC: 4\n"));
}
