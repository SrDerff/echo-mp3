#pragma once
#include <string>
#include "DoublyLinkedList.h"
#include "Song.h"

using namespace std;

using uint = unsigned int;

/**
 * @file Artist.h
 * @brief Entidad que agrupa canciones bajo un artista especifico.
 *
 * Representa al autor/compositor y mantiene una sublista de sus canciones
 * disponibles en la biblioteca. Facilita la navegacion por artista.
 */

class Artist {
private:
    string name;                ///< Nombre del artista.
    string bio;                 ///< Biografia o descripcion corta.
    DoublyLinkedList<Song> songs;  ///< Coleccion de canciones de este artista.

public:
    Artist(string name = "", string bio = "") : name(name), bio(bio) {}

    // Todos los getters son const para poder usarse en objetos const y range-for const
    string getName() const { return name; }
    void setName(const string& n) { name = n; }

    string getBio() const { return bio; }
    void setBio(const string& b) { bio = b; }

    void addSong(const Song& s) { songs.insertAtTail(s); }
    DoublyLinkedList<Song>* getSongs() { return &songs; }

    uint getSongCount() const { return songs.getSize(); }
};