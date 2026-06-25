#pragma once

template<typename T>
class Node
{
private:
    T value;            // Dato almacenado en el nodo. T puede ser cualquier tipo (primitivo o clase).
public:
    Node<T>* next;      // Puntero al siguiente nodo en la secuencia.
    Node<T>* prev;      // Puntero al nodo anterior. Habilita recorrido bidireccional.

    Node() : value(T()), next(nullptr), prev(nullptr) {}
    Node(T value) : value(value), next(nullptr), prev(nullptr) {}

    T& getValue() { return value; }
    const T& getValue() const { return value; }

    void setValue(T value) { this->value = value; }
};
