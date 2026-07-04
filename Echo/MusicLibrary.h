#pragma once
#include "Playlist.h"
#include "SortEngine.h"
#include "CircularDoublyLinkedList.h"
#include "Artist.h"
#include "Album.h"
#include "Stack.h"
#include "QuickSort.h"
#include "HeapSort.h"
#include "HashTable.h"
#include "RedBlackTree.h"
#include "AVLTree.h"
#include "MergeSort.h"
#include "Graph.h"
#include <vector>
#include <cctype>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;
using uint = unsigned int;

class MusicLibrary {
private:
    DoublyLinkedList<Song> allSongs;
    CircularDoublyLinkedList<Song> queue;
    vector<Artist> artists;
    vector<Album> albums;

    HashTable<string, Song*> indexBySource;
    HashTable<string, vector<Song*>> indexByArtist;
    HashTable<string, vector<Song*>> indexByGenre;

    RedBlackTree<string, Song> indexByName;
    AVLTree<float, Song> indexByDuration;
    AVLTree<int, Song> indexByPlayCount;

    Stack<Song> sessionHistory;

    static string normalizeTitleKey(const string& text) {
        string key;
        for (char c : text) {
            unsigned char uc = static_cast<unsigned char>(c);
            if (isalnum(uc)) {
                key.push_back(static_cast<char>(tolower(uc)));
            }
        }
        return key;
    }

    static int songSimilarity(const Song& a, const Song& b) {
        int score = 0;
        if (a.getAuthor() == b.getAuthor()) score += 40;
        if (!a.getGenre().empty() && a.getGenre() == b.getGenre()) score += 30;
        if (fabs(a.getDuration() - b.getDuration()) <= 20.0f) score += 10;
        return score;
    }

    Graph<Song> buildRecommendationGraph(const vector<Song>& songs) const {
        Graph<Song> graph;

        for (const Song& song : songs) {
            graph.addVertex(song);
        }

        for (int i = 0; i < graph.size(); ++i) {
            for (int j = i + 1; j < graph.size(); ++j) {
                int weight = songSimilarity(graph.vertexAt(i), graph.vertexAt(j));
                if (weight > 0) {
                    graph.addEdge(i, j, weight);
                    graph.addEdge(j, i, weight);
                }
            }
        }

        return graph;
    }

    static string toLowerStr(string str) {
        for (char& c : str) {
            c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
        }
        return str;
    }

public:
    bool isKnownGenre(const string& genre) {
        return genre == "Rock"
            || genre == "Pop"
            || genre == "Hip Hop"
            || genre == "Latin"
            || genre == "Electronic"
            || genre == "Ballad";
    }

    static string normalizeGenre(const Song& song) {
        string title = normalizeTitleKey(song.getName());

        if (title == "alright") return "Hip Hop";
        if (title == "am") return "Latin";
        if (title == "animals") return "Electronic";
        if (title == "apocalypse") return "Ballad";
        if (title == "asitwas") return "Pop";
        if (title == "badguy") return "Pop";
        if (title == "bestfriend") return "Pop";
        if (title == "blindinglights") return "Pop";
        if (title == "boulevardofbrokendreams") return "Rock";
        if (title == "californication") return "Rock";
        if (title == "callaita") return "Latin";
        if (title == "cantstopthefeeling") return "Pop";
        if (title == "chamberofreflection") return "Ballad";
        if (title == "closer") return "Electronic";
        if (title == "comeasyouare") return "Rock";
        if (title == "creep") return "Rock";
        if (title == "dakiti") return "Latin";
        if (title == "dna") return "Hip Hop";
        if (title == "doiwannaknow") return "Rock";
        if (title == "dontyouworrychild") return "Electronic";
        if (title == "ellabailasola") return "Latin";
        if (title == "felizcumpleferxxo") return "Latin";
        if (title == "firework") return "Pop";
        if (title == "getlucky") return "Electronic";
        if (title == "glimpseofus") return "Ballad";
        if (title == "godsplan") return "Hip Hop";
        if (title == "goosebumps") return "Hip Hop";
        if (title == "humble") return "Hip Hop";
        if (title == "karmapolice") return "Rock";
        if (title == "lacancion") return "Latin";
        if (title == "lastnite") return "Rock";
        if (title == "levels") return "Electronic";
        if (title == "levitating") return "Pop";
        if (title == "loseyourself") return "Hip Hop";
        if (title == "luciddreams") return "Hip Hop";
        if (title == "mrbrightside") return "Rock";
        if (title == "normal") return "Latin";
        if (title == "norolemodelz") return "Hip Hop";
        if (title == "nothingsgonnahurtyoubaby") return "Ballad";
        if (title == "onemoretime") return "Electronic";
        if (title == "outside") return "Electronic";
        if (title == "prettygirl") return "Pop";
        if (title == "provenza") return "Latin";
        if (title == "scaredtobelonely") return "Electronic";
        if (title == "sevennationarmy") return "Rock";
        if (title == "shapeofyou") return "Pop";
        if (title == "sickomode") return "Hip Hop";
        if (title == "slowdancinginthedark") return "Ballad";
        if (title == "sofia") return "Pop";
        if (title == "spacesong") return "Ballad";
        if (title == "stronger") return "Hip Hop";
        if (title == "style") return "Pop";
        if (title == "summer") return "Electronic";
        if (title == "sunflower") return "Pop";
        if (title == "smellsliketeenspirit") return "Rock";
        if (title == "thankunext") return "Pop";
        if (title == "titimepregunto") return "Latin";
        if (title == "tusa") return "Latin";
        if (title == "uptownfunk") return "Pop";
        if (title == "wakemeup") return "Electronic";

        return "Other";
    }

