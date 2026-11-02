#include "mini_test.h"
#include "ListaDoble.h"
#include "Profesor.h"
#include <string>
#include <vector>

// ---------- Ayudas para las pruebas ----------

// Recorre hacia adelante usando los enlaces "siguiente".
static std::vector<int> haciaAdelante(const ListaDoble<int>& l) {
    std::vector<int> v;
    for (Nodo<int>* n = l.getCabeza(); n; n = n->getSiguiente()) v.push_back(n->getDato());
    return v;
}

// Recorre hacia atras usando los enlaces "anterior", empezando en la cola.
static std::vector<int> haciaAtras(const ListaDoble<int>& l) {
    std::vector<int> v;
    for (Nodo<int>* n = l.getCola(); n; n = n->getAnterior()) v.push_back(n->getDato());
    return v;
}

// Comprueba TODOS los invariantes de una lista doblemente enlazada:
//  - cabeza->anterior == nullptr y cola->siguiente == nullptr
//  - para cada nodo, siguiente->anterior == nodo
//  - la cantidad de nodos coincide con getTamanio()
//  - el ultimo nodo alcanzado hacia adelante es la cola
//  - lista vacia <=> cabeza == cola == nullptr y tamanio 0
static bool invariantesOk(const ListaDoble<int>& l) {
    if (l.getCabeza() == nullptr || l.getCola() == nullptr) {
        return l.getCabeza() == nullptr && l.getCola() == nullptr && l.getTamanio() == 0;
    }
    if (l.getCabeza()->getAnterior() != nullptr) return false;
    if (l.getCola()->getSiguiente() != nullptr) return false;
    size_t contados = 0;
    Nodo<int>* ultimo = nullptr;
    for (Nodo<int>* n = l.getCabeza(); n; n = n->getSiguiente()) {
        ++contados;
        ultimo = n;
        if (n->getSiguiente() && n->getSiguiente()->getAnterior() != n) return false;
        if (contados > 10000) return false;  // proteccion contra ciclos
    }
    return ultimo == l.getCola() && contados == l.getTamanio();
}

static ListaDoble<int> listaCon(std::vector<int> valores) {
    ListaDoble<int> l;
    for (int v : valores) l.insertar(v);
    return l;
}

// Tipo que cuenta cuantas instancias estan vivas, para detectar fugas de memoria.
struct Contador {
    static int vivos;
    int id;
    Contador() : id(0) { ++vivos; }
    explicit Contador(int i) : id(i) { ++vivos; }
    Contador(const Contador& o) : id(o.id) { ++vivos; }
    Contador& operator=(const Contador& o) { id = o.id; return *this; }
    ~Contador() { --vivos; }
};
int Contador::vivos = 0;

// ---------- Estado inicial ----------

TEST_CASE("ListaDoble: una lista nueva esta vacia") {
    ListaDoble<int> l;
    CHECK(l.getCabeza() == nullptr);
    CHECK(l.getCola() == nullptr);
    CHECK_EQ(l.getTamanio(), 0);
    CHECK(invariantesOk(l));
}

TEST_CASE("ListaDoble: recorrer una lista vacia no ejecuta la accion") {
    ListaDoble<int> l;
    int llamadas = 0;
    l.recorrer([&llamadas](int) { ++llamadas; });
    CHECK_EQ(llamadas, 0);
}

TEST_CASE("ListaDoble: buscar en una lista vacia devuelve el valor por defecto sin fallar") {
    ListaDoble<int> l;
    CHECK_EQ(l.buscar([](int) { return true; }), 0);
}

TEST_CASE("ListaDoble: eliminar en una lista vacia no hace nada ni falla") {
    ListaDoble<int> l;
    l.eliminar([](int) { return true; });
    CHECK_EQ(l.getTamanio(), 0);
    CHECK(invariantesOk(l));
}

// ---------- Insercion ----------

TEST_CASE("ListaDoble: insertar el primer elemento deja cabeza == cola") {
    ListaDoble<int> l;
    l.insertar(7);
    REQUIRE(l.getCabeza() != nullptr);
    CHECK(l.getCabeza() == l.getCola());
    CHECK_EQ(l.getCabeza()->getDato(), 7);
    CHECK(l.getCabeza()->getAnterior() == nullptr);
    CHECK(l.getCabeza()->getSiguiente() == nullptr);
    CHECK_EQ(l.getTamanio(), 1);
    CHECK(invariantesOk(l));
}

TEST_CASE("ListaDoble: insertar agrega al final y conserva el orden") {
    ListaDoble<int> l = listaCon({1, 2, 3});
    CHECK_EQ(l.getTamanio(), 3);
    CHECK_EQ(l.getCabeza()->getDato(), 1);
    CHECK_EQ(l.getCola()->getDato(), 3);
    CHECK(haciaAdelante(l) == std::vector<int>({1, 2, 3}));
    CHECK(invariantesOk(l));
}

