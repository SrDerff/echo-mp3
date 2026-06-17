#pragma once
#include <string>
#include "DoublyLinkedList.h"
#include "Song.h"

using uint = unsigned int;

/**
 * @file Album.h
 * @brief Entidad que agrupa canciones como un album o coleccion.
 *
 * Dado que Song no tiene campo explicito de album, se usa el genero como
 * agrupador inicial, o se pueden crear albumes personalizados.
 */

class Album {
private:
    string title;               ///< Titulo del album o coleccion.
    string artistName;          ///< Artista principal del album.
    int year;                   ///< Anio de lanzamiento.
    DoublyLinkedList<Song> tracks; ///< Pistas contenidas.

public:
    Album(string title = "", string artist = "", int year = 0)
        : title(title), artistName(artist), year(year) {
    }

    // Todos los getters son const para uso seguro en objetos const
    string getTitle() const { return title; }
    string getArtistName() const { return artistName; }
    int getYear() const { return year; }

    void addTrack(const Song& s) { tracks.insertAtTail(s); }
    DoublyLinkedList<Song>* getTracks() { return &tracks; }

    uint getTrackCount() const { return tracks.getSize(); }
};