    vector<Song*> getAllSongPtrs() {
        vector<Song*> ptrs;
        Node<Song>* curr = allSongs.getHead();
        while (curr != nullptr) {
            ptrs.push_back(&(curr->getValue()));
            curr = curr->next;
        }
        return ptrs;
    }

    MusicLibrary() {
    }

    void addSongToLibrary(const Song& song) {
        Song normalizedSong = song;
        normalizedSong.setGenre(normalizeGenre(normalizedSong));

        allSongs.insertAtTail(normalizedSong);
        Node<Song>* tail = allSongs.getTail();
        Song* ptr = &(tail->getValue());

        indexBySource.insert(ptr->getSource(), ptr);

        vector<Song*>* artistVec = indexByArtist.findPtr(ptr->getAuthor());
        if (artistVec) {
            artistVec->push_back(ptr);
        }
        else {
            indexByArtist.insert(ptr->getAuthor(), vector<Song*>{ptr});
        }

        vector<Song*>* genreVec = indexByGenre.findPtr(ptr->getGenre());
        if (genreVec) {
            genreVec->push_back(ptr);
        }
        else {
            indexByGenre.insert(ptr->getGenre(), vector<Song*>{ptr});
        }

        indexByName.insert(ptr->getName(), *ptr);
        indexByDuration.insert(ptr->getDuration(), *ptr);
        indexByPlayCount.insert(ptr->getPlayCount(), *ptr);

        bool foundArtist = false;
        for (auto& artist : artists) {
            if (artist.getName() == ptr->getAuthor()) {
                artist.addSong(*ptr);
                foundArtist = true;
                break;
            }
        }
        if (!foundArtist) {
            Artist newArtist(ptr->getAuthor(), "");
            newArtist.addSong(*ptr);
            artists.push_back(newArtist);
        }

        bool foundAlbum = false;
        for (auto& album : albums) {
            if (album.getTitle() == ptr->getGenre() && album.getArtistName() == ptr->getAuthor()) {
                album.addTrack(*ptr);
                foundAlbum = true;
                break;
            }
        }
        if (!foundAlbum) {
            Album newAlbum(ptr->getGenre(), ptr->getAuthor(), 2024);
            newAlbum.addTrack(*ptr);
            albums.push_back(newAlbum);
        }
    }

    Song* findSongBySource(const string& source) {
        Song** ptr = indexBySource.findPtr(source);
        return ptr ? *ptr : nullptr;
    }

    Song* findSongByName(const string& name) {
        Node<Song>* curr = allSongs.getHead();
        while (curr != nullptr && curr->getValue().getName() != name) {
            curr = curr->next;
        }
        return curr ? &(curr->getValue()) : nullptr;
    }

