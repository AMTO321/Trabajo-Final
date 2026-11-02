#ifndef LISTADOBLE_H
#define LISTADOBLE_H

#include "Nodo.h"
#include <functional>
#include <iostream>

template <typename T>
class ListaDoble {
private:
    Nodo<T>* cabeza;
    Nodo<T>* cola;
    size_t tamanio;

public:
    ListaDoble() : cabeza(nullptr), cola(nullptr), tamanio(0) {}

    Nodo<T>* getCabeza() const { return cabeza; }
    void setCabeza(Nodo<T>* newCabeza) { cabeza = newCabeza; }

    Nodo<T>* getCola() const { return cola; }
    void setCola(Nodo<T>* newCola) { cola = newCola; }

    size_t getTamanio() const { return tamanio; }
    void setTamanio(size_t newTamanio) { tamanio = newTamanio; }

    void insertar(T dato) {
        Nodo<T>* nuevo = new Nodo<T>(dato);
        if (!cabeza) {
            cabeza = cola = nuevo;
        } else {
            cola->setSiguiente(nuevo);
            nuevo->setAnterior(cola);
            cola = nuevo;
        }
        tamanio++;
    }

    T buscar(std::function<bool(T)> criterio) {
        Nodo<T>* actual = cabeza;
        while (actual) {
            if (criterio(actual->getDato())) {
                return actual->getDato();
            }
            actual = actual->getSiguiente();
        }
        return T(); 
    }

    void eliminar(std::function<bool(T)> criterio) {
        Nodo<T>* actual = cabeza;
        while (actual) {
            if (criterio(actual->getDato())) {
                if (actual == cabeza) {
                    cabeza = cabeza->getSiguiente();
                    if (cabeza) cabeza->setAnterior(nullptr);
                    else cola = nullptr;
                } else if (actual == cola) {
                    cola = cola->getAnterior();
                    if (cola) cola->setSiguiente(nullptr);
                } else {
                    actual->getAnterior()->setSiguiente(actual->getSiguiente());
                    actual->getSiguiente()->setAnterior(actual->getAnterior());
                }
                delete actual;
                tamanio--;
                return;
            }
            actual = actual->getSiguiente();
        }
    }

    void recorrer(std::function<void(T)> accion) const {
        Nodo<T>* actual = cabeza;
        while (actual) {
            accion(actual->getDato());
            actual = actual->getSiguiente();
        }
    }
};

#endif