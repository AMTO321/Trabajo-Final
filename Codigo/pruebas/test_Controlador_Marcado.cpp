#include "mini_test.h"
#include "fixture.h"

// Una prueba por funcion publica del Controlador (profesor, pruebas de hora y menus).
// Datos base (datosBasicos): profesor 1 = 0605293885 con NRC 10001 (Lun 07-09) y 10002 (Lun 09-11);
//                            profesor 2 = 1710034065 con NRC 10003 (Mar 07-09).

static const std::string P1 = "0605293885\n";   // cedula del profesor 1

// Marca un ingreso/salida simulando teclado y reloj. Devuelve lo que se imprimio.
static std::string ingreso(Controlador& c, const std::string& fecha, int h, int m, const std::string& teclas) {
    RelojFijo reloj(fecha, h, m);
    Consola k(teclas);
    c.loginProfesor();
    return k.texto();
}
static std::string salida(Controlador& c, const std::string& fecha, int h, int m, const std::string& teclas) {
    RelojFijo reloj(fecha, h, m);
    Consola k(teclas);
    c.logoutProfesor();
    return k.texto();
}

// =====================================================================
//  loginProfesor (registro de ingreso con la hora actual)
// =====================================================================
TEST(Controlador_loginProfesor) {
    ArchivosTemporales tmp;
    {   // puntual: registra y guarda
        datosBasicos();
        Controlador c = crear();
        std::string s = ingreso(c, LUNES_FECHA, 7, 5, P1 + "1\n");
        CHECK_EQ(c.getRegistros().getTamanio(), 1u);
        const RegistroAsistencia& r = c.getRegistros().getCabeza()->getDato();
        CHECK_EQ(r.getFecha(), "2026-11-02");
        CHECK_EQ(r.getHora(), "07:05");
        CHECK_EQ(r.getNrc(), "10001");
        CHECK_EQ(r.getCedula(), "0605293885");
        CHECK_EQ(r.getTipo(), "ENTRADA");
        CHECK_EQ(r.getEstado(), "PUNTUAL");
        CHECK_EQ(ArchivosTemporales::leer("asistencias.txt"), "2026-11-02,07:05,0605293885,10001,ENTRADA,PUNTUAL\n");
        CHECK_CONTAINS(s, "Ingreso registrado a las 07:05. Estado: PUNTUAL.");
        CHECK_CONTAINS(s, "[SIMULADO] 2026-11-02 07:05 (LUNES)");   // muestra el reloj al inicio
    }
    {   // limite entre puntual (07:10) y atraso (07:11)
        ArchivosTemporales::borrar(); datosBasicos();
        Controlador c = crear();
        ingreso(c, LUNES_FECHA, 7, 0, P1 + "1\n");
        CHECK_EQ(c.getRegistros().getCola()->getDato().getEstado(), "PUNTUAL");

        ArchivosTemporales::borrar(); datosBasicos();
        Controlador d = crear();
        ingreso(d, LUNES_FECHA, 7, 10, P1 + "1\n");
        CHECK_EQ(d.getRegistros().getCola()->getDato().getEstado(), "PUNTUAL");

        ArchivosTemporales::borrar(); datosBasicos();
        Controlador e = crear();
        std::string s = ingreso(e, LUNES_FECHA, 7, 11, P1 + "1\n");
        CHECK_EQ(e.getRegistros().getCola()->getDato().getEstado(), "ATRASO");
        CHECK_CONTAINS(s, "con ATRASO");
    }
    {   // antes de la hora: rechaza
        ArchivosTemporales::borrar(); datosBasicos();
        Controlador c = crear();
        std::string s = ingreso(c, LUNES_FECHA, 6, 59, P1 + "1\n");
        CHECK(c.getRegistros().estaVacia());
        CHECK_CONTAINS(s, "aun no es hora. La clase inicia a las 07:00");
    }
    {   // ultimo minuto permitido (08:49) y plazo vencido (08:50)
        ArchivosTemporales::borrar(); datosBasicos();
        Controlador c = crear();
        std::string tarde = ingreso(c, LUNES_FECHA, 8, 50, P1 + "1\n");
        CHECK(c.getRegistros().estaVacia());
        CHECK_CONTAINS(tarde, "el plazo para registrar ingreso termino a las 08:49");
        ingreso(c, LUNES_FECHA, 8, 49, P1 + "1\n");
        CHECK_EQ(c.getRegistros().getTamanio(), 1u);
        CHECK_EQ(c.getRegistros().getCabeza()->getDato().getEstado(), "ATRASO");
    }
    {   // duplicado el mismo dia: rechaza
        ArchivosTemporales::borrar(); datosBasicos();
        Controlador c = crear();
        ingreso(c, LUNES_FECHA, 7, 5, P1 + "1\n");
        std::string s = ingreso(c, LUNES_FECHA, 7, 6, P1 + "1\n");
        CHECK_EQ(c.getRegistros().getTamanio(), 1u);
        CHECK_CONTAINS(s, "ya registro su ingreso hoy en el NRC 10001 a las 07:05");
    }
    {   // otro curso del mismo profesor es independiente; uno de otro horario aun no es hora
        ArchivosTemporales::borrar(); datosBasicos();
        Controlador c = crear();
        ingreso(c, LUNES_FECHA, 7, 5, P1 + "1\n");
        std::string s = ingreso(c, LUNES_FECHA, 7, 5, P1 + "2\n");   // NRC 10002 empieza a las 09:00
        CHECK_CONTAINS(s, "aun no es hora. La clase inicia a las 09:00");
        CHECK_EQ(c.getRegistros().getTamanio(), 1u);
        ingreso(c, LUNES_FECHA, 9, 3, P1 + "2\n");
        CHECK_EQ(c.getRegistros().getTamanio(), 2u);
        CHECK_EQ(c.getRegistros().getCola()->getDato().getNrc(), "10002");
    }
    {   // dia incorrecto y fin de semana
        ArchivosTemporales::borrar(); datosBasicos();
        Controlador c = crear();
        CHECK_CONTAINS(ingreso(c, MARTES_FECHA, 7, 5, P1 + "1\n"), "se dicta los LUNES y hoy es MARTES");
        CHECK_CONTAINS(ingreso(c, SABADO_FECHA, 7, 5, P1 + "1\n"), "fin de semana");
        CHECK(c.getRegistros().estaVacia());
    }
    {   // otro profesor: solo ve sus cursos
        ArchivosTemporales::borrar(); datosBasicos();
        Controlador c = crear();
        std::string s = ingreso(c, MARTES_FECHA, 7, 5, "1710034065\n1\n");
        CHECK_EQ(c.getRegistros().getTamanio(), 1u);
        CHECK_EQ(c.getRegistros().getCabeza()->getDato().getNrc(), "10003");
        CHECK_NOT_CONTAINS(s, "10001");
        CHECK_NOT_CONTAINS(s, "10002");
    }
    {   // dos profesores no pueden registrar ingreso a la misma hora el mismo dia
        ArchivosTemporales::borrar(); datosBasicos();
        ArchivosTemporales::escribir("asistencias.txt", "2026-11-02,07:05,1710034065,10003,ENTRADA,PUNTUAL\n");
        Controlador c = crear();
        std::string s = ingreso(c, LUNES_FECHA, 7, 10, P1 + "1\n");
        CHECK(c.getRegistros().getTamanio() == 1u);
        CHECK_CONTAINS(s, "El profesor Almeida Ana ya registro su ingreso hoy a las 07:05 (NRC 10003)");
        CHECK_CONTAINS(s, "Dos profesores no pueden coincidir el mismo dia a la misma hora");
    }
    {   // identificacion: cedula invalida, no registrada o cancelada
        ArchivosTemporales::borrar(); datosBasicos();
        Controlador c = crear();
        CHECK_CONTAINS(ingreso(c, LUNES_FECHA, 7, 5, "123\n"), "la cedula ingresada no es valida");
        CHECK_CONTAINS(ingreso(c, LUNES_FECHA, 7, 5, "1701234567\n"), "Profesor no encontrado en el sistema");
        ingreso(c, LUNES_FECHA, 7, 5, "0\n");
        ingreso(c, LUNES_FECHA, 7, 5, P1 + "0\n");       // cancela en la seleccion de curso
        CHECK(c.getRegistros().estaVacia());
    }
    {   // profesor sin cursos
        ArchivosTemporales::borrar(); datosBasicos();
        ArchivosTemporales::escribir("profesores.txt",
            "0605293885,Adrian Michael,Toa Orejuela\n1710034065,Ana,Almeida\n3000000012,Luis,Mora\n");
        Controlador c = crear();
        std::string s = ingreso(c, LUNES_FECHA, 7, 5, "3000000012\n");
        CHECK_CONTAINS(s, "No tiene cursos asignados.");
        CHECK(c.getRegistros().estaVacia());
    }
    {   // persiste entre ejecuciones y sigue sin dejar duplicar
        ArchivosTemporales::borrar(); datosBasicos();
        { Controlador c = crear(); ingreso(c, LUNES_FECHA, 7, 5, P1 + "1\n"); }
        Controlador otra = crear();
        CHECK_EQ(otra.getRegistros().getTamanio(), 1u);
        CHECK_CONTAINS(ingreso(otra, LUNES_FECHA, 7, 20, P1 + "1\n"), "ya registro su ingreso");
    }
}

