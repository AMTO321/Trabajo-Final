#include "mini_test.h"
#include "../ListaDoble.h"
#include <vector>

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

TEST(Lista_nuevaEstaVacia) {
    ListaDoble<int> l;
    CHECK(l.estaVacia());
    CHECK_EQ(l.getTamanio(), 0u);
    CHECK(l.getCabeza() == nullptr);
    CHECK(l.getCola() == nullptr);
}

TEST(Lista_insertar_mantieneOrdenYEnlaces) {
    ListaDoble<int> l = lista({1, 2, 3});
    CHECK_EQ(l.getTamanio(), 3u);
    CHECK_EQ(l.getCabeza()->getDato(), 1);
    CHECK_EQ(l.getCola()->getDato(), 3);
    CHECK(aVector(l) == std::vector<int>({1, 2, 3}));
    CHECK(aVectorInverso(l) == std::vector<int>({3, 2, 1}));
}

TEST(Lista_buscar_devuelveCopiaODefecto) {
    ListaDoble<int> l = lista({5, 8, 12});
    CHECK_EQ(l.buscar([](const int& x) { return x > 6; }), 8);
    CHECK_EQ(l.buscar([](const int& x) { return x > 100; }), 0);   // T() si no existe
}

TEST(Lista_buscarPtr_permiteModificarElElemento) {
    ListaDoble<int> l = lista({5, 8, 12});
    int* p = l.buscarPtr([](const int& x) { return x == 8; });
    CHECK(p != nullptr);
    *p = 80;
    CHECK(aVector(l) == std::vector<int>({5, 80, 12}));
    CHECK(l.buscarPtr([](const int& x) { return x == 999; }) == nullptr);
}

TEST(Lista_buscarPtr_constYExiste) {
    const ListaDoble<int> l = lista({1, 2, 3});
    const int* p = l.buscarPtr([](const int& x) { return x == 2; });
    CHECK(p != nullptr);
    CHECK_EQ(*p, 2);
    CHECK(l.existe([](const int& x) { return x == 3; }));
    CHECK_FALSE(l.existe([](const int& x) { return x == 4; }));
}

TEST(Lista_obtenerPtr_porIndice) {
    ListaDoble<int> l = lista({10, 20, 30});
    CHECK_EQ(*l.obtenerPtr(0), 10);
    CHECK_EQ(*l.obtenerPtr(2), 30);
    CHECK(l.obtenerPtr(3) == nullptr);
    CHECK(l.obtenerPtr(100) == nullptr);
    ListaDoble<int> vacia;
    CHECK(vacia.obtenerPtr(0) == nullptr);
}

TEST(Lista_contar) {
    ListaDoble<int> l = lista({1, 2, 3, 4, 5, 6});
    CHECK_EQ(l.contar([](const int& x) { return x % 2 == 0; }), 3u);
    CHECK_EQ(l.contar([](const int& x) { return x > 10; }), 0u);
}

TEST(Lista_eliminar_primero) {
    ListaDoble<int> l = lista({1, 2, 3});
    CHECK(l.eliminar([](const int& x) { return x == 1; }));
    CHECK(aVector(l) == std::vector<int>({2, 3}));
    CHECK(l.getCabeza()->getAnterior() == nullptr);
    CHECK_EQ(l.getTamanio(), 2u);
}

TEST(Lista_eliminar_ultimo) {
    ListaDoble<int> l = lista({1, 2, 3});
    CHECK(l.eliminar([](const int& x) { return x == 3; }));
    CHECK(aVector(l) == std::vector<int>({1, 2}));
    CHECK(l.getCola()->getSiguiente() == nullptr);
    CHECK_EQ(l.getCola()->getDato(), 2);
}

TEST(Lista_eliminar_medio) {
    ListaDoble<int> l = lista({1, 2, 3});
    CHECK(l.eliminar([](const int& x) { return x == 2; }));
    CHECK(aVector(l) == std::vector<int>({1, 3}));
    CHECK(aVectorInverso(l) == std::vector<int>({3, 1}));
}

TEST(Lista_eliminar_unicoElementoYInexistente) {
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

TEST(Lista_eliminar_soloElPrimeroQueCumple) {
    ListaDoble<int> l = lista({4, 4, 4});
    l.eliminar([](const int& x) { return x == 4; });
    CHECK_EQ(l.getTamanio(), 2u);
}

TEST(Lista_ordenar_ascendenteYDescendente) {
    ListaDoble<int> l = lista({5, 2, 9, 1, 5, 3});
    l.ordenar([](const int& a, const int& b) { return a < b; });
    CHECK(aVector(l) == std::vector<int>({1, 2, 3, 5, 5, 9}));
    CHECK(aVectorInverso(l) == std::vector<int>({9, 5, 5, 3, 2, 1}));
    l.ordenar([](const int& a, const int& b) { return a > b; });
    CHECK(aVector(l) == std::vector<int>({9, 5, 5, 3, 2, 1}));
}

TEST(Lista_ordenar_vaciaUnElementoYYaOrdenada) {
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

TEST(Lista_ordenar_esEstable) {
    // se ordena por la decena; los empates conservan el orden original
    ListaDoble<int> l = lista({21, 12, 22, 11, 23});
    l.ordenar([](const int& a, const int& b) { return a / 10 < b / 10; });
    CHECK(aVector(l) == std::vector<int>({12, 11, 21, 22, 23}));
}

TEST(Lista_copia_esProfunda) {
    ListaDoble<int> original = lista({1, 2, 3});
    ListaDoble<int> copia(original);
    *copia.obtenerPtr(0) = 99;
    CHECK_EQ(*original.obtenerPtr(0), 1);          // el original no cambia
    CHECK_EQ(copia.getTamanio(), 3u);
    original.vaciar();
    CHECK_EQ(copia.getTamanio(), 3u);              // la copia sobrevive
}

TEST(Lista_asignacionCopia_reemplazaContenidoYAutoasignacion) {
    ListaDoble<int> a = lista({1, 2, 3});
    ListaDoble<int> b = lista({9});
    b = a;
    CHECK(aVector(b) == std::vector<int>({1, 2, 3}));
    ListaDoble<int>& ref = b;
    b = ref;                                        // autoasignacion segura
    CHECK(aVector(b) == std::vector<int>({1, 2, 3}));
}

TEST(Lista_move_transfiereYDejaVaciaLaOrigen) {
    ListaDoble<int> a = lista({1, 2, 3});
    ListaDoble<int> b(std::move(a));
    CHECK(a.estaVacia());
    CHECK(aVector(b) == std::vector<int>({1, 2, 3}));
    ListaDoble<int> c;
    c = std::move(b);
    CHECK(b.estaVacia());
    CHECK(aVector(c) == std::vector<int>({1, 2, 3}));
}

TEST(Lista_vaciar) {
    ListaDoble<int> l = lista({1, 2, 3});
    l.vaciar();
    CHECK(l.estaVacia());
    CHECK(l.getCabeza() == nullptr);
    l.vaciar();                                     // vaciar una lista vacia no falla
    CHECK_EQ(l.getTamanio(), 0u);
}
