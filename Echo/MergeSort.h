#pragma once
#include "Song.h"
#include <vector>

using namespace std;

class MergeSort {
private:
    template <typename T, typename Compare>
    static void merge(vector<T>& arr, int left, int mid, int right, Compare comp) {
        int leftSize = mid - left + 1;
        int rightSize = right - mid;

        vector<T> leftPart(leftSize);
        vector<T> rightPart(rightSize);

        for (int i = 0; i < leftSize; ++i) {
            leftPart[i] = arr[left + i];
        }

        for (int j = 0; j < rightSize; ++j) {
            rightPart[j] = arr[mid + 1 + j];
        }

        int i = 0;
        int j = 0;
        int k = left;

        while (i < leftSize && j < rightSize) {
            if (comp(leftPart[i], rightPart[j])) {
                arr[k++] = leftPart[i++];
            }
            else {
                arr[k++] = rightPart[j++];
            }
        }

        while (i < leftSize) {
            arr[k++] = leftPart[i++];
        }

        while (j < rightSize) {
            arr[k++] = rightPart[j++];
        }
    }

    template <typename T, typename Compare>
    static void mergeSortRec(vector<T>& arr, int left, int right, Compare comp) {
        if (left >= right) return;

        int mid = left + (right - left) / 2;
        mergeSortRec(arr, left, mid, comp);
        mergeSortRec(arr, mid + 1, right, comp);
        merge(arr, left, mid, right, comp);
    }

public:
    template <typename T, typename Compare>
    static void sort(vector<T>& arr, Compare comp) {
        if (arr.size() > 1) {
            mergeSortRec(arr, 0, static_cast<int>(arr.size()) - 1, comp);
        }
    }

    static void sortByDurationAscending(vector<Song*>& songs) {
        sort(songs, [](Song* a, Song* b) {
            return a->getDuration() <= b->getDuration();
            });
    }

    static void sortByDurationDescending(vector<Song*>& songs) {
        sort(songs, [](Song* a, Song* b) {
            return a->getDuration() >= b->getDuration();
            });
    }

    static void sortByDurationAscending(vector<Song>& songs) {
        sort(songs, [](const Song& a, const Song& b) {
            return a.getDuration() <= b.getDuration();
            });
    }

    static void sortByDurationDescending(vector<Song>& songs) {
        sort(songs, [](const Song& a, const Song& b) {
            return a.getDuration() >= b.getDuration();
            });
    }
};