// =====================================================================
//  logoutProfesor (registro de salida con la hora actual)
// =====================================================================
TEST(Controlador_logoutProfesor) {
    ArchivosTemporales tmp;
    {   // sin ingreso previo: rechaza
        datosBasicos();
        Controlador c = crear();
        std::string s = salida(c, LUNES_FECHA, 8, 55, P1 + "1\n");
        CHECK(c.getRegistros().estaVacia());
        CHECK_CONTAINS(s, "primero debe registrar su ingreso en el NRC 10001 hoy");
    }
    {   // ok: guarda el registro
        ArchivosTemporales::borrar(); datosBasicos();
        Controlador c = crear();
        ingreso(c, LUNES_FECHA, 7, 5, P1 + "1\n");
        std::string s = salida(c, LUNES_FECHA, 8, 55, P1 + "1\n");
        CHECK_EQ(c.getRegistros().getTamanio(), 2u);
        const RegistroAsistencia& r = c.getRegistros().getCola()->getDato();
        CHECK_EQ(r.getTipo(), "SALIDA");
        CHECK_EQ(r.getEstado(), "REGISTRADA");
        CHECK_EQ(r.getHora(), "08:55");
        CHECK_CONTAINS(s, "Salida registrada a las 08:55 para el NRC 10001");
        CHECK_CONTAINS(ArchivosTemporales::leer("asistencias.txt"), "2026-11-02,08:55,0605293885,10001,SALIDA,REGISTRADA");
    }
    {   // solo en los ultimos 10 minutos: 08:49 y 09:00 rechazan, 08:50 y 08:59 aceptan
        ArchivosTemporales::borrar(); datosBasicos();
        Controlador c = crear();
        ingreso(c, LUNES_FECHA, 7, 5, P1 + "1\n");
        CHECK_CONTAINS(salida(c, LUNES_FECHA, 8, 49, P1 + "1\n"), "la salida se registra entre 08:50 y 08:59");
        CHECK_CONTAINS(salida(c, LUNES_FECHA, 9, 0, P1 + "1\n"), "la salida se registra entre 08:50 y 08:59");
        CHECK_EQ(c.getRegistros().getTamanio(), 1u);
        salida(c, LUNES_FECHA, 8, 50, P1 + "1\n");
        CHECK_EQ(c.getRegistros().getTamanio(), 2u);

        ArchivosTemporales::borrar(); datosBasicos();
        Controlador d = crear();
        ingreso(d, LUNES_FECHA, 7, 5, P1 + "1\n");
        salida(d, LUNES_FECHA, 8, 59, P1 + "1\n");
        CHECK_EQ(d.getRegistros().getTamanio(), 2u);
    }
    {   // salida duplicada: rechaza
        ArchivosTemporales::borrar(); datosBasicos();
        Controlador c = crear();
        ingreso(c, LUNES_FECHA, 7, 5, P1 + "1\n");
        salida(c, LUNES_FECHA, 8, 55, P1 + "1\n");
        std::string s = salida(c, LUNES_FECHA, 8, 56, P1 + "1\n");
        CHECK_EQ(c.getRegistros().getTamanio(), 2u);
        CHECK_CONTAINS(s, "ya registro su salida hoy en el NRC 10001");
    }
    {   // dia incorrecto y fin de semana
        ArchivosTemporales::borrar(); datosBasicos();
        Controlador c = crear();
        CHECK_CONTAINS(salida(c, MARTES_FECHA, 8, 55, P1 + "1\n"), "se dicta los LUNES");
        CHECK_CONTAINS(salida(c, SABADO_FECHA, 8, 55, P1 + "1\n"), "fin de semana");
    }
    {   // el ingreso de un dia no vale para otro
        ArchivosTemporales::borrar(); datosBasicos();
        Controlador c = crear();
        ingreso(c, LUNES_FECHA, 7, 5, P1 + "1\n");
        CHECK_CONTAINS(salida(c, "2026-11-09", 8, 55, P1 + "1\n"), "primero debe registrar su ingreso");
    }
}

