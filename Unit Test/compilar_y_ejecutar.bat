@echo off
REM Ejecutar desde la carpeta "pruebas" (que debe estar dentro de la carpeta del codigo).
REM No compila main.cpp (las pruebas traen su propio main en test_main.cpp).
g++ -std=c++17 -Wall -I.. ^
    test_main.cpp test_Nodo.cpp test_ListaDoble.cpp test_Validacion.cpp ^
    test_IntervaloHorario.cpp test_Profesor_Aula_Registro.cpp test_Controlador.cpp ^
    ..\validacion.cpp ..\Profesor.cpp ..\Aula.cpp ..\IntervaloHorario.cpp ^
    ..\RegistroAsistencia.cpp ..\controlador.cpp ^
    -o pruebas.exe
if errorlevel 1 (
    echo.
    echo *** ERROR DE COMPILACION ***
    exit /b 1
)
pruebas.exe