TEST_CASE("ListaDoble: el recorrido hacia atras (por los enlaces anterior) es el inverso") {
    ListaDoble<int> l = listaCon({1, 2, 3, 4});
    CHECK(haciaAtras(l) == std::vector<int>({4, 3, 2, 1}));
}

TEST_CASE("ListaDoble: insertar admite valores repetidos") {
    ListaDoble<int> l = listaCon({5, 5, 5});
    CHECK_EQ(l.getTamanio(), 3);
    CHECK(haciaAdelante(l) == std::vector<int>({5, 5, 5}));
}

// ---------- Recorrido ----------

TEST_CASE("ListaDoble: recorrer visita todos los elementos de cabeza a cola") {
    ListaDoble<int> l = listaCon({10, 20, 30});
    std::vector<int> visitados;
    l.recorrer([&visitados](int x) { visitados.push_back(x); });
    CHECK(visitados == std::vector<int>({10, 20, 30}));
}

// ---------- Busqueda ----------

TEST_CASE("ListaDoble: buscar devuelve el elemento que cumple el criterio") {
    ListaDoble<int> l = listaCon({1, 2, 3});
    CHECK_EQ(l.buscar([](int x) { return x == 2; }), 2);
}

TEST_CASE("ListaDoble: buscar encuentra el primero y el ultimo elemento") {
    ListaDoble<int> l = listaCon({1, 2, 3});
    CHECK_EQ(l.buscar([](int x) { return x == 1; }), 1);
    CHECK_EQ(l.buscar([](int x) { return x == 3; }), 3);
}

TEST_CASE("ListaDoble: si varios cumplen el criterio, buscar devuelve el primero") {
    ListaDoble<int> l = listaCon({4, 8, 12});
    CHECK_EQ(l.buscar([](int x) { return x % 4 == 0; }), 4);
}

TEST_CASE("ListaDoble: buscar sin coincidencias devuelve el valor por defecto") {
    ListaDoble<int> l = listaCon({1, 2, 3});
    CHECK_EQ(l.buscar([](int x) { return x == 99; }), 0);
}

TEST_CASE("ListaDoble: buscar no modifica la lista") {
    ListaDoble<int> l = listaCon({1, 2, 3});
    l.buscar([](int x) { return x == 2; });
    CHECK_EQ(l.getTamanio(), 3);
    CHECK(invariantesOk(l));
}

// ---------- Eliminacion ----------

TEST_CASE("ListaDoble: eliminar la cabeza actualiza cabeza y deja anterior en nullptr") {
    ListaDoble<int> l = listaCon({1, 2, 3});
    l.eliminar([](int x) { return x == 1; });
    CHECK(haciaAdelante(l) == std::vector<int>({2, 3}));
    CHECK_EQ(l.getCabeza()->getDato(), 2);
    CHECK(l.getCabeza()->getAnterior() == nullptr);
    CHECK_EQ(l.getTamanio(), 2);
    CHECK(invariantesOk(l));
}

TEST_CASE("ListaDoble: eliminar la cola actualiza cola y deja siguiente en nullptr") {
    ListaDoble<int> l = listaCon({1, 2, 3});
    l.eliminar([](int x) { return x == 3; });
    CHECK(haciaAdelante(l) == std::vector<int>({1, 2}));
    CHECK_EQ(l.getCola()->getDato(), 2);
    CHECK(l.getCola()->getSiguiente() == nullptr);
    CHECK_EQ(l.getTamanio(), 2);
    CHECK(invariantesOk(l));
}

TEST_CASE("ListaDoble: eliminar un nodo intermedio reconecta sus vecinos en ambos sentidos") {
    ListaDoble<int> l = listaCon({1, 2, 3});
    l.eliminar([](int x) { return x == 2; });
    CHECK(haciaAdelante(l) == std::vector<int>({1, 3}));
    CHECK(haciaAtras(l) == std::vector<int>({3, 1}));
    CHECK_EQ(l.getTamanio(), 2);
    CHECK(invariantesOk(l));
}

TEST_CASE("ListaDoble: eliminar el unico elemento deja la lista vacia") {
    ListaDoble<int> l = listaCon({9});
    l.eliminar([](int x) { return x == 9; });
    CHECK(l.getCabeza() == nullptr);
    CHECK(l.getCola() == nullptr);
    CHECK_EQ(l.getTamanio(), 0);
    CHECK(invariantesOk(l));
}

TEST_CASE("ListaDoble: eliminar un elemento inexistente no cambia la lista") {
    ListaDoble<int> l = listaCon({1, 2, 3});
    l.eliminar([](int x) { return x == 99; });
    CHECK(haciaAdelante(l) == std::vector<int>({1, 2, 3}));
    CHECK_EQ(l.getTamanio(), 3);
    CHECK(invariantesOk(l));
}