// =====================================================================
//  verMisCursos
// =====================================================================
TEST(Controlador_verMisCursos) {
    ArchivosTemporales tmp;
    {   // lista solo sus cursos
        datosBasicos();
        Controlador c = crear();
        Consola k(P1);
        c.verMisCursos();
        std::string s = k.texto();
        CHECK_CONTAINS(s, "NRC 10001 | Algebra Lineal | LUNES 07:00 - 09:00");
        CHECK_CONTAINS(s, "NRC 10002 | Ecuaciones Diferenciales | LUNES 09:00 - 11:00");
        CHECK_NOT_CONTAINS(s, "10003");
        CHECK_NOT_CONTAINS(s, "periodo");
    }
    {   // ordenados por dia y hora
        ArchivosTemporales::borrar(); datosBasicos();
        ArchivosTemporales::escribir("cursos.txt",
            "10001,ALGE101,0605293885,VIERNES,420,540\n"
            "10002,EDIF201,0605293885,LUNES,660,780\n"
            "10004,CALC101,0605293885,LUNES,420,540\n");
        Controlador c = crear();
        Consola k(P1);
        c.verMisCursos();
        std::string s = k.texto();
        CHECK(s.find("10004") < s.find("10002"));      // Lunes 07:00 antes que Lunes 11:00
        CHECK(s.find("10002") < s.find("10001"));      // Lunes antes que Viernes
    }
    {   // sin cursos, cancelado o cedula no registrada
        ArchivosTemporales::borrar(); datosBasicos();
        ArchivosTemporales::escribir("profesores.txt",
            "0605293885,Adrian Michael,Toa Orejuela\n1710034065,Ana,Almeida\n3000000012,Luis,Mora\n");
        Controlador c = crear();
        { Consola k("3000000012\n"); c.verMisCursos(); CHECK_CONTAINS(k.texto(), "No tiene cursos asignados."); }
        { Consola k("0\n"); c.verMisCursos(); CHECK_NOT_CONTAINS(k.texto(), "NRC"); }
        { Consola k("1701234567\n"); c.verMisCursos(); CHECK_CONTAINS(k.texto(), "Profesor no encontrado"); }
        { Consola k("123\n"); c.verMisCursos(); CHECK_CONTAINS(k.texto(), "la cedula ingresada no es valida"); }
    }
}

