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
#include <vector>
#include "MergeSort.h"
#include <cctype>

using namespace std;
using uint = unsigned int;

class MusicLibrary {
private:
    DoublyLinkedList<Song> allSongs;
    CircularDoublyLinkedList<Song> queue;
    vector<Artist> artists;
    vector<Album> albums;

    // ===== INDICES PARA BUSQUEDA OPTIMIZADA =====
    HashTable<string, Song*> indexBySource;           // O(1) por ruta de archivo
    HashTable<string, vector<Song*>> indexByArtist;  // Agrupacion por artista
    HashTable<string, vector<Song*>> indexByGenre;    // Agrupacion por genero

    RedBlackTree<string, Song> indexByName;           // Orden alfabetico por nombre
    AVLTree<float, Song> indexByDuration;             // Rango de duracion
    AVLTree<int, Song> indexByPlayCount;             // Top canciones reproducidas

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

public: bool isKnownGenre(const string& genre) {
        return genre == "Rock" || genre == "Pop" || genre == "Hip Hop"
            || genre == "Latin" || genre == "Electronic" || genre == "Ballad";
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

public:
    MusicLibrary(){
        
    }

    /*
    Importancia:
    Funcion nuclear que inserta una cancion en la lista principal y la indexa en
    los 6 sistemas de busqueda optimizada (hash por fuente, hash por artista,
    hash por genero, arbol rojo-negro por nombre, AVL por duracion, AVL por
    reproducciones). Tambien actualiza las entidades de artista y album.
    Es la unica puerta de entrada para incorporar canciones a la biblioteca.

    BigO: O(N + A + B)
    Donde N es la cantidad actual de canciones (insercion al final de la lista
    enlazada: O(1) si hay puntero a la cola, pero puede ser O(N) si no), A es la
    cantidad de artistas existentes (busqueda lineal para encontrar coincidencia) y
    B es la cantidad de albumes existentes (busqueda lineal similar).
    Las inserciones en los indices HashTable son O(1) promedio y O(N) en el peor
    caso de colisiones. Las inserciones en RedBlackTree y AVLTree son O(log M)
    donde M es la cantidad de elementos en cada arbol.
    Espacio O(1) adicional, pues la cancion ya se almacena en la lista y los indices
    solo guardan punteros o copias ligeras.

    Explicacion detallada:
    1. Toma una copia de la cancion (`normalizedSong`) para no modificar el original.
    2. Normaliza el genero invocando `normalizeGenre()`, que asigna un genero
       estandarizado (Rock, Pop, Hip Hop, Latin, Electronic, Ballad, Other) segun
       una tabla fija de nombres de cancion conocidos. Esto garantiza consistencia
       en los filtros por genero y en el sistema de recomendaciones.
    3. Inserta la cancion al final de la lista doblemente enlazada `allSongs`.
    4. Obtiene un puntero al nodo recien insertado para trabajar con la misma
       instancia en memoria en todos los indices.
    5. Indexa en los 6 sistemas de busqueda:
       - `indexBySource`: HashTable clave=fuente(ruta archivo), valor=puntero a Song.
       - `indexByArtist`: HashTable clave=nombre artista, valor=vector de punteros.
       - `indexByGenre`: HashTable clave=genero normalizado, valor=vector de punteros.
       - `indexByName`: RedBlackTree clave=nombre cancion, valor=copia de Song.
       - `indexByDuration`: AVLTree clave=duracion (float), valor=copia de Song.
       - `indexByPlayCount`: AVLTree clave=conteo reproducciones (int), valor=copia.
    6. Busca si el artista ya existe en el vector `artists`. Si existe, agrega la
       cancion a su lista; si no, crea un nuevo artista y lo agrega al vector.
    7. Busca si existe un album con el mismo titulo (genero) y mismo artista.
       Si existe, agrega la cancion como pista; si no, crea un nuevo album y lo
       agrega al vector. Esto organiza las canciones en una estructura jerarquica
       Artista > Album > Cancion.
    */
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
        while(curr!=nullptr && curr->getValue().getName() != name) {
            curr = curr->next;
		}
		return curr ? &(curr->getValue()) : nullptr;
	}

    /**/

    void removeSongFromLibrary(const Song& song) {
        allSongs.removeNode(song);

        indexBySource.remove(song.getSource());
        indexByName.remove(song.getName());
        indexByDuration.remove(song.getDuration());
        indexByPlayCount.remove(song.getPlayCount());

        vector<Song*>* artistVec = indexByArtist.findPtr(song.getAuthor());
        if (artistVec) {
            auto it = remove_if(artistVec->begin(), artistVec->end(),
                [&](Song* s) { return s->getSource() == song.getSource(); });
            artistVec->erase(it, artistVec->end());
            if (artistVec->empty()) indexByArtist.remove(song.getAuthor());
        }

        vector<Song*>* genreVec = indexByGenre.findPtr(song.getGenre());
        if (genreVec) {
            auto it = remove_if(genreVec->begin(), genreVec->end(),
                [&](Song* s) { return s->getSource() == song.getSource(); });
            genreVec->erase(it, genreVec->end());
            if (genreVec->empty()) indexByGenre.remove(song.getGenre());
        }
    }