    void removeSongFromLibrary(const Song& song) {
        allSongs.removeNode(song);

        indexBySource.remove(song.getSource());
        indexByName.remove(song.getName());
        indexByDuration.remove(song.getDuration());
        indexByPlayCount.remove(song.getPlayCount());

        vector<Song*>* artistVec = indexByArtist.findPtr(song.getAuthor());
        if (artistVec) {
            auto it = std::remove_if(artistVec->begin(), artistVec->end(),
                [&](Song* s) { return s->getSource() == song.getSource(); });
            artistVec->erase(it, artistVec->end());
            if (artistVec->empty()) indexByArtist.remove(song.getAuthor());
        }

        vector<Song*>* genreVec = indexByGenre.findPtr(song.getGenre());
        if (genreVec) {
            auto it = std::remove_if(genreVec->begin(), genreVec->end(),
                [&](Song* s) { return s->getSource() == song.getSource(); });
            genreVec->erase(it, genreVec->end());
            if (genreVec->empty()) indexByGenre.remove(song.getGenre());
        }
    }

    DoublyLinkedList<Song>* getAllSongs() {
        return &allSongs;
    }

    vector<Song*> getSongsSortedByName() {
        vector<Song*> all = getAllSongPtrs();
        SortEngine<Song>::quickSort(all, [](Song* a, Song* b) {
            return a->getName() < b->getName();
            });
        return all;
    }

    vector<Song*> getSongsSortedByDurationAscending() {
        vector<Song*> all = getAllSongPtrs();
        MergeSort::sortByDurationAscending(all);
        return all;
    }

    vector<Song*> getSongsSortedByDurationDescending() {
        vector<Song*> all = getAllSongPtrs();
        MergeSort::sortByDurationDescending(all);
        return all;
    }

    vector<Song> getAllSongsVector() {
        vector<Song> songs;
        Node<Song>* curr = allSongs.getHead();
        while (curr != nullptr) {
            songs.push_back(curr->getValue());
            curr = curr->next;
        }
        return songs;
    }

    vector<RecommendationItem> getRecommendedSongs() {
        vector<RecommendationItem> recommendations;
        vector<Song> allSongsVector = getAllSongsVector();

        if (allSongsVector.empty()) return recommendations;

        Graph<Song> graph = buildRecommendationGraph(allSongsVector);
        vector<int> scores(graph.size(), 0);

        auto boostFromSeed = [&](const Song& seed, int baseBoost) {
            int seedIndex = graph.findVertexIndex([&](const Song& candidate) {
                return candidate.getSource() == seed.getSource();
                });

            if (seedIndex < 0) return;

            graph.forEachNeighbor(seedIndex, [&](int neighborIndex, int edgeWeight, const Song&) {
                scores[neighborIndex] += baseBoost + edgeWeight;
                });
            };

        uint historyCount = sessionHistory.size();
        for (uint i = 0; i < historyCount; ++i) {
            boostFromSeed(sessionHistory.getAt(i), 10);
        }

        for (int i = 0; i < graph.size(); ++i) {
            const Song& song = graph.vertexAt(i);

            bool alreadyInHistory = false;
            for (uint j = 0; j < historyCount; ++j) {
                if (sessionHistory.getAt(j).getSource() == song.getSource()) {
                    alreadyInHistory = true;
                    break;
                }
            }

            if (!alreadyInHistory) {
                recommendations.emplace_back(song, scores[i], "");
            }
        }

        return recommendations;
    }

    void clear() {
        allSongs.clear();
        artists.clear();
        albums.clear();

        indexBySource.clear();
        indexByArtist.clear();
        indexByGenre.clear();
        indexByName.clear();
        indexByDuration.clear();
        indexByPlayCount.clear();
    }

    vector<Song> getSongsByArtistFast(const string& artist) {
        vector<Song> result;
        vector<Song*> vec;
        if (indexByArtist.find(artist, vec)) {
            for (Song* p : vec) result.push_back(*p);
        }
        return result;
    }

    vector<Song> getSongsByGenreFast(const string& genre) {
        vector<Song> result;
        vector<Song*> vec;
        if (indexByGenre.find(genre, vec)) {
            for (Song* p : vec) result.push_back(*p);
        }
        return result;
    }

    vector<Song> getSongsSortedByNameTree() {
        vector<Song> result;
        auto items = indexByName.inorder();
        for (auto& pair : items) result.push_back(pair.second);
        return result;
    }

    Song* findSongByNameExact(const string& name) {
        return indexByName.searchPtr(name);
    }

