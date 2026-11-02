#include "mini_test.h"
#include "Nodo.h"
#include "Profesor.h"
#include <string>

TEST_CASE("Nodo: el constructor guarda el dato y deja los enlaces en nullptr") {
    Nodo<int> n(42);
    CHECK_EQ(n.getDato(), 42);
    CHECK(n.getSiguiente() == nullptr);
    CHECK(n.getAnterior() == nullptr);
}

TEST_CASE("Nodo: setDato reemplaza el dato") {
    Nodo<std::string> n("antes");
    n.setDato("despues");
    CHECK_EQ(n.getDato(), std::string("despues"));
}

TEST_CASE("Nodo: setSiguiente y setAnterior enlazan dos nodos en ambos sentidos") {
    Nodo<int> a(1), b(2);
    a.setSiguiente(&b);
    b.setAnterior(&a);
    CHECK(a.getSiguiente() == &b);
    CHECK(b.getAnterior() == &a);
    CHECK(a.getAnterior() == nullptr);
    CHECK(b.getSiguiente() == nullptr);
}

TEST_CASE("Nodo: los enlaces se pueden volver a poner en nullptr") {
    Nodo<int> a(1), b(2);
    a.setSiguiente(&b);
    a.setSiguiente(nullptr);
    CHECK(a.getSiguiente() == nullptr);
}

TEST_CASE("Nodo: funciona con objetos (Profesor) y conserva sus campos") {
    Profesor p("0605293885", "Adrian", "Toapanta");
    Nodo<Profesor> n(p);
    CHECK_EQ(n.getDato().getCedula(), std::string("0605293885"));
    CHECK_EQ(n.getDato().getNombres(), std::string("Adrian"));
    CHECK_EQ(n.getDato().getApellidos(), std::string("Toapanta"));
}