// =====================================================================
//  pruebaLogin / pruebaLogout
//  Curso 10001 (prof 1): Lunes 07:00-09:00 -> a tiempo 07:00-07:10, con atraso hasta 08:49,
//                        deslogin (salida) 08:50-08:59.
//  Teclas despues de cedula y curso: hora ficticia; luego el submenu (1 repetir, 2 otro curso, 3 volver).
//  Es una prueba: no guarda nada.
// =====================================================================
static std::string correrPruebaLogin(const std::string& teclas) {
    Consola k(P1 + "1\n" + teclas);
    Controlador c = crear();
    c.pruebaLogin();
    return k.texto();
}
static std::string correrPruebaLogout(const std::string& teclas) {
    Consola k(P1 + "1\n" + teclas);
    Controlador c = crear();
    c.pruebaLogout();
    return k.texto();
}

TEST(Controlador_pruebaLogin) {
    ArchivosTemporales tmp;
    datosBasicos();
    {   // ingreso a tiempo, hora escrita por teclado
        std::string s = correrPruebaLogin("7:05\n3\n");
        CHECK_CONTAINS(s, "prueba de login");
        CHECK_CONTAINS(s, "Ingreso a tiempo : 07:00 a 07:10");
        CHECK_CONTAINS(s, "Ingreso con atraso: hasta las 08:49");
        CHECK_CONTAINS(s, "Hora de ingreso : ");
        CHECK_CONTAINS(s, "07:05  ingreso a tiempo .");
        CHECK_CONTAINS(s, "Ingreso : 07:05 -> a tiempo");
    }
    {   // acepta las 7 am y varios formatos de hora
        CHECK_CONTAINS(correrPruebaLogin("7\n3\n"), "07:00  ingreso a tiempo");
        CHECK_CONTAINS(correrPruebaLogin("07:00\n3\n"), "07:00  ingreso a tiempo");
        CHECK_CONTAINS(correrPruebaLogin("7am\n3\n"), "07:00  ingreso a tiempo");
        CHECK_CONTAINS(correrPruebaLogin("7:05 AM\n3\n"), "07:05  ingreso a tiempo");
    }
    {   // limite de los primeros 10 minutos: 07:10 a tiempo, 07:11 con atraso
        CHECK_CONTAINS(correrPruebaLogin("7:10\n3\n"), "07:10  ingreso a tiempo");
        std::string s = correrPruebaLogin("7:11\n3\n");
        CHECK_CONTAINS(s, "07:11  ingreso con atraso.");
        CHECK_CONTAINS(s, "Ingreso : 07:11 -> con atraso");
        CHECK_NOT_CONTAINS(s, "ingreso a tiempo .");
    }
    {   // ingreso fuera del plazo: pide otra hora
        std::string s = correrPruebaLogin("8:50\n10am\n8:49\n3\n");
        CHECK_CONTAINS(s, "08:50  el plazo para registrar ingreso termino a las 08:49. Quedaria como ausente.");
        CHECK_CONTAINS(s, "10:00  el plazo para registrar ingreso termino");
        CHECK_CONTAINS(s, "Intente con otra hora de ingreso.");
        CHECK_CONTAINS(s, "08:49  ingreso con atraso.");
    }
    {   // solo se permiten horas entre 07:00 y 21:00
        std::string s = correrPruebaLogin("21:01\n9:30pm\n22:00\n06:59\n5am\n0\n");
        CHECK_CONTAINS(s, "el horario de clases va de 07:00 a 21:00");
        CHECK_NOT_CONTAINS(s, "ingreso a tiempo .");
        CHECK_NOT_CONTAINS(s, "ingreso con atraso.");
        std::string t = correrPruebaLogin("9pm\n0\n");                 // 21:00 si es ingresable
        CHECK_NOT_CONTAINS(t, "el horario de clases va de");
        CHECK_CONTAINS(t, "21:00  el plazo para registrar ingreso termino");
    }
    {   // hora con formato invalido: repite
        std::string s = correrPruebaLogin("abc\n25:00\n7:5\n\n0\n");
        CHECK_CONTAINS(s, "hora invalida");
        CHECK_NOT_CONTAINS(s, "ingreso a tiempo .");
    }
    {   // repetir la prueba con este curso (opcion 1)
        std::string s = correrPruebaLogin("7:05\n1\n7:30\n3\n");
        CHECK_CONTAINS(s, "Ingreso : 07:05 -> a tiempo");
        CHECK_CONTAINS(s, "Ingreso : 07:30 -> con atraso");
    }
    {   // probar con otro curso (opcion 2)
        std::string s = correrPruebaLogin("7:05\n2\n2\n9:05\n3\n");
        CHECK_CONTAINS(s, "NRC 10002 - Ecuaciones Diferenciales");
        CHECK_CONTAINS(s, "Ingreso a tiempo : 09:00 a 09:10");
        CHECK_CONTAINS(s, "09:05  ingreso a tiempo");
    }
    {   // antes de la hora de inicio del otro curso: aun no es hora
        std::string s = correrPruebaLogin("7:05\n2\n2\n8:00\n0\n");
        CHECK_CONTAINS(s, "08:00  aun no es hora. La clase inicia a las 09:00.");
    }
    {   // opcion 2 pero cancelando la eleccion del curso: sigue con el mismo
        std::string s = correrPruebaLogin("7:05\n2\n0\n0\n");
        CHECK_CONTAINS(s, "Hora de ingreso : ");
    }
    {   // submenu fuera de rango: repite
        CHECK_CONTAINS(correrPruebaLogin("7:05\n5\n3\n"), "el valor debe estar entre 1 y 3");
    }
    {   // no guarda nada
        Controlador c = crear();
        Consola k(P1 + "1\n7:05\n3\n");
        c.pruebaLogin();
        CHECK(c.getRegistros().estaVacia());
        CHECK_FALSE(ArchivosTemporales::existe("asistencias.txt"));
    }
    {   // cancelar / identificacion
        Controlador c = crear();
        { Consola k("0\n"); c.pruebaLogin(); }                                   // cedula
        { Consola k(P1 + "0\n"); c.pruebaLogin(); }                              // curso
        { Consola k(P1 + "1\n0\n"); c.pruebaLogin(); }                           // hora
        { Consola k("1701234567\n"); c.pruebaLogin(); CHECK_CONTAINS(k.texto(), "Profesor no encontrado"); }
        { Consola k("123\n"); c.pruebaLogin(); CHECK_CONTAINS(k.texto(), "la cedula ingresada no es valida"); }
    }
}

