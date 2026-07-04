#pragma once
<<<<<<< HEAD
=======
using uint = unsigned int;
>>>>>>> 0c6f778 (AvanceNew)
#include <stdexcept>

template<typename T>
class Stack {
private:

    struct StackNode {
        T data;           // Dato almacenado.
        StackNode* next;  // Puntero al nodo inferior en la pila.
        StackNode(T d) : data(d), next(nullptr) {}
    };
    StackNode* topNode;   // Puntero al tope de la pila. nullptr si esta vacia.
    uint count;           // Numero de elementos.

public:
    Stack() : topNode(nullptr), count(0) {}

    Stack(const Stack& other) : topNode(nullptr), count(0) {
        if (other.isEmpty()) return;
        StackNode* otherCurr = other.topNode;
        StackNode* last = nullptr;
        while (otherCurr) {
            StackNode* newNode = new StackNode(otherCurr->data);
            if (!topNode) {
                topNode = newNode;
            } else {
                last->next = newNode;
            }
            last = newNode;
            otherCurr = otherCurr->next;
        }
        count = other.count;
    }

    Stack& operator=(const Stack& other) {
        if (this != &other) {
            while (!isEmpty()) pop();
            if (other.isEmpty()) return *this;
            StackNode* otherCurr = other.topNode;
            StackNode* last = nullptr;
            while (otherCurr) {
                StackNode* newNode = new StackNode(otherCurr->data);
                if (!topNode) {
                    topNode = newNode;
                } else {
                    last->next = newNode;
                }
                last = newNode;
                otherCurr = otherCurr->next;
            }
            count = other.count;
        }
        return *this;
    }

    ~Stack() {
        while (!isEmpty()) pop();
    }


    void push(T value) {
        StackNode* newNode = new StackNode(value);
        newNode->next = topNode;  // El nuevo nodo descansa sobre el tope actual.
        topNode = newNode;        // El tope sube al nuevo nodo.
        count++;
    }


    void pop() {
        if (isEmpty()) return;
        StackNode* temp = topNode;
        topNode = topNode->next;  // El tope baja al siguiente nodo.
        delete temp;              // Liberamos la memoria del antiguo tope.
        count--;
    }


    T top() const {
        if (isEmpty()) throw std::runtime_error("Stack vacio");
        return topNode->data;
    }

    bool isEmpty() const { return topNode == nullptr; }
    uint size() const { return count; }

    void clear() {
        while (!isEmpty()) pop();
    }

    T getAt(uint index) const {
        if (index >= count) throw std::runtime_error("Index out of bounds");
        StackNode* curr = topNode;
        for (uint i = 0; i < index; i++) {
            curr = curr->next;
        }
        return curr->data;
    }
<<<<<<< HEAD
};
=======
};
>>>>>>> 0c6f778 (AvanceNew)
