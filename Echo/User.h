#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include "Playlist.h"
#include "HashTable.h"
#include "Stack.h"
#include "Song.h"
#include "MusicLibrary.h"
#include "HeapSort.h"

using namespace std;

class User
{
private:
    string username;
    string hashPassword;
    vector<Playlist> playlists;
    HashTable<string, bool> likedSongs;
    Stack<Song> sessionHistory;
    bool premium;

public:
    User()
        : username(""), hashPassword(""), premium(false) {
        playlists.reserve(100);
    }

    User(const string& username, const string& hashPassword, bool premium = false)
        : username(username), hashPassword(hashPassword), premium(premium) {
        playlists.reserve(100);
    }

    ~User() {
    }

    bool likesSong(const string& songSource) {
        bool val;
        return likedSongs.find(songSource, val);
    }

    bool likeSong(const string& songSource) {
        likedSongs.insert(songSource, true);
        return true;
    }

    bool unlikeSong(const string& songSource) {
        likedSongs.remove(songSource);
        return true;
    }

    bool isSongLiked(const string& songSource) {
        bool val;
        return likedSongs.find(songSource, val);
    }

    void addPlaylist(const string& pl_name) {
        playlists.emplace_back(pl_name);
    }

    void addToPlaylist(const string& pl_name, Song song) {
        for (auto& pl : playlists) {
            if (pl.getName() == pl_name) {
                pl.addSong(song);
                return;
            }
        }
    }

    void removePlaylist(int index) {
        if (index < 0 || index >= static_cast<int>(playlists.size())) {
            return;
        }
        playlists.erase(playlists.begin() + index);
    }

    Playlist* getPlaylist(int index) {
        if (index < 0 || index >= static_cast<int>(playlists.size())) {
            return nullptr;
        }
        return &playlists[index];
    }

    int getPlaylistCount() const {
        return static_cast<int>(playlists.size());
    }

    void sortPlaylistsBySongCount(bool ascending) {
        HeapSort::sortBySongCount(playlists, ascending);
    }

    string getUsername() const { return username; }
    string getHashPassword() const { return hashPassword; }
    bool isPremium() const { return premium; }

    vector<Playlist>* getPlaylists() { return &playlists; }
    HashTable<string, bool>& getLikedSongs() { return likedSongs; }

    vector<Song> getLikedSongsVector(MusicLibrary& lib) {
        vector<Song> likedSongsVector;
        vector<pair<string, bool>> items = likedSongs.getAllItems();

        for (size_t i = 0; i < items.size(); ++i) {
            if (!items[i].second) continue;

            Song* songPtr = lib.findSongBySource(items[i].first);
            if (songPtr != nullptr) {
                likedSongsVector.push_back(*songPtr);
            }
        }

        return likedSongsVector;
    }

    vector<RecommendationItem> getRecommendedSongs(MusicLibrary& lib) {
        vector<RecommendationItem> recommendations;
        vector<Song> allSongsVector = lib.getAllSongsVector();
        vector<Song> likedSongsVector = getLikedSongsVector(lib);

        if (allSongsVector.empty()) {
            return recommendations;
        }

        uint historyCount = sessionHistory.size();

        for (const Song& song : allSongsVector) {
            if (isSongLiked(song.getSource())) continue;

            int score = 0;

            for (uint i = 0; i < historyCount; ++i) {
                Song listenedSong = sessionHistory.getAt(i);
                if (song.getAuthor() == listenedSong.getAuthor()) {
                    score += 10;
                }
                if (lib.isKnownGenre(song.getGenre()) && lib.isKnownGenre(listenedSong.getGenre()) &&
                    song.getGenre() == listenedSong.getGenre()) {
                    score += 15;
                }
            }

            for (const Song& likedSong : likedSongsVector) {
                if (lib.isKnownGenre(song.getGenre()) && lib.isKnownGenre(likedSong.getGenre()) &&
                    song.getGenre() == likedSong.getGenre()) {
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
            if (top.getSource() == song.getSource()) return;
        }
        sessionHistory.push(song);
    }

    Stack<Song>* getSessionHistory() {
        return &sessionHistory;
    }

    void createDailyPlaylist(MusicLibrary& lib) {
        vector<Song> all = lib.getAllSongsVector();
        if (all.empty()) {
            cout << "No hay canciones en la biblioteca.\n";
            return;
        }

        addPlaylist("Daily Mix");
        Playlist* p = getPlaylist(getPlaylistCount() - 1);
        if (!p) return;

        srand(static_cast<unsigned int>(time(nullptr)));

        for (int i = static_cast<int>(all.size()) - 1; i > 0; --i) {
            int j = rand() % (i + 1);
            Song temp = all[i];
            all[i] = all[j];
            all[j] = temp;
        }

        size_t limit = all.size() < static_cast<size_t>(10) ? all.size() : static_cast<size_t>(10);

        for (size_t i = 0; i < limit; ++i) {
            p->addSong(all[i]);
        }
    }
};