#include "mini_test.h"
#include "../ListaDoble.h"
#include <utility>
#include <vector>

// Una prueba por funcion (Nodo y ListaDoble).

static ListaDoble<int> lista(std::initializer_list<int> valores) {
    ListaDoble<int> l;
    for (std::initializer_list<int>::iterator it = valores.begin(); it != valores.end(); ++it) l.insertar(*it);
    return l;
}

static std::vector<int> aVector(const ListaDoble<int>& l) {
    std::vector<int> v;
    l.recorrer([&v](const int& x) { v.push_back(x); });
    return v;
}

// recorre hacia atras con los punteros "anterior" para validar los enlaces dobles
static std::vector<int> aVectorInverso(const ListaDoble<int>& l) {
    std::vector<int> v;
    for (Nodo<int>* n = l.getCola(); n; n = n->getAnterior()) v.push_back(n->getDato());
    return v;
}

// =====================================================================
//  Nodo
// =====================================================================
TEST(Nodo_constructor) {
    Nodo<int> n(5);
    CHECK_EQ(n.getDato(), 5);
    CHECK(n.getSiguiente() == nullptr);
    CHECK(n.getAnterior() == nullptr);
}

TEST(Nodo_getDato) {
    Nodo<int> n(8);
    CHECK_EQ(n.getDato(), 8);
}

TEST(Nodo_getDatoPtr) {
    Nodo<int> n(8);
    *n.getDatoPtr() = 9;
    CHECK_EQ(n.getDato(), 9);
}

TEST(Nodo_setDato) {
    Nodo<int> n(1);
    n.setDato(42);
    CHECK_EQ(n.getDato(), 42);
}

TEST(Nodo_getSiguiente_setSiguiente) {
    Nodo<int> a(1), b(2);
    a.setSiguiente(&b);
    CHECK(a.getSiguiente() == &b);
    a.setSiguiente(nullptr);
    CHECK(a.getSiguiente() == nullptr);
}

TEST(Nodo_getAnterior_setAnterior) {
    Nodo<int> a(1), b(2);
    b.setAnterior(&a);
    CHECK(b.getAnterior() == &a);
    b.setAnterior(nullptr);
    CHECK(b.getAnterior() == nullptr);
}

// =====================================================================
//  ListaDoble
// =====================================================================
TEST(ListaDoble_constructor) {
    ListaDoble<int> l;
    CHECK(l.estaVacia());
    CHECK_EQ(l.getTamanio(), 0u);
    CHECK(l.getCabeza() == nullptr);
    CHECK(l.getCola() == nullptr);
}

TEST(ListaDoble_constructorCopia) {
    ListaDoble<int> original = lista({1, 2, 3});
    ListaDoble<int> copia(original);
    *copia.obtenerPtr(0) = 99;
    CHECK_EQ(*original.obtenerPtr(0), 1);          // la copia es profunda: el original no cambia
    CHECK_EQ(copia.getTamanio(), 3u);
    original.vaciar();
    CHECK_EQ(copia.getTamanio(), 3u);              // la copia sobrevive al original
}

TEST(ListaDoble_constructorMovimiento) {
    ListaDoble<int> a = lista({1, 2, 3});
    ListaDoble<int> b(std::move(a));
    CHECK(a.estaVacia());                          // la origen queda vacia
    CHECK(aVector(b) == std::vector<int>({1, 2, 3}));
}

TEST(ListaDoble_operadorAsignacionCopia) {
    ListaDoble<int> a = lista({1, 2, 3});
    ListaDoble<int> b = lista({9});
    b = a;
    CHECK(aVector(b) == std::vector<int>({1, 2, 3}));
    ListaDoble<int>& ref = b;
    b = ref;                                        // autoasignacion segura
    CHECK(aVector(b) == std::vector<int>({1, 2, 3}));
}

TEST(ListaDoble_operadorAsignacionMovimiento) {
    ListaDoble<int> a = lista({1, 2, 3});
    ListaDoble<int> c = lista({7, 8});
    c = std::move(a);
    CHECK(a.estaVacia());
    CHECK(aVector(c) == std::vector<int>({1, 2, 3}));
}