TEST(Controlador_pruebaLogout) {
    ArchivosTemporales tmp;
    datosBasicos();
    {   // salida a tiempo, hora escrita por teclado
        std::string s = correrPruebaLogout("8:55\n3\n");
        CHECK_CONTAINS(s, "prueba de deslogin");
        CHECK_CONTAINS(s, "Salida a tiempo : 08:50 a 08:59");
        CHECK_CONTAINS(s, "Hora de salida  : ");
        CHECK_CONTAINS(s, "08:55  salida a tiempo .");
        CHECK_CONTAINS(s, "Salida  : 08:55 a tiempo");
    }
    {   // solo los ultimos 10 minutos: 08:49 y 09:00 rechazan; 08:50 y 08:59 aceptan
        std::string s = correrPruebaLogout("8:49\n9am\n8:50\n3\n");
        CHECK_CONTAINS(s, "08:49  la salida se registra entre 08:50 y 08:59.");
        CHECK_CONTAINS(s, "09:00  la salida se registra entre 08:50 y 08:59.");
        CHECK_CONTAINS(s, "Intente con otra hora de salida.");
        CHECK_CONTAINS(s, "08:50  salida a tiempo");
        CHECK_CONTAINS(correrPruebaLogout("8:59\n3\n"), "08:59  salida a tiempo");
        CHECK_CONTAINS(correrPruebaLogout("7:05\n0\n"), "07:05  la salida se registra entre 08:50 y 08:59.");
    }
    {   // solo se permiten horas entre 07:00 y 21:00
        std::string s = correrPruebaLogout("21:01\n9:30pm\n06:59\n0\n");
        CHECK_CONTAINS(s, "el horario de clases va de 07:00 a 21:00");
        CHECK_NOT_CONTAINS(s, "salida a tiempo .");
    }
    {   // hora con formato invalido: repite
        std::string s = correrPruebaLogout("abc\n8:5\n0\n");
        CHECK_CONTAINS(s, "hora invalida");
        CHECK_NOT_CONTAINS(s, "salida a tiempo .");
    }
    {   // probar con otro curso (opcion 2) y repetir (opcion 1)
        std::string s = correrPruebaLogout("8:55\n2\n2\n10:55\n1\n10:50\n3\n");
        CHECK_CONTAINS(s, "Salida a tiempo : 10:50 a 10:59");
        CHECK_CONTAINS(s, "10:55  salida a tiempo");
        CHECK_CONTAINS(s, "10:50  salida a tiempo");
    }
    {   // no guarda nada
        Controlador c = crear();
        Consola k(P1 + "1\n8:55\n3\n");
        c.pruebaLogout();
        CHECK(c.getRegistros().estaVacia());
        CHECK_FALSE(ArchivosTemporales::existe("asistencias.txt"));
    }
    {   // cancelar / identificacion
        Controlador c = crear();
        { Consola k("0\n"); c.pruebaLogout(); }
        { Consola k(P1 + "0\n"); c.pruebaLogout(); }
        { Consola k(P1 + "1\n0\n"); c.pruebaLogout(); }
        { Consola k("1701234567\n"); c.pruebaLogout(); CHECK_CONTAINS(k.texto(), "Profesor no encontrado"); }
    }
}

