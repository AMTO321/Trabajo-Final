#ifndef NODO_H
#define NODO_H

template <typename T>
class Nodo {
private:
    T dato;
    Nodo<T>* siguiente;
    Nodo<T>* anterior;

public:
    Nodo(T dato) : dato(dato), siguiente(nullptr), anterior(nullptr) {}

    T getDato() const { return dato; }
    void setDato(T newDato) { dato = newDato; }

    Nodo<T>* getSiguiente() const { return siguiente; }
    void setSiguiente(Nodo<T>* newSiguiente) { siguiente = newSiguiente; }

    Nodo<T>* getAnterior() const { return anterior; }
    void setAnterior(Nodo<T>* newAnterior) { anterior = newAnterior; }
};

#endif