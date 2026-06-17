#pragma once
#include "Node.h"
#include <stdexcept>

using uint = unsigned int;

/**
 * @file DoublyLinkedList.h
 * @brief Lista doblemente enlazada generica (template).
 *
 * Estructura de datos lineal donde cada elemento (nodo) contiene referencias
 * al anterior y siguiente. Permite insercion/eliminacion en ambos extremos
 * y recorrido bidireccional en O(1) para extremos y O(n) para posiciones intermedias.
 * Se usa como contenedor principal de canciones y playlists.
 */

template<typename T>
class DoublyLinkedList
{
private:
    Node<T>* head;   ///< Puntero al primer nodo. nullptr si la lista esta vacia.
    Node<T>* tail;   ///< Puntero al ultimo nodo. Permite insercion en cola en O(1).
    uint size;       ///< Numero de elementos. 'uint' es alias de unsigned int.

public:
    /**
     * @brief Constructor. Inicializa lista vacia con punteros nulos y tamaño 0.
     */
    DoublyLinkedList() : head(nullptr), tail(nullptr), size(0) {}

    /**
     * @brief Destructor. Libera la memoria de cada nodo secuencialmente.
     * Recorre desde head hasta nullptr eliminando cada nodo para evitar fugas de memoria.
     */
    ~DoublyLinkedList() {
        Node<T>* current = head;
        while (current != nullptr) {
            Node<T>* next = current->next;  // Guardamos el siguiente antes de borrar.
            delete current;                 // Libera la memoria del nodo actual.
            current = next;                 // Avanzamos al siguiente.
        }
        head = tail = nullptr;
        size = 0;
    }

    /**
     * @brief Inserta un elemento al inicio de la lista.
     * @param value Valor de tipo T a insertar.
     * Complejidad: O(1). Se actualizan 4 punteros como maximo.
     */
    void insertAtHead(T value) {
        Node<T>* newNode = new Node<T>(value);
        if (head == nullptr) {
            head = tail = newNode;          // Lista vacia: head y tail apuntan al nuevo nodo.
        }
        else {
            newNode->next = head;           // El nuevo nodo apunta al antiguo head.
            head->prev = newNode;           // El antiguo head reconoce al nuevo como anterior.
            head = newNode;                 // Head se actualiza al nuevo nodo.
        }
        size++;
    }

    /**
     * @brief Inserta un elemento al final de la lista.
     * @param value Valor de tipo T a insertar.
     * Complejidad: O(1) gracias al puntero tail.
     */
    void insertAtTail(T value) {
        Node<T>* newNode = new Node<T>(value);
        if (tail == nullptr) {
            head = tail = newNode;
        }
        else {
            tail->next = newNode;           // El ultimo nodo apunta al nuevo.
            newNode->prev = tail;           // El nuevo nodo reconoce al antiguo tail.
            tail = newNode;                 // Tail avanza al nuevo nodo.
        }
        size++;
    }

    /**
     * @brief Elimina el primer nodo cuyo valor coincida con el parametro.
     * @param value Valor a buscar y eliminar. Requiere que T tenga operator== sobrecargado.
     * @return true si se encontro y elimino, false en caso contrario.
     * Complejidad: O(n) en el peor caso.
     */
    bool removeNode(T value) {
        Node<T>* current = head;
        while (current != nullptr) {
            if (current->getValue() == value) {
                // Reenlace de punteros para "saltar" el nodo actual.
                if (current == head) {
                    head = head->next;
                    if (head) head->prev = nullptr;
                }
                else if (current == tail) {
                    tail = tail->prev;
                    if (tail) tail->next = nullptr;
                }
                else {
                    current->prev->next = current->next;
                    current->next->prev = current->prev;
                }
                delete current;
                size--;
                if (size == 0) head = tail = nullptr;
                return true;
            }
            current = current->next;
        }
        return false;
    }

    /**
     * @brief Elimina el nodo en la posicion indicada.
     * @param index Indice basado en 0. Debe ser menor que size.
     * @return true si el indice es valido y se elimino.
     */
    bool removeAt(uint index) {
        if (index >= size) return false;
        Node<T>* current = head;
        for (uint i = 0; i < index; i++) current = current->next;

        if (current == head) {
            head = head->next;
            if (head) head->prev = nullptr;
        }
        else if (current == tail) {
            tail = tail->prev;
            if (tail) tail->next = nullptr;
        }
        else {
            current->prev->next = current->next;
            current->next->prev = current->prev;
        }
        delete current;
        size--;
        if (size == 0) head = tail = nullptr;
        return true;
    }

    /**
     * @brief Obtiene un puntero al valor en la posicion indicada.
     * @param index Indice basado en 0.
     * @return Puntero al valor almacenado (T*) o nullptr si el indice es invalido.
     * Retorna puntero en lugar de copia para permitir modificaciones externas y por eficiencia.
     */
    T getAt(uint index) const {
        if (index >= size) return T();
        Node<T>* current = head;
        for (uint i = 0; i < index; i++) current = current->next;
        return current->getValue();
    }

    Node<T>* getHead() const { return head; }
    Node<T>* getTail() const { return tail; }
    uint getSize() const { return size; }
    bool isEmpty() const { return size == 0; }

    /**
     * @brief Vacia la lista liberando toda la memoria.
     */
    void clear() {
        Node<T>* current = head;
        while (current != nullptr) {
            Node<T>* next = current->next;
            delete current;
            current = next;
        }
        head = tail = nullptr;
        size = 0;
    }
};