// =====================================================================
//  menuHorasDePrueba
// =====================================================================
TEST(Controlador_menuHorasDePrueba) {
    ArchivosTemporales tmp;
    datosBasicos();
    {   // opciones del menu
        Controlador c = crear();
        Consola k("3\n");
        c.menuHorasDePrueba();
        CHECK_CONTAINS(k.texto(), "1. Prueba de login");
        CHECK_CONTAINS(k.texto(), "2. Prueba de deslogin");
        CHECK_CONTAINS(k.texto(), "3. Volver");
    }
    {   // opcion 1 abre la prueba de login, opcion 2 la de deslogin
        Controlador c = crear();
        Consola k("1\n" + P1 + "1\n7:05\n3\n" + "2\n" + P1 + "1\n8:55\n3\n" + "3\n");
        c.menuHorasDePrueba();
        CHECK_CONTAINS(k.texto(), "prueba de login");
        CHECK_CONTAINS(k.texto(), "07:05  ingreso a tiempo");
        CHECK_CONTAINS(k.texto(), "prueba de deslogin");
        CHECK_CONTAINS(k.texto(), "08:55  salida a tiempo");
        CHECK(c.getRegistros().estaVacia());
    }
    {   // opcion fuera de rango: repite
        Controlador c = crear();
        Consola k("4\n0\nabc\n3\n");
        c.menuHorasDePrueba();
        CHECK_CONTAINS(k.texto(), "el valor debe estar entre 1 y 3");
    }
}

