#pragma once
#include "DoublyLinkedList.h"
#include "Song.h"
#include <vector>
#include <string>
#include <cctype>

using namespace std;

class SearchSong {
public:
    template<typename Predicate>
    static vector<Song*> search(DoublyLinkedList<Song>& songs, Predicate predicate) {
        vector<Song*> results;
        searchRec(songs.getHead(), predicate, results);
        return results;
    }

    // --- BUSQUEDA POR NOMBRE ---

    static vector<Song*> searchByName(DoublyLinkedList<Song>& songs, const string& query) { 
        return search(songs, [&query](const Song& s) { return s.getName() == query; });
    }

    static vector<Song*> searchByNameContains(DoublyLinkedList<Song>& songs, const string& query) { 
        return search(songs, [&query](const Song& s) {
            return s.getName().find(query) != string::npos;
            });
    }

    static vector<Song*> searchByNameIgnoreCase(DoublyLinkedList<Song>& songs, const string& query) {
        string lowerQuery = toLowerStr(query);
        return search(songs, [&lowerQuery](const Song& s) {
            return toLowerStr(s.getName()).find(lowerQuery) != string::npos;
            });
    }

    // --- BUSQUEDA POR ARTISTA ---
    //lo mismo q las song en cada uno
    static vector<Song*> searchByArtist(DoublyLinkedList<Song>& songs, const string& query) {
        return search(songs, [&query](const Song& s) {
            return s.getAuthor() == query;
            });
    }

    static vector<Song*> searchByArtistContains(DoublyLinkedList<Song>& songs, const string& query) {
        return search(songs, [&query](const Song& s) {
            return s.getAuthor().find(query) != string::npos;
            });
    }

    static vector<Song*> searchByArtistIgnoreCase(DoublyLinkedList<Song>& songs, const string& query) {
        string lowerQuery = toLowerStr(query);
        return search(songs, [&lowerQuery](const Song& s) { return toLowerStr(s.getAuthor()).find(lowerQuery) != string::npos; });
    }

private:
    template<typename Predicate>
    static void searchRec(Node<Song>* node, Predicate predicate, vector<Song*>& results) {
        if (node == nullptr) return;
        if (predicate(node->getValue())) {
            results.push_back(&(node->getValue()));
        }
        searchRec(node->next, predicate, results);
    }

    static string toLowerStr(string str) {
        for (char& c : str) {
            c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
        }
        return str;
    }
};