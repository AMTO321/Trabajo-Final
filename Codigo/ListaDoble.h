#ifndef LISTADOBLE_H
#define LISTADOBLE_H

#include "Nodo.h"
#include <cstddef>
#include <functional>
#include <utility>

template <typename T>
class ListaDoble {
private:
    Nodo<T>* cabeza;
    Nodo<T>* cola;
    size_t tamanio;

    void copiarDe(const ListaDoble<T>& otra) {
        Nodo<T>* actual = otra.cabeza;
        while (actual) {
            insertar(actual->getDato());
            actual = actual->getSiguiente();
        }
    }

public:
    ListaDoble() : cabeza(nullptr), cola(nullptr), tamanio(0) {}

    ListaDoble(const ListaDoble<T>& otra) : cabeza(nullptr), cola(nullptr), tamanio(0) {
        copiarDe(otra);
    }

    ListaDoble(ListaDoble<T>&& otra) noexcept
        : cabeza(otra.cabeza), cola(otra.cola), tamanio(otra.tamanio) {
        otra.cabeza = otra.cola = nullptr;
        otra.tamanio = 0;
    }

    ListaDoble<T>& operator=(const ListaDoble<T>& otra) {
        if (this != &otra) {
            vaciar();
            copiarDe(otra);
        }
        return *this;
    }

    ListaDoble<T>& operator=(ListaDoble<T>&& otra) noexcept {
        if (this != &otra) {
            vaciar();
            cabeza = otra.cabeza;
            cola = otra.cola;
            tamanio = otra.tamanio;
            otra.cabeza = otra.cola = nullptr;
            otra.tamanio = 0;
        }
        return *this;
    }

    ~ListaDoble() { vaciar(); }

    Nodo<T>* getCabeza() const { return cabeza; }
    Nodo<T>* getCola() const { return cola; }
    size_t getTamanio() const { return tamanio; }
    bool estaVacia() const { return tamanio == 0; }

    void insertar(const T& dato) {
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


    T buscar(std::function<bool(const T&)> criterio) const {
        Nodo<T>* actual = cabeza;
        while (actual) {
            if (criterio(actual->getDato())) return actual->getDato();
            actual = actual->getSiguiente();
        }
        return T();
    }

    
    T* buscarPtr(std::function<bool(const T&)> criterio) {
        Nodo<T>* actual = cabeza;
        while (actual) {
            if (criterio(*actual->getDatoPtr())) return actual->getDatoPtr();
            actual = actual->getSiguiente();
        }
        return nullptr;
    }

    const T* buscarPtr(std::function<bool(const T&)> criterio) const {
        Nodo<T>* actual = cabeza;
        while (actual) {
            if (criterio(*actual->getDatoPtr())) return actual->getDatoPtr();
            actual = actual->getSiguiente();
        }
        return nullptr;
    }

    T* obtenerPtr(size_t indice) {
        if (indice >= tamanio) return nullptr;
        Nodo<T>* actual = cabeza;
        for (size_t i = 0; i < indice; i++) actual = actual->getSiguiente();
        return actual->getDatoPtr();
    }

    bool existe(std::function<bool(const T&)> criterio) const {
        return buscarPtr(criterio) != nullptr;
    }

    size_t contar(std::function<bool(const T&)> criterio) const {
        size_t total = 0;
        Nodo<T>* actual = cabeza;
        while (actual) {
            if (criterio(actual->getDato())) total++;
            actual = actual->getSiguiente();
        }
        return total;
    }

    
    bool eliminar(std::function<bool(const T&)> criterio) {
        Nodo<T>* actual = cabeza;
        while (actual) {
            if (criterio(actual->getDato())) {
                if (actual == cabeza) {
                    cabeza = cabeza->getSiguiente();
                    if (cabeza) cabeza->setAnterior(nullptr);
                    else cola = nullptr;
                } else if (actual == cola) {
                    cola = cola->getAnterior();
                    cola->setSiguiente(nullptr);
                } else {
                    actual->getAnterior()->setSiguiente(actual->getSiguiente());
                    actual->getSiguiente()->setAnterior(actual->getAnterior());
                }
                delete actual;
                tamanio--;
                return true;
            }
            actual = actual->getSiguiente();
        }
        return false;
    }

    void recorrer(std::function<void(const T&)> accion) const {
        Nodo<T>* actual = cabeza;
        while (actual) {
            accion(actual->getDato());
            actual = actual->getSiguiente();
        }
    }

    void ordenar(std::function<bool(const T&, const T&)> menorQue) {
        if (!cabeza) return;
        for (Nodo<T>* i = cabeza->getSiguiente(); i; i = i->getSiguiente()) {
            T clave = i->getDato();
            Nodo<T>* j = i->getAnterior();
            while (j && menorQue(clave, j->getDato())) {
                j->getSiguiente()->setDato(j->getDato());
                j = j->getAnterior();
            }
            if (j) j->getSiguiente()->setDato(clave);
            else cabeza->setDato(clave);
        }
    }

    void vaciar() {
        Nodo<T>* actual = cabeza;
        while (actual) {
            Nodo<T>* siguiente = actual->getSiguiente();
            delete actual;
            actual = siguiente;
        }
        cabeza = cola = nullptr;
        tamanio = 0;
    }
};

#endif
