#pragma once
#include <string>
#include <iostream>

using namespace std;

/**
 * @file Song.h
 * @brief Entidad que representa una cancion en el sistema.
 *
 * Almacena metadatos basicos: nombre, autor, ruta de archivo, duracion, genero
 * y una imagen ASCII de 25x25 caracteres. Implementa operator== para permitir
 * busquedas y eliminaciones en estructuras de datos.
 */

class Song
{
private:
    string name;        ///< Titulo de la cancion.
    string author;      ///< Nombre del artista/autor.
    string source;      ///< Ruta absoluta o relativa del archivo MP3 en disco.
    float duration;     ///< Duracion en segundos.
    string genre;       ///< Genero musical (o album si se prefiere).
    char img[25][25];   ///< Matriz de caracteres ASCII para "caratula" en consola.
    int count;
    bool liked;
public:
    /**
     * @brief Constructor por defecto. Inicializa strings vacios y duracion 0.
     * memset se usa para llenar la matriz img con espacios en blanco.
     */
    Song() : name(""), author(""), source(""), duration(0.0f), genre(""), count(0), liked(false) {
        clearImg();
    }

    /**
     * @brief Constructor parametrizado.
     * @param name Titulo.
     * @param author Artista.
     * @param source Ruta del archivo.
     * @param duration Duracion en segundos.
     * @param genre Genero musical.
     * @param img Matriz 25x25 de caracteres ASCII (caratula).
     * @param count Numero de reproducciones
     * @param liked Likeado o no likeado
     *
     * Se copia la matriz img elemento por elemento para evitar compartir memoria.
     */
    Song(string name, string author, string source, float duration, string genre, char img[25][25], int count, bool liked)
        : name(name), author(author), source(source), duration(duration), genre(genre), count(count), liked(liked) {
        for (int i = 0; i < 25; i++)
            for (int j = 0; j < 25; j++)
                this->img[i][j] = img[i][j];
    }

    ~Song() {}

    // --- Getters ---
    string getName() const { return name; }
    string getAuthor() const { return author; }
    string getSource() const { return source; }
    float getDuration() const { return duration; }
    string getGenre() const { return genre; }
    int getPlayCount() const { return count; }
    bool isLiked() const { return liked; }

    // --- Setters ---
    void setName(const string& n) { name = n; }
    void setAuthor(const string& a) { author = a; }
    void setSource(const string& s) { source = s; }
    void setDuration(float d) { duration = d; }
    void setGenre(const string& g) { genre = g; }
    void setPlayCount(int count) { this->count = count; }
    void setLiked(bool value) { liked = value; }

    void getImg(char dest[25][25]) const {
        for (int i = 0; i < 25; i++)
            for (int j = 0; j < 25; j++)
                dest[i][j] = img[i][j];
    }

    void setImg(char src[25][25]) {
        for (int i = 0; i < 25; i++)
            for (int j = 0; j < 25; j++)
                img[i][j] = src[i][j];
    }

    void clearImg() {
        for (int i = 0; i < 25; i++)
            for (int j = 0; j < 25; j++)
                img[i][j] = ' ';
    }

    /**
     * @brief Comparacion por igualdad. Necesaria para removeNode en DoublyLinkedList.
     * Dos canciones son iguales si coinciden nombre, autor y ruta de archivo.
     */
    bool operator==(const Song& other) const {
        return name == other.name && author == other.author && source == other.source;
    }

    bool operator!=(const Song& other) const {
        return !(*this == other);
    }

    void printInfo() const {
        cout << "  Cancion: " << name << "\n";
        cout << "  Artista: " << author << "\n";
        cout << "  Album/Genero: " << genre << "\n";
        cout << "  Duracion: " << duration << "s\n";
        cout << "  Archivo: " << source << "\n";
    }
};