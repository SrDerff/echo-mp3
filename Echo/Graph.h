#pragma once
#include <vector>

using namespace std;

template <typename T>
class Graph {
private:
    struct Edge {
        int to;
        int weight;
    };

    vector<T> vertices;
    vector<vector<Edge>> adjacency;

public:
    int addVertex(const T& value) {
        vertices.push_back(value);
        adjacency.emplace_back();
        return static_cast<int>(vertices.size()) - 1;
    }

    int size() const {
        return static_cast<int>(vertices.size());
    }

    const T& vertexAt(int index) const {
        return vertices.at(index);
    }

    T& vertexAt(int index) {
        return vertices.at(index);
    }

    template <typename Predicate>
    int findVertexIndex(Predicate predicate) const {
        for (int index = 0; index < size(); ++index) {
            if (predicate(vertices[index])) {
                return index;
            }
        }
        return -1;
    }

    void addEdge(int from, int to, int weight = 1) {
        if (from < 0 || to < 0 || from >= size() || to >= size()) return;
        adjacency[from].push_back({ to, weight });
    }

    template <typename Visitor>
    void forEachNeighbor(int from, Visitor visitor) const {
        if (from < 0 || from >= size()) return;

        for (const Edge& edge : adjacency[from]) {
            visitor(edge.to, edge.weight, vertices[edge.to]);
        }
    }
};