#include "validacion.h"
#include <cctype>

std::string Validacion::trim(const std::string& texto) {
    size_t inicio = 0;
    size_t fin = texto.size();
    while (inicio < fin && std::isspace(static_cast<unsigned char>(texto[inicio]))) inicio++;
    while (fin > inicio && std::isspace(static_cast<unsigned char>(texto[fin - 1]))) fin--;
    return texto.substr(inicio, fin - inicio);
}

std::string Validacion::normalizarEspacios(const std::string& texto) {
    std::string limpio = trim(texto);
    std::string resultado;
    bool espacioPrevio = false;
    for (size_t i = 0; i < limpio.size(); i++) {
        char c = limpio[i];
        if (c == ' ' || c == '\t') {
            if (!espacioPrevio) resultado += ' ';
            espacioPrevio = true;
        } else {
            resultado += c;
            espacioPrevio = false;
        }
    }
    return resultado;
}

std::string Validacion::aMayusculas(const std::string& texto) {
    std::string r = texto;
    for (size_t i = 0; i < r.size(); i++) {
        unsigned char c = static_cast<unsigned char>(r[i]);
        if (c < 0x80) r[i] = static_cast<char>(std::toupper(c));
    }
    return r;
}

std::vector<std::string> Validacion::dividir(const std::string& linea, char separador) {
    std::vector<std::string> partes;
    std::string actual;
    for (size_t i = 0; i < linea.size(); i++) {
        if (linea[i] == separador) {
            partes.push_back(trim(actual));
            actual.clear();
        } else {
            actual += linea[i];
        }
    }
    partes.push_back(trim(actual));
    return partes;
}

bool Validacion::soloNumeros(const std::string& texto) {
    if (texto.empty()) return false;
    for (size_t i = 0; i < texto.size(); i++) {
        if (!std::isdigit(static_cast<unsigned char>(texto[i]))) return false;
    }
    return true;
}

bool Validacion::soloLetras(const std::string& texto) {
    if (texto.empty()) return false;
    bool hayLetra = false;
    for (size_t i = 0; i < texto.size(); i++) {
        unsigned char c = static_cast<unsigned char>(texto[i]);
        if (c >= 0x80) {            // bytes de tildes y enie (UTF-8 / ANSI)
            hayLetra = true;
        } else if (std::isalpha(c)) {
            hayLetra = true;
        } else if (c != ' ') {
            return false;
        }
    }
    return hayLetra;
}

bool Validacion::validarNombrePersona(const std::string& texto) {
    return texto.size() >= 2 && texto.size() <= 50 && soloLetras(texto);
}

bool Validacion::validarTextoLibre(const std::string& texto, size_t minimo, size_t maximo) {
    if (texto.size() < minimo || texto.size() > maximo) return false;
    bool hayAlfanumerico = false;
    for (size_t i = 0; i < texto.size(); i++) {
        unsigned char c = static_cast<unsigned char>(texto[i]);
        if (c >= 0x80 || std::isalnum(c)) hayAlfanumerico = true;
        else if (c != ' ') return false;
    }
    return hayAlfanumerico;
}

bool Validacion::validarCedula(const std::string& cedula) {
    if (cedula.length() != 10 || !soloNumeros(cedula)) {
        return false;
    }

    int provincia = std::stoi(cedula.substr(0, 2));
    bool provinciaValida = (provincia >= 1 && provincia <= 24) || provincia == 30;
    if (!provinciaValida) {
        return false;
    }

    int tercerDigito = cedula[2] - '0';
    if (tercerDigito >= 6) {
        return false;
    }

    int coeficientes[9] = {2, 1, 2, 1, 2, 1, 2, 1, 2};
    int digitoVerificador = cedula[9] - '0';
    int suma = 0;

    for (int i = 0; i < 9; i++) {
        int valor = (cedula[i] - '0') * coeficientes[i];
        if (valor >= 10) {
            valor -= 9;
        }
        suma += valor;
    }

    int residuo = suma % 10;
    int resultadoVerificador = (residuo == 0) ? 0 : (10 - residuo);

    return resultadoVerificador == digitoVerificador;
}

bool Validacion::validarIdMateria(const std::string& id) {
    if (id.size() < 4 || id.size() > 10) return false;
    for (size_t i = 0; i < id.size(); i++) {
        unsigned char c = static_cast<unsigned char>(id[i]);
        if (!(std::isdigit(c) || (c >= 'A' && c <= 'Z'))) return false;
    }
    return true;
}

bool Validacion::validarNrc(const std::string& nrc) {
    return nrc.size() == 5 && soloNumeros(nrc) && std::stoi(nrc) >= 10001;
}

bool Validacion::validarFecha(const std::string& fecha) {
    if (fecha.size() != 10 || fecha[4] != '-' || fecha[7] != '-') return false;
    std::string a = fecha.substr(0, 4), m = fecha.substr(5, 2), d = fecha.substr(8, 2);
    if (!soloNumeros(a) || !soloNumeros(m) || !soloNumeros(d)) return false;
    int anio = std::stoi(a), mes = std::stoi(m), dia = std::stoi(d);
    if (anio < 2000 || anio > 2100 || mes < 1 || mes > 12 || dia < 1) return false;
    int diasMes[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool bisiesto = (anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0);
    int maximo = diasMes[mes - 1] + ((mes == 2 && bisiesto) ? 1 : 0);
    return dia <= maximo;
}

bool Validacion::parsearHora(const std::string& texto, int& minutos) {
    if (texto.size() != 5 || texto[2] != ':') return false;
    std::string h = texto.substr(0, 2), m = texto.substr(3, 2);
    if (!soloNumeros(h) || !soloNumeros(m)) return false;
    int hh = std::stoi(h), mm = std::stoi(m);
    if (hh > 23 || mm > 59) return false;
    minutos = hh * 60 + mm;
    return true;
}