// =====================================================================
//  menuListados
// =====================================================================
TEST(Controlador_menuListados) {
    ArchivosTemporales tmp;
    datosBasicos();
    {   // opciones y cada listado
        Controlador c = crear();
        Consola k("1\n2\n3\n4\n");
        c.menuListados();
        std::string s = k.texto();
        CHECK_CONTAINS(s, "1. Profesores");
        CHECK_CONTAINS(s, "2. Materias");
        CHECK_CONTAINS(s, "3. Cursos (NRC)");
        CHECK_CONTAINS(s, "4. Volver");
        CHECK_NOT_CONTAINS(s, "Periodos");
        CHECK_CONTAINS(s, "Profesores (2)");
        CHECK_CONTAINS(s, "Materias (3)");
        CHECK_CONTAINS(s, "Cursos (3)");
    }
    {   // opcion fuera de rango: repite
        Controlador c = crear();
        Consola k("5\n0\n4\n");
        c.menuListados();
        CHECK_CONTAINS(k.texto(), "el valor debe estar entre 1 y 4");
    }
}

// =====================================================================
//  menuAdmin
// =====================================================================
TEST(Controlador_menuAdmin) {
    ArchivosTemporales tmp;
    {   // opciones simplificadas, sin las eliminadas
        Controlador c = crear();
        Consola k("6\n");
        c.menuAdmin();
        std::string s = k.texto();
        CHECK_CONTAINS(s, "1. Registrar profesor");
        CHECK_CONTAINS(s, "2. Registrar curso");
        CHECK_CONTAINS(s, "3. Eliminar curso");
        CHECK_CONTAINS(s, "4. Eliminar profesor");
        CHECK_CONTAINS(s, "5. Listados");
        CHECK_CONTAINS(s, "6. Volver al menu principal");
        CHECK_NOT_CONTAINS(s, "Cambiar horario");
        CHECK_NOT_CONTAINS(s, "Reporte de asistencias");
        CHECK_NOT_CONTAINS(s, "Registrar materia");
        CHECK_NOT_CONTAINS(s, "eriodo");
        CHECK_NOT_CONTAINS(s, "7. ");
    }
    {   // opcion fuera de rango: repite
        ArchivosTemporales::borrar();
        Controlador c = crear();
        Consola k("7\n0\nabc\n6\n");
        c.menuAdmin();
        CHECK_CONTAINS(k.texto(), "el valor debe estar entre 1 y 6");
    }
    {   // cada opcion llama a su funcion
        ArchivosTemporales::borrar();
        datosBasicos();
        Controlador c = crear();
        Consola k("1\n0\n" "2\n0\n" "3\n0\n" "4\n0\n" "5\n4\n" "6\n");
        c.menuAdmin();
        std::string s = k.texto();
        CHECK_CONTAINS(s, "Registrar nuevo profesor");
        CHECK_CONTAINS(s, "Registrar Curso");
        CHECK_CONTAINS(s, "eliminar curso");
        CHECK_CONTAINS(s, "Eliminar profesor");
        CHECK_CONTAINS(s, "Listados");
    }
    {   // crear un curso desde el menu
        ArchivosTemporales::borrar();
        datosSinCursos();
        Controlador c = crear();
        Consola k("2\n4\n1\n10001\n1\n1\ns\n6\n");
        c.menuAdmin();
        CHECK_EQ(c.getCursos().getTamanio(), 1u);
        CHECK_CONTAINS(k.texto(), "NRC 10001");
    }
}

