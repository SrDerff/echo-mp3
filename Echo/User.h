#pragma once
#include <string>
#include "Playlist.h"
#include "HashTable.h"
#include "Stack.h"
#include "Song.h"
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
	///TODO
	User(const string& username, const string& hashPassword, bool premium = false) {
		this->username = username;
		this->hashPassword = hashPassword;
		this->premium = premium;
		playlists.reserve(100);
	}
	///TODO
	~User() {
	}
	bool likesSong(const string& song_name) {
		bool val;
		return likedSongs.find(song_name, val);
	}
	bool likeSong(const string& song_name) {
		// Implementation for liking a song
		likedSongs.insert(song_name, true);
		return true;
	}
	bool unlikeSong(const string& song_name) {
		// Implementation for unliking a song
		likedSongs.remove(song_name);
		return true;
	}
	bool isSongLiked(const string& songSource) {
		bool val;
		if (likedSongs.find(songSource, val)) {
			return true;
		}
		return false;
	}
	
	void addPlaylist(const string& pl_name) {
		playlists.emplace_back(pl_name);
	}

	void addToPlaylist(const string& pl_name, Song song) {
		for (auto &pl : playlists) {
			if(pl.getName() == pl_name) {
				pl.addSong(song);
				return;
			}
		}
	}

	void removePlaylist(int index) {
		for(int i = 0; i < static_cast<int>(playlists.size()); i++) {
			if(i == index) {
				playlists.erase(playlists.begin() + i);
				return;
			}
		}
	}

	Playlist* getPlaylist(int index) {
		if (index < 0 || index >= static_cast<int>(playlists.size())) {
			return nullptr;
		}
		return &playlists[index];
	}

	int getPlaylistCount() const { return playlists.size(); }

	void sortPlaylistsBySongCount(bool ascending) {
		HeapSort::sortBySongCount(playlists, ascending);
	}
	
	string getUsername() const { return username; }
	string getHashPassword() const { return hashPassword; }
	bool isPremium() const { return premium; }

	vector<Playlist>* getPlaylists() { return &playlists; }
	HashTable<string, bool>& getLikedSongs() { return likedSongs; }

	vector<Song> getLikedSongsVector(MusicLibrary&lib) {
		vector<Song> likedSongsVector;
		for (const auto& pair : likedSongs.getAllItems()) {
			if (pair.second) {
				Song* songPtr = lib.findSongBySource(pair.first);
				if (songPtr) {
					likedSongsVector.push_back(*songPtr);
				}
			}
		}
		return likedSongsVector;
	}

	vector<RecommendationItem> getRecommendedSongs(MusicLibrary&lib) {
		vector<RecommendationItem> recommendations;
		vector<Song> allSongsVector = lib.getAllSongsVector();
		vector<Song> likedSongs = getLikedSongsVector(lib);

		if (allSongsVector.empty()) {
			return recommendations;
		}

		uint historyCount = sessionHistory.size();

		for (const Song& song : allSongsVector) {
			if (likesSong(song.getName())) continue;
			                  
			int score = 0;

			for (uint i = 0; i < historyCount; ++i) {
				Song listenedSong = sessionHistory.getAt(i);
				if (song.getAuthor() == listenedSong.getAuthor()) {
					score += 10;
				}
				if (lib.isKnownGenre(song.getGenre()) && lib.isKnownGenre(listenedSong.getGenre()) && song.getGenre() == listenedSong.getGenre()) {
					score += 15;
				}
			}

			for (const Song& likedSong : likedSongs) {
				if (lib.isKnownGenre(song.getGenre()) && lib.isKnownGenre(likedSong.getGenre()) && song.getGenre() == likedSong.getGenre()) {
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

	Stack<Song>* getSessionHistory() { return &sessionHistory; }

	void createDailyPlaylist(MusicLibrary&lib) {
		vector<Song> all = lib.getAllSongsVector();
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
};

