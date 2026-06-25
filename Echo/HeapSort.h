#pragma once
#include "Playlist.h"
#include <vector>

using namespace std;

class HeapSort {
private:
    static void swapItems(Playlist& left, Playlist& right) {
        Playlist temp = left;
        left = right;
        right = temp;
    }

    static bool hasHigherPriority(const Playlist& left, const Playlist& right) {
        if (left.getSize() != right.getSize()) {
            return left.getSize() > right.getSize();
        }

        return left.getName() < right.getName();
    }

    static int left(int index) {
        return 2 * index + 1;
    }

    static int right(int index) {
        return 2 * index + 2;
    }

    static void maxHeapify(vector<Playlist>& items, int count, int index) {
        int largest = index;
        int leftChild = left(index);
        int rightChild = right(index);

        if (leftChild < count && hasHigherPriority(items[leftChild], items[largest])) {
            largest = leftChild;
        }

        if (rightChild < count && hasHigherPriority(items[rightChild], items[largest])) {
            largest = rightChild;
        }

        if (largest != index) {
            swapItems(items[index], items[largest]);
            maxHeapify(items, count, largest);
        }
    }

    static void buildMaxHeap(vector<Playlist>& items) {
        for (int index = static_cast<int>(items.size()) / 2 - 1; index >= 0; --index) {
            maxHeapify(items, static_cast<int>(items.size()), index);
        }
    }

    static void reverseItems(vector<Playlist>& items) {
        int leftIndex = 0;
        int rightIndex = static_cast<int>(items.size()) - 1;

        while (leftIndex < rightIndex) {
            swapItems(items[leftIndex], items[rightIndex]);
            ++leftIndex;
            --rightIndex;
        }
    }

public:
    static void sortBySongCount(vector<Playlist>& items, bool ascending) {
        if (items.size() < 2) return;

        buildMaxHeap(items);

        for (int index = static_cast<int>(items.size()) - 1; index > 0; --index) {
            swapItems(items[0], items[index]);
            maxHeapify(items, index, 0);
        }

        if (!ascending) {
            reverseItems(items);
        }
    }
};
