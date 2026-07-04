#pragma once
#include "DoublyLinkedList.h"
#include "Song.h"
#include <iostream>

/**
 * @file Playlist.h
 * @brief Entidad que representa una lista de reproduccion.
 *
 * Combina una DoublyLinkedList de canciones con un reproductor (GestorAudio),
 * un historial de navegacion (Stack) y una cola de siguientes (Queue).
 * Gestiona la logica de reproduccion: play, pause, next, previous, loop.
 */

class Playlist {
private:
    string name;
    DoublyLinkedList<Song> songs;  ///< Coleccion de canciones de la playlist.
    bool loop;                     ///< Si true, al terminar la lista vuelve al inicio.

public:
    Playlist(string name = "Nueva Playlist")
        : name(name), loop(false) {
    }

    ~Playlist() {
    }

    void setName(const string& n) { name = n; }

    string getName() const { return name; }

    void addSong(const Song& song) {
        songs.insertAtTail(song);
    }

    bool removeSong(const Song& song) {
        return songs.removeNode(song);
    }

    void removeSong(int index) {

        if (index < 0 || index >= static_cast<int>(songs.getSize()))
            return;

        Song song = songs.getAt(index);

        songs.removeNode(song);
    }

    uint getSize() const { return songs.getSize(); }

    bool isEmpty() const { return songs.isEmpty(); }

    Song getSongAt(uint index) const {
        return songs.getAt(index);
    }

private:
    Node<Song>* findSongNode(const Song& song) {

        Node<Song>* curr = songs.getHead();

        while (curr != nullptr) {

            if (curr->getValue() == song)
                return curr;

            curr = curr->next;
        }

        return nullptr;
    }
};