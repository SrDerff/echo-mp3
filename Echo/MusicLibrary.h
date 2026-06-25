#pragma once
#include "Playlist.h"
#include "SortEngine.h"
#include "CircularDoublyLinkedList.h"
#include "Artist.h"
#include "Album.h"
#include "Stack.h"
#include "QuickSort.h"
#include "HeapSort.h"
#include <vector>
#include "MergeSort.h"
#include <cctype>

using namespace std;
using uint = unsigned int;

class MusicLibrary {
private:
    int currentPlaylistIndex;
public: vector<Playlist> playlists;
private:
    DoublyLinkedList<Song> allSongs;
	CircularDoublyLinkedList<Song> queue;
    vector<Artist> artists;
    vector<Album> albums;
    Stack<Song> sessionHistory;

    static bool containsKeyword(const string& text, const vector<string>& keywords) {
        for (const string& keyword : keywords) {
            if (text.find(keyword) != string::npos) {
                return true;
            }
        }
        return false;
    }

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

    static bool isKnownGenre(const string& genre) {
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

    static bool containsString(const vector<string>& values, const string& value) {
        for (const string& item : values) {
            if (item == value) return true;
        }
        return false;
    }

public:
    MusicLibrary() : currentPlaylistIndex(-1) {
        playlists.reserve(100);
    }


    void addPlaylist(const string& name) {
        string newName = name;
        playlists.emplace_back(newName);
        if (currentPlaylistIndex == -1) currentPlaylistIndex = 0;
    }

    void removePlaylist(int index) {
        if (index >= 0 && index < (int)playlists.size()) {
            if (currentPlaylistIndex == index) currentPlaylistIndex = -1;
            playlists.erase(playlists.begin() + index);
            if (currentPlaylistIndex >= (int)playlists.size())
                currentPlaylistIndex = playlists.empty() ? -1 : 0;
        }
    }

    Playlist* getPlaylist(int index) {
        if (index >= 0 && index < (int)playlists.size()) return &playlists[index];
        return nullptr;
    }

    Playlist* getCurrentPlaylist() {
        return getPlaylist(currentPlaylistIndex);
    }

    int getPlaylistCount() const { return playlists.size(); }
    int getCurrentIndex() const { return currentPlaylistIndex; }

    void sortPlaylistsBySongCount(bool ascending) {
        HeapSort::sortBySongCount(playlists, ascending);
    }

    void addSongToLibrary(const Song& song) {
        Song normalizedSong = song;
        normalizedSong.setGenre(normalizeGenre(normalizedSong));

        allSongs.insertAtTail(normalizedSong);
        Node<Song>* tail = allSongs.getTail();
        Song* ptr = &(tail->getValue());

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
        Node<Song>* curr = allSongs.getHead();
        while (curr != nullptr) {
            if (curr->getValue().getSource() == source) {
                return &(curr->getValue());
            }
            curr = curr->next;
        }
        return nullptr;
    }

    bool toggleLikedBySource(const string& source) {
        Song* song = findSongBySource(source);
        if (song == nullptr) return false;
        song->setLiked(!song->isLiked());
        return true;
    }


    void removeSongFromLibrary(const Song& song) {
        allSongs.removeNode(song);
    }

    DoublyLinkedList<Song>* getAllSongs() { return &allSongs; }

    void addSongToPlaylist(int playlistIndex, const Song& song) {
        Playlist* p = getPlaylist(playlistIndex);
        if (p) p->addSong(song);
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

    vector<Song> getLikedSongsVector() {
        vector<Song> likedSongs;
        Node<Song>* curr = allSongs.getHead();

        while (curr != nullptr) {
            if (curr->getValue().isLiked()) {
                likedSongs.push_back(curr->getValue());
            }
            curr = curr->next;
        }

        return likedSongs;
    }

    vector<RecommendationItem> getRecommendedSongs() {
        vector<RecommendationItem> recommendations;
        vector<Song> allSongsVector = getAllSongsVector();
        vector<Song> likedSongs = getLikedSongsVector();

        if (allSongsVector.empty()) {
            return recommendations;
        }

        uint historyCount = sessionHistory.size();

        for (const Song& song : allSongsVector) {
            if (song.isLiked()) continue;

            int score = 0;

            for (uint i = 0; i < historyCount; ++i) {
                Song listenedSong = sessionHistory.getAt(i);
                if (song.getAuthor() == listenedSong.getAuthor()) {
                    score += 10;
                }
                if (isKnownGenre(song.getGenre()) && isKnownGenre(listenedSong.getGenre()) && song.getGenre() == listenedSong.getGenre()) {
                    score += 15;
                }
            }

            for (const Song& likedSong : likedSongs) {
                if (isKnownGenre(song.getGenre()) && isKnownGenre(likedSong.getGenre()) && song.getGenre() == likedSong.getGenre()) {
                    score += 20;
                }
            }

            recommendations.emplace_back(song, score, "");
        }

        return recommendations;
    }


    void addToSessionHistory(const Song& song) {
        if (!sessionHistory.isEmpty()) {
            Song top = sessionHistory.top();
            if (top.getSource() == song.getSource()) return; // evita duplicado consecutivo
        }
        sessionHistory.push(song);
    }
    Stack<Song>* getSessionHistory() { return &sessionHistory; }

    void createDailyPlaylist() {

        vector<Song> all = getAllSongsVector();
        if (all.empty()) {
            cout << "No hay canciones en la biblioteca.\n";
            return;
        }

        addPlaylist("Daily Mix");
        Playlist* p = getPlaylist(getPlaylistCount() - 1);

        if (!p) return;

        srand(time(nullptr));

        for (int i = static_cast<int>(all.size()) - 1; i > 0; --i) {
            int j = rand() % (i + 1);
            Song temp = all[i];
            all[i] = all[j];
            all[j] = temp;
        }

        size_t limit = all.size() < size_t(10) ? all.size() : size_t(10);

        for (size_t i = 0; i < limit; ++i) {
            p->addSong(all[i]);
        }

    }

    /**
     * @brief Limpia TODA la biblioteca sin destruir el objeto.
     * Usa esto en vez de 'lib = MusicLibrary()' para evitar el crash.
     */
    void clear() {
        playlists.clear();
        currentPlaylistIndex = -1;
        allSongs.clear();
        artists.clear();
        albums.clear();
        sessionHistory.clear();
    }

};
