#pragma once
#include "Song.h"
#include <vector>

using namespace std;

struct RecommendationItem {
    Song song;
    int score;
    string reason;

    RecommendationItem() : song(), score(0), reason("") {}
    RecommendationItem(const Song& song, int score, const string& reason)
        : song(song), score(score), reason(reason) {}
};

class QuickSort {
private:
    template <typename T, typename Compare>
    static int partition(vector<T>& items, int left, int right, Compare comp) {
        T pivot = items[right];
        int i = left - 1;

        for (int j = left; j < right; ++j) {
            if (comp(items[j], pivot)) {
                ++i;
                T temp = items[i];
                items[i] = items[j];
                items[j] = temp;
            }
        }

        T temp = items[i + 1];
        items[i + 1] = items[right];
        items[right] = temp;

        return i + 1;
    }

    template <typename T, typename Compare>
    static void quickSortRec(vector<T>& items, int left, int right, Compare comp) {
        if (left < right) {
            int pivotIndex = partition(items, left, right, comp);
            quickSortRec(items, left, pivotIndex - 1, comp);
            quickSortRec(items, pivotIndex + 1, right, comp);
        }
    }

public:
    template <typename T, typename Compare>
    static void sort(vector<T>& items, Compare comp) {
        if (items.size() > 1) {
            quickSortRec(items, 0, static_cast<int>(items.size()) - 1, comp);
        }
    }

    static void quickSort(vector<RecommendationItem>& items, int left, int right, bool descending = true) {
        auto comp = [descending](const RecommendationItem& a, const RecommendationItem& b) {
            if (a.score == b.score) {
                return a.song.getName() < b.song.getName();
            }
            return descending ? (a.score > b.score) : (a.score < b.score);
        };

        quickSortRec(items, left, right, comp);
    }
};