// =====================================================================
//  menuProfesor
// =====================================================================
TEST(Controlador_menuProfesor) {
    ArchivosTemporales tmp;
    {   // opciones y navegacion: ver cursos, registrar ingreso (NRC 10001), registrar salida sin ingreso y salir
        datosBasicos();
        Controlador c = crear();
        RelojFijo reloj(LUNES_FECHA, 7, 5);
        Consola k("3\n" + P1 + "1\n" + P1 + "1\n" + "5\n");
        c.menuProfesor();
        std::string s = k.texto();
        CHECK_CONTAINS(s, "1. Registrar ingreso");
        CHECK_CONTAINS(s, "2. Registrar salida");
        CHECK_CONTAINS(s, "3. Ver mis cursos");
        CHECK_CONTAINS(s, "4. Horas de prueba");
        CHECK_CONTAINS(s, "5. Volver al menu principal");
        CHECK_CONTAINS(s, "Mis cursos");
        CHECK_EQ(c.getRegistros().getTamanio(), 1u);
    }
    {   // la opcion 2 llama a la salida
        ArchivosTemporales::borrar(); datosBasicos();
        Controlador c = crear();
        RelojFijo reloj(LUNES_FECHA, 8, 55);
        Consola k("2\n" + P1 + "1\n5\n");
        c.menuProfesor();
        CHECK_CONTAINS(k.texto(), "primero debe registrar su ingreso");
    }
    {   // la opcion 4 abre el menu de horas de prueba y no registra nada real
        ArchivosTemporales::borrar(); datosBasicos();
        Controlador c = crear();
        Consola k("4\n1\n" + P1 + "1\n7:05\n3\n3\n5\n");
        c.menuProfesor();
        CHECK_CONTAINS(k.texto(), "Horas de prueba");
        CHECK_CONTAINS(k.texto(), "07:05  ingreso a tiempo");
        CHECK(c.getRegistros().estaVacia());
    }
    {   // opcion fuera de rango: repite; no muestra reloj simulado
        ArchivosTemporales::borrar();
        Controlador c = crear();
        Consola k("6\n5\n");
        c.menuProfesor();
        CHECK_CONTAINS(k.texto(), "el valor debe estar entre 1 y 5");
        CHECK_NOT_CONTAINS(k.texto(), "SIMULADO");
    }
}
