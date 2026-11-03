#ifndef PROFESOR_H
#define PROFESOR_H

#include <string>

class Profesor {
private:
    std::string cedula;
    std::string nombres;
    std::string apellidos;

public:
    Profesor();
    Profesor(std::string cedula, std::string nombres, std::string apellidos);

    std::string getCedula() const;
    void setCedula(std::string newCedula);

    std::string getNombres() const;
    void setNombres(std::string newNombres);

    std::string getApellidos() const;
    void setApellidos(std::string newApellidos);
};

#endif