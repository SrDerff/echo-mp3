#pragma once
#include "Song.h"
#include <vector>
#include <string>

using namespace std;

struct RecommendationItem {
    Song song;
    int score;
    string reason;

    RecommendationItem() : song(), score(0), reason("") {}
    RecommendationItem(const Song& song, int score, const string& reason)
        : song(song), score(score), reason(reason) {
    }
};

class QuickSort {
public:
    static int particion(vector<RecommendationItem>& A, int p, int r, bool descending) {
        int x = A[r].score;
        int i = p - 1;

        for (int j = p; j < r; j++) {
            bool shouldSwap = descending ? (A[j].score >= x) : (A[j].score <= x);
            if (shouldSwap) {
                i++;
                RecommendationItem temp = A[i];
                A[i] = A[j];
                A[j] = temp;
            }
        }

        RecommendationItem temp = A[i + 1];
        A[i + 1] = A[r];
        A[r] = temp;

        return i + 1;
    }

    static void quickSort(vector<RecommendationItem>& A, int p, int r, bool descending = true) {
        int q;
        if (p < r) {
            q = particion(A, p, r, descending);
            quickSort(A, p, q - 1, descending);
            quickSort(A, q + 1, r, descending);
        }
    }
};
