#pragma once
#include <vector>
#include <functional>

using namespace std;

using uint = unsigned int;

template<typename T>
class SortEngine {
private:
    static int partition(vector<T*>& arr, int low, int high, function<bool(T*, T*)> comp) {
        T* pivot = arr[high];  // Elegimos el ultimo elemento como pivote.
        int i = low - 1;       // i marca el final de la zona de "menores al pivote".
        for (int j = low; j < high; j++) {
            if (comp(arr[j], pivot)) {
                i++;
                swap(arr[i], arr[j]);  // Intercambio estandar de la STL.
            }
        }
        swap(arr[i + 1], arr[high]);  // Colocamos el pivote en su posicion correcta.
        return i + 1;
    }

    
    static void quickSortRec(vector<T*>& arr, int low, int high, function<bool(T*, T*)> comp) {
        if (low < high) {
            int pi = partition(arr, low, high, comp);
            quickSortRec(arr, low, pi - 1, comp);   // Ordenar subvector izquierdo.
            quickSortRec(arr, pi + 1, high, comp);  // Ordenar subvector derecho.
        }
    }

public:
    
    static void quickSort(vector<T*>& arr, function<bool(T*, T*)> comp) {
        if (arr.size() > 1)
            quickSortRec(arr, 0, arr.size() - 1, comp);
    }
};