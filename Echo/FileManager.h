#pragma once
#include "MusicLibrary.h"
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>

using namespace std;

class FileManager {
private:
    static string trim(const string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, last - first + 1);
    }

    static Song findSongBySource(MusicLibrary& lib, const string& source) {
        Node<Song>* curr = lib.getAllSongs()->getHead();
        while (curr != nullptr) {
            if (curr->getValue().getSource() == source) {
                return curr->getValue();
            }
            curr = curr->next;
        }
        return Song();
    }

public:
    static bool loadSongs(MusicLibrary& lib, const string& filename = "..\\Data\\songsList.txt") {
        ifstream file(filename);
        if (!file.is_open()) return false;

        lib.clear();

        string line;
        while (getline(file, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            line = trim(line);
            if (line.empty()) continue;

            stringstream ss(line);
            string name, author, source, durationStr, genre, playCountStr, likedStr;

            getline(ss, name, '|');
            getline(ss, author, '|');
            getline(ss, source, '|');
            getline(ss, durationStr, '|');
            getline(ss, genre, '|');
            getline(ss, playCountStr, '|');
            getline(ss, likedStr, '|');

            char dummyImg[25][25];
            memset(dummyImg, ' ', sizeof(dummyImg));

            try {
                float duration = durationStr.empty() ? 0.0f : stof(durationStr);
                int playCount = playCountStr.empty() ? 0 : stoi(playCountStr);
                bool liked = (likedStr == "1" || likedStr == "true" || likedStr == "True");

                Song song(name, author, source, duration, genre, dummyImg, playCount, liked);

                lib.addSongToLibrary(song);

            }
            catch (...) {
                // Ignorar lineas mal formadas
            }
        }

        file.close();
        return true;
    }

    static bool saveSongs(const MusicLibrary& lib, const string& filename = "..\\Data\\songsList.txt") {
        ofstream file(filename);
        if (!file.is_open()) return false;

        DoublyLinkedList<Song>* songs = const_cast<MusicLibrary&>(lib).getAllSongs();
        Node<Song>* curr = songs->getHead();

        while (curr != nullptr) {
            const Song& s = curr->getValue();
            file << s.getName() << "|"
                << s.getAuthor() << "|"
                << s.getSource() << "|"
                << s.getDuration() << "|"
                << s.getGenre() << "|"
                << s.getPlayCount() << "|"
                << (s.isLiked() ? 1 : 0) << "\n";
            curr = curr->next;
        }


        file.close();
        return true;
    }

    static bool loadPlaylists(MusicLibrary& lib, const string& filename = "..\\Data\\playlists.txt") {
        ifstream file(filename);
        if (!file.is_open()) return false;

        string line;
        int currentPlaylistIndex = -1;
        vector<string>names;
        while (getline(file, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            line = trim(line);
            if (line.empty()) continue;

            if (line == "[ENDPLAYLIST]") {
                currentPlaylistIndex = -1;
                continue;
            }

            if (currentPlaylistIndex == -1) {
                if (names.size() > 0) {
                    for (auto x : names) {
                        if (x == line || line == "Daily Mix") {
                            file.close();
                            return false;
                        }
                    }
                }
                lib.addPlaylist(line);
                names.push_back(line);
                currentPlaylistIndex = lib.getPlaylistCount() - 1;
                continue;
            }

            Song song = findSongBySource(lib, line);

            if (song != Song()) {
                lib.addSongToPlaylist(currentPlaylistIndex, song);
            }
        }

        file.close();
        return true;
    }

    static bool savePlaylists(const MusicLibrary& lib, const string& filename = "..\\Data\\playlists.txt") {
        ofstream file(filename);
        if (!file.is_open()) return false;

        for (int i = 0; i < lib.getPlaylistCount(); i++) {
            Playlist* p = const_cast<MusicLibrary&>(lib).getPlaylist(i);
            if (p == nullptr) continue;
            if (p->getName() == "Daily Mix") continue; // No guardamos la Daily Mix
            file << p->getName() << "\n";
            for (uint j = 0; j < p->getSize(); j++) {
                Song s = p->getSongAt(j);
                file << s.getSource() << "\n";
            }
            file << "[ENDPLAYLIST]\n";
        }

        file.close();
        return true;
    }

    static bool loadLibrary(MusicLibrary& lib) {
        bool songsOk = loadSongs(lib);
        bool playlistsOk = loadPlaylists(lib);
        return songsOk && playlistsOk;
    }

    static bool saveLibrary(const MusicLibrary& lib) {
        bool songsOk = saveSongs(lib);
        bool playlistsOk = savePlaylists(lib);
        return songsOk && playlistsOk;
    }
};