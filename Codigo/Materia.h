#ifndef MATERIA_H
#define MATERIA_H

#include <string>


class Materia {
private:
    std::string id;
    std::string nombre;
    int creditos;

public:
    Materia();
    Materia(std::string id, std::string nombre, int creditos);

    std::string getId() const;
    void setId(std::string newId);

    std::string getNombre() const;
    void setNombre(std::string newNombre);

    int getCreditos() const;
    void setCreditos(int newCreditos);
};

#endif