TEST(ListaDoble_getCabeza) {
    ListaDoble<int> l = lista({4, 5, 6});
    CHECK_EQ(l.getCabeza()->getDato(), 4);
    CHECK(l.getCabeza()->getAnterior() == nullptr);
    ListaDoble<int> vacia;
    CHECK(vacia.getCabeza() == nullptr);
}

TEST(ListaDoble_getCola) {
    ListaDoble<int> l = lista({4, 5, 6});
    CHECK_EQ(l.getCola()->getDato(), 6);
    CHECK(l.getCola()->getSiguiente() == nullptr);
    ListaDoble<int> vacia;
    CHECK(vacia.getCola() == nullptr);
}

TEST(ListaDoble_getTamanio) {
    ListaDoble<int> l;
    CHECK_EQ(l.getTamanio(), 0u);
    l.insertar(1); l.insertar(2);
    CHECK_EQ(l.getTamanio(), 2u);
    l.eliminar([](const int& x) { return x == 1; });
    CHECK_EQ(l.getTamanio(), 1u);
}

TEST(ListaDoble_estaVacia) {
    ListaDoble<int> l;
    CHECK(l.estaVacia());
    l.insertar(1);
    CHECK_FALSE(l.estaVacia());
    l.vaciar();
    CHECK(l.estaVacia());
}

TEST(ListaDoble_insertar) {
    ListaDoble<int> l = lista({1, 2, 3});
    CHECK_EQ(l.getTamanio(), 3u);
    CHECK_EQ(l.getCabeza()->getDato(), 1);
    CHECK_EQ(l.getCola()->getDato(), 3);
    CHECK(aVector(l) == std::vector<int>({1, 2, 3}));           // conserva el orden de insercion
    CHECK(aVectorInverso(l) == std::vector<int>({3, 2, 1}));    // los enlaces dobles son consistentes
}

TEST(ListaDoble_buscar) {
    ListaDoble<int> l = lista({5, 8, 12});
    CHECK_EQ(l.buscar([](const int& x) { return x > 6; }), 8);
    CHECK_EQ(l.buscar([](const int& x) { return x > 100; }), 0);   // T() si no existe
}

TEST(ListaDoble_buscarPtr) {
    {   // version no constante: permite modificar el elemento
        ListaDoble<int> l = lista({5, 8, 12});
        int* p = l.buscarPtr([](const int& x) { return x == 8; });
        CHECK(p != nullptr);
        *p = 80;
        CHECK(aVector(l) == std::vector<int>({5, 80, 12}));
        CHECK(l.buscarPtr([](const int& x) { return x == 999; }) == nullptr);
    }
    {   // version constante
        const ListaDoble<int> l = lista({1, 2, 3});
        const int* p = l.buscarPtr([](const int& x) { return x == 2; });
        CHECK(p != nullptr);
        CHECK_EQ(*p, 2);
        CHECK(l.buscarPtr([](const int& x) { return x == 9; }) == nullptr);
    }
}

TEST(ListaDoble_obtenerPtr) {
    ListaDoble<int> l = lista({10, 20, 30});
    CHECK_EQ(*l.obtenerPtr(0), 10);
    CHECK_EQ(*l.obtenerPtr(2), 30);
    CHECK(l.obtenerPtr(3) == nullptr);
    CHECK(l.obtenerPtr(100) == nullptr);
    ListaDoble<int> vacia;
    CHECK(vacia.obtenerPtr(0) == nullptr);
}

TEST(ListaDoble_existe) {
    const ListaDoble<int> l = lista({1, 2, 3});
    CHECK(l.existe([](const int& x) { return x == 3; }));
    CHECK_FALSE(l.existe([](const int& x) { return x == 4; }));
    ListaDoble<int> vacia;
    CHECK_FALSE(vacia.existe([](const int&) { return true; }));
}

TEST(ListaDoble_contar) {
    ListaDoble<int> l = lista({1, 2, 3, 4, 5, 6});
    CHECK_EQ(l.contar([](const int& x) { return x % 2 == 0; }), 3u);
    CHECK_EQ(l.contar([](const int& x) { return x > 10; }), 0u);
}

