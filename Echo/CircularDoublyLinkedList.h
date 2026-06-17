#pragma once
#include "Node.h"
#include <stdexcept>

using uint = unsigned int;

/**
 * @file CircularDoublyLinkedList.h
 * @brief Lista doblemente enlazada circular generica.
 *
 * Variante de la lista doble donde el ultimo nodo apunta al primero y viceversa.
 * Util para representar colas circulares, rotaciones o historiales de reproduccion
 * donde despues del ultimo elemento se vuelve al primero automaticamente.
 */

template<typename T>
class CircularDoublyLinkedList {
private:
    Node<T>* head;   ///< Puntero de entrada. En lista circular no hay extremos absolutos.
    Node<T>* tail;   ///< Puntero al nodo anterior a head. Facilita insercion al "final".
    uint size;       ///< Numero de elementos.

public:
    CircularDoublyLinkedList() : head(nullptr), tail(nullptr), size(0) {}

    ~CircularDoublyLinkedList() { clear(); }

    /**
     * @brief Inserta un elemento en la posicion siguiente a tail (antes de head en la circularidad).
     * @param value Dato de tipo T a almacenar.
     * Mantiene la invariante circular: tail->next == head y head->prev == tail.
     */
    void insertAtTail(T value) {
        Node<T>* newNode = new Node<T>(value);
        if (!head) {
            // Caso lista vacia: el nodo apunta a si mismo en ambas direcciones.
            head = tail = newNode;
            head->next = head;
            head->prev = head;
        }
        else {
            tail->next = newNode;
            newNode->prev = tail;
            newNode->next = head;   // Cierre circular hacia adelante.
            head->prev = newNode;   // Cierre circular hacia atras.
            tail = newNode;
        }
        size++;
    }

    bool removeAt(uint index) {
        if (index >= size || !head) return false;
        Node<T>* curr = head;
        for (uint i = 0; i < index; i++) curr = curr->next;

        if (size == 1) {
            delete curr;
            head = tail = nullptr;
        }
        else {
            curr->prev->next = curr->next;
            curr->next->prev = curr->prev;
            if (curr == head) head = curr->next;
            if (curr == tail) tail = curr->prev;
            delete curr;
        }
        size--;
        return true;
    }

    T* getAt(uint index) const {
        if (index >= size || !head) return nullptr;
        Node<T>* curr = head;
        for (uint i = 0; i < index; i++) curr = curr->next;
        return &(curr->getValue());
    }

    Node<T>* getHead() const { return head; }
    uint getSize() const { return size; }
    bool isEmpty() const { return size == 0; }

    /**
     * @brief Libera todos los nodos respetando la circularidad.
     * Usa un do-while para asegurar que recorre todos los nodos una sola vez.
     */
    void clear() {
        if (!head) return;
        Node<T>* curr = head;
        do {
            Node<T>* next = curr->next;
            delete curr;
            curr = next;
        } while (curr != head);
        head = tail = nullptr;
        size = 0;
    }
};