TEST_CASE("ListaDoble: con duplicados, eliminar quita solo la primera coincidencia") {
    ListaDoble<int> l = listaCon({1, 2, 1});
    l.eliminar([](int x) { return x == 1; });
    CHECK(haciaAdelante(l) == std::vector<int>({2, 1}));
    CHECK_EQ(l.getTamanio(), 2);
    CHECK(invariantesOk(l));
}

TEST_CASE("ListaDoble: se puede vaciar elemento por elemento y volver a usar la lista") {
    ListaDoble<int> l = listaCon({1, 2, 3});
    l.eliminar([](int x) { return x == 2; });
    l.eliminar([](int x) { return x == 1; });
    l.eliminar([](int x) { return x == 3; });
    CHECK_EQ(l.getTamanio(), 0);
    CHECK(invariantesOk(l));
    l.insertar(50);
    CHECK(haciaAdelante(l) == std::vector<int>({50}));
    CHECK(l.getCabeza() == l.getCola());
    CHECK(invariantesOk(l));
}

TEST_CASE("ListaDoble: insertar despues de eliminar la cola mantiene los enlaces") {
    ListaDoble<int> l = listaCon({1, 2, 3});
    l.eliminar([](int x) { return x == 3; });
    l.insertar(4);
    CHECK(haciaAdelante(l) == std::vector<int>({1, 2, 4}));
    CHECK(haciaAtras(l) == std::vector<int>({4, 2, 1}));
    CHECK(invariantesOk(l));
}

TEST_CASE("ListaDoble: eliminar libera el nodo (no queda ninguna copia viva del dato)") {
    int base = Contador::vivos;
    ListaDoble<Contador> l;
    l.insertar(Contador(1));
    l.insertar(Contador(2));
    l.insertar(Contador(3));
    CHECK_EQ(Contador::vivos, base + 3);
    l.eliminar([](Contador c) { return c.id == 2; });
    CHECK_EQ(Contador::vivos, base + 2);
    l.eliminar([](Contador c) { return c.id == 1; });
    l.eliminar([](Contador c) { return c.id == 3; });
    CHECK_EQ(Contador::vivos, base);  // los 3 nodos quedaron liberados
}

// ---------- Otros tipos ----------

TEST_CASE("ListaDoble: funciona con std::string") {
    ListaDoble<std::string> l;
    l.insertar("a");
    l.insertar("b");
    CHECK_EQ(l.getTamanio(), 2);
    CHECK_EQ(l.buscar([](std::string s) { return s == "b"; }), std::string("b"));
}

TEST_CASE("ListaDoble: funciona con objetos Profesor y se busca por cedula") {
    ListaDoble<Profesor> l;
    l.insertar(Profesor("0605293885", "Adrian", "Toapanta"));
    l.insertar(Profesor("1710034065", "Ana", "Gomez"));
    Profesor p = l.buscar([](Profesor x) { return x.getCedula() == "1710034065"; });
    CHECK_EQ(p.getNombres(), std::string("Ana"));
    Profesor noExiste = l.buscar([](Profesor x) { return x.getCedula() == "0000000000"; });
    CHECK_EQ(noExiste.getCedula(), std::string(""));  // Profesor() por defecto
}

TEST_CASE("ListaDoble: getters y setters de cabeza, cola y tamanio") {
    ListaDoble<int> l;
    Nodo<int> n(5);
    l.setCabeza(&n);
    l.setCola(&n);
    l.setTamanio(1);
    CHECK(l.getCabeza() == &n);
    CHECK(l.getCola() == &n);
    CHECK_EQ(l.getTamanio(), 1);
    // Se deja la lista en estado vacio: el nodo es local y no debe quedar referenciado.
    l.setCabeza(nullptr);
    l.setCola(nullptr);
    l.setTamanio(0);
}

// ---------- Defectos conocidos (describen el comportamiento CORRECTO) ----------

DEFECT_CASE("[DEFECTO] ListaDoble libera todos sus nodos al destruirse (sin fugas de memoria)") {
    int base = Contador::vivos;
    {
        ListaDoble<Contador> l;
        l.insertar(Contador(1));
        l.insertar(Contador(2));
        l.insertar(Contador(3));
    }  // aqui deberia ejecutarse un destructor que borre los 3 nodos
    CHECK_EQ(Contador::vivos, base);
}

DEFECT_CASE("[DEFECTO] copiar una lista produce una copia independiente (copia profunda)") {
    ListaDoble<int> a = listaCon({1, 2});
    ListaDoble<int> b = a;  // sin constructor de copia, comparte los nodos con 'a'
    b.insertar(3);
    CHECK_EQ(a.getTamanio(), 2);
    CHECK(haciaAdelante(a) == std::vector<int>({1, 2}));  // 'a' no debe ver el 3
}
