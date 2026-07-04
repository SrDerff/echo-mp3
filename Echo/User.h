#pragma once
#include <string>
<<<<<<< HEAD
=======
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <iostream>
>>>>>>> 0c6f778 (AvanceNew)
#include "Playlist.h"
#include "HashTable.h"
#include "Stack.h"
#include "Song.h"
<<<<<<< HEAD
=======
#include "MusicLibrary.h"
#include "Graph.h"
#include "QuickSort.h"
#include "HeapSort.h"
>>>>>>> 0c6f778 (AvanceNew)
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
<<<<<<< HEAD
	///TODO
=======
>>>>>>> 0c6f778 (AvanceNew)
	User(const string& username, const string& hashPassword, bool premium = false) {
		this->username = username;
		this->hashPassword = hashPassword;
		this->premium = premium;
		playlists.reserve(100);
	}
<<<<<<< HEAD
	///TODO
=======
>>>>>>> 0c6f778 (AvanceNew)
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
<<<<<<< HEAD
		// Implementation for unliking a song
=======
>>>>>>> 0c6f778 (AvanceNew)
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

<<<<<<< HEAD
	vector<RecommendationItem> getRecommendedSongs(MusicLibrary&lib) {
		vector<RecommendationItem> recommendations;
		vector<Song> allSongsVector = lib.getAllSongsVector();
		vector<Song> likedSongs = getLikedSongsVector(lib);
=======
	vector<RecommendationItem> getRecommendedSongs(MusicLibrary& lib) {
		vector<RecommendationItem> recommendations;
		vector<Song> allSongsVector = lib.getAllSongsVector();
		vector<Song> likedSongsVector = getLikedSongsVector(lib);
>>>>>>> 0c6f778 (AvanceNew)

		if (allSongsVector.empty()) {
			return recommendations;
		}

<<<<<<< HEAD
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
=======
		CGrafo<Song, int> graph;
		for (const Song& song : allSongsVector) {
			graph.adicionarVertice(song);
		}

		auto sameArtist = [](const Song& left, const Song& right) {
			return left.getAuthor() == right.getAuthor();
		};

		auto sameKnownGenre = [&lib](const Song& left, const Song& right) {
			return lib.isKnownGenre(left.getGenre())
				&& lib.isKnownGenre(right.getGenre())
				&& left.getGenre() == right.getGenre();
		};

		auto relationWeight = [&](const Song& left, const Song& right) {
			int weight = 0;
			if (sameArtist(left, right)) {
				weight += 10;
			}
			if (sameKnownGenre(left, right)) {
				weight += 15;
			}
			return weight;
		};

		for (int source = 0; source < graph.cantidadVertices(); ++source) {
			for (int target = source + 1; target < graph.cantidadVertices(); ++target) {
				int weight = relationWeight(graph.obtenerVertice(source), graph.obtenerVertice(target));
				if (weight > 0) {
					graph.adicionarArco(source, target, weight);
					graph.adicionarArco(target, source, weight);
				}
			}
		}

		vector<int> scores(allSongsVector.size(), 0);

		auto applySeed = [&](const Song& seed, bool fromLiked) {
			int seedIndex = graph.buscarVerticeSi([&](const Song& current) {
				return current.getSource() == seed.getSource();
			});

			if (seedIndex < 0) {
				return;
			}

			graph.recorrerVertice(seedIndex, [&](int neighborIndex, int weight, const Song& neighbor) {
				if (neighbor.getSource() == seed.getSource()) {
					return;
				}

				if (fromLiked) {
					if (sameKnownGenre(seed, neighbor)) {
						scores[neighborIndex] += 20;
					}
				}
				else {
					if (sameArtist(seed, neighbor)) {
						scores[neighborIndex] += 10;
					}
					if (sameKnownGenre(seed, neighbor)) {
						scores[neighborIndex] += 15;
					}
				}
			});
		};

		uint historyCount = sessionHistory.size();
		for (uint index = 0; index < historyCount; ++index) {
			applySeed(sessionHistory.getAt(index), false);
		}

		for (const Song& likedSong : likedSongsVector) {
			applySeed(likedSong, true);
		}

		for (int index = 0; index < static_cast<int>(allSongsVector.size()); ++index) {
			if (isSongLiked(allSongsVector[index].getSource())) {
				continue;
			}
			recommendations.emplace_back(allSongsVector[index], scores[index], "Afinidad por grafo");
		}

		if (!recommendations.empty()) {
			QuickSort::quickSort(recommendations, 0, static_cast<int>(recommendations.size()) - 1, true);
>>>>>>> 0c6f778 (AvanceNew)
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

<<<<<<< HEAD
=======


>>>>>>> 0c6f778 (AvanceNew)