TEST(ListaDoble_eliminar) {
    {   // primero
        ListaDoble<int> l = lista({1, 2, 3});
        CHECK(l.eliminar([](const int& x) { return x == 1; }));
        CHECK(aVector(l) == std::vector<int>({2, 3}));
        CHECK(l.getCabeza()->getAnterior() == nullptr);
    }
    {   // ultimo
        ListaDoble<int> l = lista({1, 2, 3});
        CHECK(l.eliminar([](const int& x) { return x == 3; }));
        CHECK(aVector(l) == std::vector<int>({1, 2}));
        CHECK(l.getCola()->getSiguiente() == nullptr);
    }
    {   // medio
        ListaDoble<int> l = lista({1, 2, 3});
        CHECK(l.eliminar([](const int& x) { return x == 2; }));
        CHECK(aVector(l) == std::vector<int>({1, 3}));
        CHECK(aVectorInverso(l) == std::vector<int>({3, 1}));
    }
    {   // unico elemento e inexistente
        ListaDoble<int> l = lista({7});
        CHECK_FALSE(l.eliminar([](const int& x) { return x == 99; }));
        CHECK_EQ(l.getTamanio(), 1u);
        CHECK(l.eliminar([](const int& x) { return x == 7; }));
        CHECK(l.estaVacia());
        CHECK(l.getCabeza() == nullptr);
        CHECK(l.getCola() == nullptr);
        l.insertar(1);                      // sigue siendo utilizable
        CHECK_EQ(l.getTamanio(), 1u);
    }
    {   // solo elimina el primero que cumple
        ListaDoble<int> l = lista({4, 4, 4});
        l.eliminar([](const int& x) { return x == 4; });
        CHECK_EQ(l.getTamanio(), 2u);
    }
}

TEST(ListaDoble_recorrer) {
    ListaDoble<int> l = lista({3, 1, 2});
    std::vector<int> visto;
    l.recorrer([&visto](const int& x) { visto.push_back(x); });
    CHECK(visto == std::vector<int>({3, 1, 2}));
    ListaDoble<int> vacia;
    int llamadas = 0;
    vacia.recorrer([&llamadas](const int&) { llamadas++; });
    CHECK_EQ(llamadas, 0);
}

TEST(ListaDoble_ordenar) {
    {   // ascendente y descendente
        ListaDoble<int> l = lista({5, 2, 9, 1, 5, 3});
        l.ordenar([](const int& a, const int& b) { return a < b; });
        CHECK(aVector(l) == std::vector<int>({1, 2, 3, 5, 5, 9}));
        CHECK(aVectorInverso(l) == std::vector<int>({9, 5, 5, 3, 2, 1}));
        l.ordenar([](const int& a, const int& b) { return a > b; });
        CHECK(aVector(l) == std::vector<int>({9, 5, 5, 3, 2, 1}));
    }
    {   // vacia, un elemento y ya ordenada
        ListaDoble<int> vacia;
        vacia.ordenar([](const int& a, const int& b) { return a < b; });
        CHECK(vacia.estaVacia());
        ListaDoble<int> una = lista({1});
        una.ordenar([](const int& a, const int& b) { return a < b; });
        CHECK(aVector(una) == std::vector<int>({1}));
        ListaDoble<int> ordenada = lista({1, 2, 3});
        ordenada.ordenar([](const int& a, const int& b) { return a < b; });
        CHECK(aVector(ordenada) == std::vector<int>({1, 2, 3}));
    }
    {   // estable: los empates conservan el orden original (se ordena por la decena)
        ListaDoble<int> l = lista({21, 12, 22, 11, 23});
        l.ordenar([](const int& a, const int& b) { return a / 10 < b / 10; });
        CHECK(aVector(l) == std::vector<int>({12, 11, 21, 22, 23}));
    }
}

TEST(ListaDoble_vaciar) {
    ListaDoble<int> l = lista({1, 2, 3});
    l.vaciar();
    CHECK(l.estaVacia());
    CHECK(l.getCabeza() == nullptr);
    l.vaciar();                                     // vaciar una lista vacia no falla
    CHECK_EQ(l.getTamanio(), 0u);
}