    DoublyLinkedList<Song>* getAllSongs() { return &allSongs; }

    /*void addSongToPlaylist(int playlistIndex, const Song& song) {
        Playlist* p = getPlaylist(playlistIndex);
        if (p) p->addSong(song);
    }*/

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

    /**/

    /*
    Importancia:
    Implementa el algoritmo de recomendacion del reproductor. Analiza el historial
    de reproduccion de la sesion actual y las canciones marcadas con "Me gusta"
    para calcular un puntaje de relevancia para cada cancion no-likeada.
    Es la base de la pestana "Recom." y permite al usuario descubrir musica
    relacionada con sus gustos actuales.

    BigO: O(T x (H + L))
    Donde T es la cantidad total de canciones en la biblioteca (sin incluir liked),
    H es el tamano del historial de sesion (Stack) y L es la cantidad de canciones
    liked. Por cada cancion candidata se recorre todo el historial (H) y todas
    las liked (L) para acumular puntajes.
    Espacio O(T) para almacenar el vector de recomendaciones resultante, mas O(T)
    temporal por las copias de `getAllSongsVector()` y `getLikedSongsVector()`.

    Explicacion detallada:
    1. Obtiene dos vectores auxiliares: todas las canciones (`getAllSongsVector`)
       y solo las canciones liked (`getLikedSongsVector`).
    2. Si no hay canciones en la biblioteca, retorna un vector vacio.
    3. Obtiene la cantidad de elementos en el historial de sesion (`sessionHistory`),
       que es un Stack donde cada reproduccion agrega un elemento.
    4. Itera sobre todas las canciones de la biblioteca. Omite las que ya estan
       liked (no tiene sentido recomendar lo que ya le gusta al usuario).
    5. Para cada cancion candidata calcula un puntaje inicializado en 0:
       a) Recorre todo el historial de sesion y por cada cancion escuchada:
          - Si coincide el artista, suma +10 puntos.
          - Si ambos tienen generos conocidos y coinciden, suma +15 puntos.
       b) Recorre todas las canciones liked y por cada una:
          - Si ambos tienen generos conocidos y coinciden, suma +20 puntos.
            (La coincidencia de genero con liked pesa mas que con historial)
    6. Agrega la cancion y su puntaje como un `RecommendationItem` al vector.
    7. Retorna el vector que luego puede ser ordenado por QuickSort segun el
       puntaje para mostrar las mas relevantes primero.
    */

    /**/

    /**/

    /**/

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
        Song* found = indexByName.searchPtr(name);
        return found;
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

    /*
    Importancia:
    Implementa la busqueda por substring (coincidencia parcial) sobre el nombre y
    el artista de cada cancion. Es el metodo de respaldo cuando la busqueda por
    prefijo (que usa el arbol rojo-negro) no encuentra resultados. Permite al
    usuario encontrar canciones incluso si no recuerda el nombre exacto o escribe
    una parte del titulo o del artista.

    BigO: O(N x (K + M))
    Donde N es la cantidad de canciones en la biblioteca, K es la longitud del
    query (despues de normalizar a minusculas), y M es la longitud promedio de
    los campos name y author (tambien normalizados). Por cada cancion se aplica
    `string::find()` que internamente tiene complejidad O(K x M) en el peor caso
    (algoritmo de busqueda ingenua sin KMP). Adicionalmente, cada normalizacion
    recorre el string completo O(len) para convertir a minusculas.
    Espacio O(N) para el vector de resultados, mas O(K + M) temporal para las
    conversiones a minusculas de query, name y author en cada iteracion.

    Explicacion detallada:
    1. Si el query esta vacio, retorna todas las canciones de la biblioteca
       (equivalente a `getAllSongsVector()`).
    2. Convierte el query completo a minusculas usando `toLowerStr()` para que
       la busqueda sea case-insensitive (no distingua mayusculas/minusculas).
    3. Recorre toda la lista doblemente enlazada `allSongs` con un puntero `curr`
       desde la cabeza hasta el final (O(N)).
    4. Para cada nodo:
       a) Obtiene la referencia a la cancion (`curr->getValue()`).
       b) Convierte `song.getName()` y `song.getAuthor()` a minusculas.
       c) Verifica si el query normalizado es substring del nombre normalizado
          usando `lowerName.find(lowerQuery)`. Si no coincide, verifica tambien
          contra el autor con `lowerArtist.find(lowerQuery)`.
       d) Si alguna de las dos verificaciones es positiva (devuelve != npos),
          agrega la cancion al vector de resultados.
    5. Retorna el vector con todas las canciones que coinciden parcialmente.
       La interfaz se encarga de mostrar estos resultados ordenados segun el
       orden de la lista original (por insercion).
    */
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

private:
    static string toLowerStr(string str) {
        for (char& c : str) {
            c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
        }
        return str;
    }
};
