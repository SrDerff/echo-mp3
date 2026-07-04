#pragma once
#include "Playlist.h"
#include <vector>

using namespace std;

class HeapSort {
private:
    template <typename T, typename Compare>
    static void swapItems(T& left, T& right) {
        T temp = left;
        left = right;
        right = temp;
    }

    template <typename T, typename Compare>
    static void heapify(vector<T>& items, int count, int index, Compare higherPriority) {
        int largest = index;
        int leftChild = 2 * index + 1;
        int rightChild = 2 * index + 2;

        if (leftChild < count && higherPriority(items[leftChild], items[largest])) {
            largest = leftChild;
        }

        if (rightChild < count && higherPriority(items[rightChild], items[largest])) {
            largest = rightChild;
        }

        if (largest != index) {
            swapItems<T, Compare>(items[index], items[largest]);
            heapify(items, count, largest, higherPriority);
        }
    }

    template <typename T, typename Compare>
    static void buildHeap(vector<T>& items, Compare higherPriority) {
        for (int index = static_cast<int>(items.size()) / 2 - 1; index >= 0; --index) {
            heapify(items, static_cast<int>(items.size()), index, higherPriority);
        }
    }

    template <typename T, typename Compare>
    static void reverseItems(vector<T>& items) {
        int leftIndex = 0;
        int rightIndex = static_cast<int>(items.size()) - 1;

        while (leftIndex < rightIndex) {
            swapItems<T, Compare>(items[leftIndex], items[rightIndex]);
            ++leftIndex;
            --rightIndex;
        }
    }

public:
    template <typename T, typename Compare>
    static void sort(vector<T>& items, Compare higherPriority, bool ascending) {
        if (items.size() < 2) return;

        buildHeap(items, higherPriority);

        for (int index = static_cast<int>(items.size()) - 1; index > 0; --index) {
            swapItems<T, Compare>(items[0], items[index]);
            heapify(items, index, 0, higherPriority);
        }

        if (!ascending) {
            reverseItems<T, Compare>(items);
        }
    }

    static void sortBySongCount(vector<Playlist>& items, bool ascending) {
        auto higherPriority = [](const Playlist& left, const Playlist& right) {
            if (left.getSize() != right.getSize()) {
                return left.getSize() > right.getSize();
            }
            return left.getName() < right.getName();
            };

        sort(items, higherPriority, ascending);
    }
};