    vector<Song> getSongsByDurationRange(float minSec, float maxSec) {
        vector<Song> result;
        vector<Song> ptrs = indexByDuration.rangeSearch(minSec, maxSec);
        for (Song& s : ptrs) result.push_back(s);
        return result;
    }

    vector<Song> getTopPlayedSongs(int n) {
        vector<Song> result;
        auto items = indexByPlayCount.reverseInorder(n);
        for (auto& pair : items) result.push_back(pair.second);
        return result;
    }

    vector<Song> getSongsSortedByDurationTree() {
        vector<Song> result;
        auto items = indexByDuration.inorder();
        for (auto& pair : items) result.push_back(pair.second);
        return result;
    }

    vector<Song> searchIncremental(const string& query) {
        vector<Song> results;
        if (query.empty()) {
            return getAllSongsVector();
        }

        string lowerQuery = toLowerStr(query);

        Node<Song>* curr = allSongs.getHead();
        while (curr != nullptr) {
            Song& song = curr->getValue();
            string lowerName = toLowerStr(song.getName());
            string lowerArtist = toLowerStr(song.getAuthor());

            if (lowerName.find(lowerQuery) != string::npos ||
                lowerArtist.find(lowerQuery) != string::npos) {
                results.push_back(song);
            }
            curr = curr->next;
        }

        return results;
    }

    vector<Song> searchByNamePrefix(const string& prefix) {
        vector<Song> result;
        auto items = indexByName.prefixSearch(prefix);
        for (auto& pair : items) result.push_back(pair.second);
        return result;
    }

    Playlist generateThirtyMinMix(const string& mixName = "Daily Mix") {
        const float TARGET = 1800.0f;
        const float TOLERANCE = 60.0f;
        const float MIN_SONG = 120.0f;
        const float MAX_SONG = 360.0f;

        Playlist mix(mixName);
        vector<string> usedSources;
        float total = 0.0f;

        srand(static_cast<unsigned>(time(nullptr)));

        for (int attempt = 0; attempt < 50 && total < TARGET - TOLERANCE; ++attempt) {
            float remaining = TARGET - total;
            float searchMin = MIN_SONG;
            float searchMax = remaining + TOLERANCE;
            if (searchMax > MAX_SONG) searchMax = MAX_SONG;

            if (searchMin > searchMax) break;

            vector<Song> candidates = getSongsByDurationRange(searchMin, searchMax);

            vector<Song> available;
            for (Song& s : candidates) {
                bool used = false;
                for (const string& src : usedSources) {
                    if (s.getSource() == src) {
                        used = true;
                        break;
                    }
                }
                if (!used) available.push_back(s);
            }

            if (available.empty()) break;

            for (size_t i = 1; i < available.size(); ++i) {
                Song key = available[i];
                float keyDiff = fabs(key.getDuration() - remaining);
                int j = static_cast<int>(i) - 1;
                while (j >= 0 && fabs(available[j].getDuration() - remaining) > keyDiff) {
                    available[j + 1] = available[j];
                    --j;
                }
                available[j + 1] = key;
            }

            int pickRange = (available.size() < 3) ? static_cast<int>(available.size()) : 3;
            int pick = rand() % pickRange;
            Song chosen = available[pick];

            mix.addSong(chosen);
            usedSources.push_back(chosen.getSource());
            total += chosen.getDuration();
        }

        return mix;
    }

    Song getNextAlphabetical(const string& currentName) {
        vector<Song> sorted = getSongsSortedByNameTree();

        for (size_t i = 0; i < sorted.size(); ++i) {
            if (sorted[i].getName() == currentName) {
                if (i + 1 < sorted.size()) return sorted[i + 1];
                break;
            }
        }
        return Song();
    }

    Song getPreviousAlphabetical(const string& currentName) {
        vector<Song> sorted = getSongsSortedByNameTree();

        for (size_t i = 0; i < sorted.size(); ++i) {
            if (sorted[i].getName() == currentName) {
                if (i > 0) return sorted[i - 1];
                break;
            }
        }
        return Song();
    }

    void addToSessionHistory(const Song& song) {
        if (!sessionHistory.isEmpty()) {
            Song top = sessionHistory.top();
            if (top.getSource() == song.getSource()) return;
        }
        sessionHistory.push(song);
    }

    Stack<Song>* getSessionHistory() {
        return &sessionHistory;
    }
};