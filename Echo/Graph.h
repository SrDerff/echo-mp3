#pragma once
#include <vector>

using namespace std;

template<typename T, typename Peso = int>
class CGrafo {
private:
    class CArco {
    public:
        int destino;
        Peso peso;

        CArco(int destino, Peso peso)
            : destino(destino), peso(peso) {
        }
    };

    class CVertice {
    public:
        T info;
        vector<CArco> ady;

        CVertice() : info(), ady() {}
        CVertice(const T& info) : info(info), ady() {}
    };

    vector<CVertice> vertices;

public:
    CGrafo() : vertices() {}

    void adicionarVertice(const T& info) {
        vertices.emplace_back(info);
    }

    int cantidadVertices() const {
        return static_cast<int>(vertices.size());
    }

    const T& obtenerVertice(int v) const {
        return vertices.at(v).info;
    }

    T& obtenerVertice(int v) {
        return vertices.at(v).info;
    }

    void modificarVertice(int v, const T& info) {
        vertices.at(v).info = info;
    }

    void adicionarArco(int v, int vLlegada, Peso peso = Peso()) {
        vertices.at(v).ady.emplace_back(vLlegada, peso);
    }

    int cantidadArcos(int v) const {
        return static_cast<int>(vertices.at(v).ady.size());
    }

    int obtenerDestinoArco(int v, int apos) const {
        return vertices.at(v).ady.at(apos).destino;
    }

    Peso obtenerPesoArco(int v, int apos) const {
        return vertices.at(v).ady.at(apos).peso;
    }

    template<typename Predicate>
    int buscarVerticeSi(Predicate predicate) const {
        for (int index = 0; index < static_cast<int>(vertices.size()); ++index) {
            if (predicate(vertices[index].info)) {
                return index;
            }
        }
        return -1;
    }

    template<typename Func>
    void recorrerVertice(int v, Func funcion) const {
        const CVertice& vertice = vertices.at(v);
        for (const CArco& arco : vertice.ady) {
            funcion(arco.destino, arco.peso, vertices.at(arco.destino).info);
        }
    }

    template<typename Func>
    void recorrerVertices(Func funcion) const {
        for (int index = 0; index < static_cast<int>(vertices.size()); ++index) {
            funcion(index, vertices[index].info);
        }
    }

    void limpiar() {
        vertices.clear();
    }
};