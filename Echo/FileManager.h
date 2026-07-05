#pragma once
#include "MusicLibrary.h"
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include <cstdlib>
#include "User.h"

using namespace std;
namespace appFiles {
	const string SONGS_LIST_FILE = "..\\Data\\songsList.txt";
	const string USERS_LIST_FILE = "..\\Data\\users\\users_list.txt";
    const string USERS_FILE = "..\\Data\\users\\";
}

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
    /*
    Importancia:
    Funcion fundamental de persistencia que restaura el estado completo de la
    biblioteca musical desde el disco. Es el unico punto donde los datos del
    archivo "Data/songsList.txt" se convierten en objetos Song en memoria.
    Sin esta funcion la aplicacion no tendria canciones que reproducir.

    BigO: O(L x F + T x (N + A + B))
    Donde L es la cantidad de lineas del archivo, F es la cantidad de campos por
    linea (~7: nombre, autor, fuente, duracion, genero, playcount, liked), y T
    es la cantidad de canciones insertadas exitosamente. Por cada cancion se
    invoca `addSongToLibrary()` que tiene su propio costo O(N + A + B) como se
    detallo previamente. El parseo por linea involucra operaciones O(F) para
    los `getline`, `trim`, `stof`, `stoi` y comparaciones booleanas.
    Espacio O(T) para la biblioteca en memoria, mas O(L) temporal para el buffer
    de lectura linea por linea.

    Explicacion detallada:
    1. Abre el archivo songsList.txt en modo lectura. Si no se puede abrir
       (archivo faltante o sin permisos), retorna false y la biblioteca queda vacia.
    2. Llama a `lib.clear()` para resetear la biblioteca, eliminando cualquier
       dato previo y todos los indices. Esto asegura que una recarga no duplique
       canciones.
    3. Itera sobre cada linea del archivo usando `getline()`:
       a) Elimina el caracter de retorno de carro '\r' al final si existe
          (comun en archivos de Windows).
       b) Aplica `trim()` para eliminar espacios en blanco al inicio y final.
       c) Ignora lineas vacias.
    4. Para cada linea no vacia, usa un `stringstream` y extrae los 7 campos
       separados por '|' mediante `getline(ss, campo, '|')`:
       - name, author, source, durationStr, genre, playCountStr, likedStr.
    5. Crea una matriz dummy de 25x25 para la caratula (placeholder, no se usa).
    6. Convierte los campos numericos:
       - `durationStr` -> float con `stof()`. Si falla, usa 0.0.
       - `playCountStr` -> int con `stoi()`. Si falla, usa 0.
       - `likedStr` -> bool: true si es "1", "true" o "True".
    7. Construye un objeto `Song(name, author, source, duration, genre, dummyImg,
       playCount, liked)` y lo inserta en la biblioteca mediante `addSongToLibrary()`,
       que se encarga de indexarlo en todas las estructuras auxiliares.
    8. Si alguna linea esta mal formada (exception en stof/stoi), el catch(...)
       la ignora silenciosamente y continua con la siguiente linea, garantizando
       que el resto del archivo se cargue correctamente.
    9. Cierra el archivo y retorna true indicando exito.
    */
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

            char dummyImg[25][25];
            memset(dummyImg, ' ', sizeof(dummyImg));

            try {
                float duration = durationStr.empty() ? 0.0f : stof(durationStr);
                int playCount = playCountStr.empty() ? 0 : stoi(playCountStr);

                Song song(name, author, source, duration, genre, dummyImg, playCount);

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
                << s.getPlayCount() << "\n";
            curr = curr->next;
        }


        file.close();
        return true;
    }

    static void loadPlaylistsPerUser(MusicLibrary& lib, vector<Playlist>* playlist_storage, const string& username, const string& filepath) {
        ifstream file(filepath);
        if (!file.is_open()) return;

        string line;
        bool isPlaylistName = true;
        vector<string>stored_names;

        while (getline(file, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            line = trim(line);

            if (line.empty()) continue;

            if (isPlaylistName) {
                bool repeated = false;
                for (auto name : stored_names) {
                    if (name == line) {
                        repeated = true;
                        break;
                    }
                }

                if (repeated) {
                    while (getline(file, line)) {
                        if (!line.empty() && line.back() == '\r') line.pop_back();
                        line = trim(line);

                        if (line.empty()) continue;

                        if (line == "[ENDPLAYLIST]") {
                            isPlaylistName = true;
                            break;
                        }
                    }
                    continue;
                }

                stored_names.push_back(line);
                playlist_storage->push_back(Playlist(line));
                isPlaylistName = false;
                continue;
            }

            if (line == "[ENDPLAYLIST]") {
                isPlaylistName = true;
                continue;
            }

            Song song = findSongBySource(lib, line);

            //Song() representa una cancion vacia, misma que es devuelta solo si no se encuentra la cancion en la biblioteca
            if (song != Song()) {
                playlist_storage->back().addSong(song);
            }
        }
    }

    static void loadLikesPerUser(HashTable<string, bool>& likes, MusicLibrary& lib, const string& filepath) {
        ifstream file(filepath);
        if (!file.is_open()) return;
        vector<string>liked_songs;
        string line;
        while (getline(file, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            line = trim(line);
        if (line.empty()) continue;

        likes.insert(line, true);
        }
    }

    static void loadHistoryPerUser(Stack<Song>* sessionHistory, MusicLibrary& lib, const string& filepath) {
        ifstream file(filepath);
        if (!file.is_open()) return;

        string line;
        while (getline(file, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            line = trim(line);
            if (line.empty()) continue;

            Song song = findSongBySource(lib, line);
            if (song != Song()) {
                sessionHistory->push(song);
            }
        }
    }

    static void savePlaylistsPerUser(vector<Playlist>* playlists, const string& filepath) {
        ofstream file(filepath);
        if (!file.is_open()) return;

        for (auto pl : *playlists) {
            file << pl.getName() << endl;
            for (size_t i = 0; i < pl.getSize(); ++i) {
                Song n_song = pl.getSongAt(i);
                file << n_song.getSource() << endl;
            }
            file << "[ENDPLAYLIST]" << endl;
        }
    }

    static void saveLikesPerUser(const HashTable<string, bool>& likes, const string& filepath) {
        ofstream file(filepath);
        if (!file.is_open()) return;

        vector<pair<string, bool>> likes_storage = likes.getAllItems();
        for (size_t i = 0; i < likes_storage.size(); ++i) {
            if (likes_storage[i].second) {
                file << likes_storage[i].first << endl;
            }
        }
    }

    static void saveHistoryPerUser(const Stack<Song>* history_storage, const string& filepath) {
        ofstream file(filepath);
        if (!file.is_open()) return;

        Stack<Song> temp = *history_storage;
        while (!temp.isEmpty()) {
            Song song = temp.getAt(0);
            temp.pop();
            file << song.getSource() << endl;
        }
    }

    /*
    Importancia:
    Generador de cuentas demo que crea 5 usuarios sinteticos (Demo1 a Demo5)
    con datos predefinidos para pruebas y demostracion del sistema. Cada cuenta
    incluye: una Daily Mix generada por el algoritmo de optimizacion de 30
    minutos, entre 2 y 3 playlists tematicas con canciones aleatorias de la
    biblioteca, una seleccion de canciones marcadas como "me gusta", y un
    historial de reproduccion simulado. Estas cuentas existen unicamente en
    memoria durante la ejecucion del programa y NO se persisten en disco,
    a diferencia de las cuentas creadas manualmente por el usuario mediante
    el formulario de registro. Esto permite al usuario explorar todas las
    funcionalidades del reproductor sin necesidad de registrarse ni tener
    datos reales.

    BigO: O(N * (P + L + H))
    Donde N es la cantidad de cuentas a generar (5 fijo), P es la cantidad
    de playlists por cuenta (2-3), L es la cantidad de likes (3-10),
    y H es la cantidad de entradas en el historial (3-7).
    Por cada cuenta:
    - `generateThirtyMinMix()` construye un grafo de canciones y ejecuta
      un algoritmo de seleccion con insercion parcial O(C^2) donde C es
      la cantidad de canciones candidatas en el rango de duracion.
    - Las playlists se llenan iterando sobre indices aleatorios del vector
      `allSongs` en O(S) donde S es la cantidad de canciones por playlist.
    - Los likes y el historial tambien usan seleccion aleatoria O(1) por
      operacion sobre el vector de canciones.
    Espacio: O(N * (P * S + L + H)) para almacenar todas las cuentas en
    memoria, mas O(C) temporal para la Daily Mix de cada cuenta.

    Explicacion detallada:
    1. Inicializa el generador de numeros aleatorios con `srand(time(nullptr))`
       para que cada ejecucion produzca datos distintos.
    2. Define un arreglo fijo de 5 nombres de usuario: "Demo1" a "Demo5",
       todos con contrasena "demo". Crea un vector `accounts` vacio.
    3. Para cada iteracion del ciclo (5 veces):
       a) Crea un nuevo User directamente en el vector mediante
          `emplace_back(nombre, "demo", false)`. Se obtiene una referencia
          `User& user` al elemento recien creado para trabajar directamente
          sobre el objeto dentro del vector, evitando copias temporales.
       b) Obtiene todas las canciones de la biblioteca como un vector
          (`allSongs`) mediante `getAllSongsVector()`.
       c) Genera una Daily Mix llamando a `lib.generateThirtyMinMix()`,
          que selecciona canciones hasta completar ~30 minutos. Si la mix
          tiene canciones, se agrega como playlist "Daily Mix" y se copian
          todas las canciones desde el objeto Playlist retornado.
       d) Crea entre 2 y 3 playlists adicionales (aleatorio) con nombres
          "Playlist 1", "Playlist 2", etc. Cada playlist se llena con
          entre 3 y 8 canciones seleccionadas aleatoriamente del vector
          `allSongs`, usando indices aleatorios con `rand() % allSongs.size()`.
       e) Marca entre 3 y 10 canciones como "me gusta" seleccionando
          indices aleatorios y llamando a `user.likeSong(source)`.
       f) Agrega entre 3 y 7 canciones al historial de sesion mediante
          `user.addToSessionHistory(cancion)`, tambien con seleccion
          aleatoria, simulando reproducciones recientes.
    4. Retorna el vector `accounts` con las 5 cuentas demo completamente
       configuradas. El AppController las almacena en `fakeAccounts` y
       las ofrece como opcion de inicio de sesion junto a las cuentas reales.
    */
    static vector<User> generateFakeAccounts(MusicLibrary& lib) {
        vector<User> accounts;
        srand(static_cast<unsigned>(time(nullptr)));

        string names[] = { "Demo1", "Demo2", "Demo3", "Demo4", "Demo5" };
        string password = "demo";

        for (int i = 0; i < 5; i++) {
            accounts.emplace_back(names[i], password, false);
            User& user = accounts.back();

            vector<Song> allSongs = lib.getAllSongsVector();

            // Daily Mix
            Playlist dailyMix = lib.generateThirtyMinMix("Daily Mix");
            if (dailyMix.getSize() > 0) {
                user.addPlaylist("Daily Mix");
                Playlist* p = user.getPlaylist(user.getPlaylistCount() - 1);
                if (p) {
                    for (uint j = 0; j < dailyMix.getSize(); j++) {
                        p->addSong(dailyMix.getSongAt(j));
                    }
                }
            }

            // Playlists aleatorias
            int numPlaylists = 2 + rand() % 2;
            for (int p = 0; p < numPlaylists; p++) {
                string plName = "Playlist " + to_string(p + 1);
                user.addPlaylist(plName);
                Playlist* pl = user.getPlaylist(user.getPlaylistCount() - 1);
                if (!pl) continue;
                int numSongs = 3 + rand() % 6;
                for (int s = 0; s < numSongs && !allSongs.empty(); s++) {
                    int idx = rand() % allSongs.size();
                    pl->addSong(allSongs[idx]);
                }
            }

            // Likes aleatorios
            int numLiked = 3 + rand() % 8;
            for (int s = 0; s < numLiked && !allSongs.empty(); s++) {
                int idx = rand() % allSongs.size();
                user.likeSong(allSongs[idx].getSource());
            }

            // Historial aleatorio
            int numHistory = 3 + rand() % 5;
            for (int s = 0; s < numHistory && !allSongs.empty(); s++) {
                int idx = rand() % allSongs.size();
                user.addToSessionHistory(allSongs[idx]);
            }
        }
        return accounts;
    }

    /*
    Importancia:
    Funcion de alto nivel que orquesta la restauracion completa del estado persistido
    de la aplicacion. Es llamada desde `AppController::AppController()` al iniciar
    el programa. Sin esta funcion la biblioteca musical arrancarfa vacia cada vez.

    BigO: O(S + P + C)
    Donde S es la cantidad de canciones en el archivo songsList.txt, P es la cantidad
    de playlists en playlists.txt, y C es la cantidad total de referencias a canciones
    dentro de todas las playlists. Cada cancion cargada invoca `addSongToLibrary()`
    que a su vez inserta en todos los indices auxiliares (O(N + A + B) por cancion).
    Espacio O(N) adicional para almacenar en memoria toda la biblioteca resultante.

    Explicacion detallada:
    1. Llama a `loadSongs()` que abre "Data/songsList.txt", lee linea por linea,
       parsea los campos separados por '|', construye objetos Song y los inserta
       en la biblioteca mediante `addSongToLibrary()`.
    2. Llama a `loadPlaylists()` que abre "Data/playlists.txt", lee nombres de
       playlists seguidos de referencias a canciones (rutas de archivo) hasta
       encontrar la marca "[ENDPLAYLIST]", y las rellena buscando cada cancion
       en la biblioteca ya cargada mediante `findSongBySource()`.
    3. Retorna true solo si ambas operaciones fueron exitosas; si falla alguna,
       la biblioteca puede quedar en un estado parcialmente cargado.
    */
    static bool loadLibrary(MusicLibrary& lib) {
        return loadSongs(lib);
    }

    static bool saveLibrary(const MusicLibrary& lib) {
        return saveSongs(lib);
    }

    //accounts

    static void loadAccountConfig(const string& username, User& user, MusicLibrary& musicLib) {
        // Load user-specific data (playlists, liked songs, etc.) from files

        //paths
		string playlistsFilePath = "..\\Data\\users\\" + username + "_playlists.txt";
		string likesFilePath = "..\\Data\\users\\" + username + "_likes.txt";
		string historyFilePath = "..\\Data\\users\\" + username + "_history.txt";
        
        //loading
		loadPlaylistsPerUser(musicLib, user.getPlaylists(), username, playlistsFilePath);
        loadLikesPerUser(user.getLikedSongs(), musicLib, likesFilePath);
        loadHistoryPerUser(user.getSessionHistory(), musicLib, historyFilePath);
    }
    
    static void saveAccountConfigs(User& user) {
        // Save user-specific data (playlists, liked songs, etc.) to files
        string playlistsFilePath = "..\\Data\\users\\" + user.getUsername()  + "_playlists.txt";
        string likesFilePath = "..\\Data\\users\\" + user.getUsername() + "_likes.txt";
        string historyFilePath = "..\\Data\\users\\" + user.getUsername() + "_history.txt";

        //saving
		savePlaylistsPerUser(user.getPlaylists(), playlistsFilePath);
		saveLikesPerUser(user.getLikedSongs(), likesFilePath);
		saveHistoryPerUser(user.getSessionHistory(), historyFilePath);
	}

    static bool findAccount(const string& username) {
        ifstream file(appFiles::USERS_LIST_FILE);
        if (!file.is_open()) return false;

        string line;
        while (getline(file, line)) {
            if (line.substr(0, line.find('|')) == username) {
                file.close();
                return true;
            }
        }

        file.close();
        return false;
    }

    static void addAccount(const string& username, const string& hashPassword, bool premium=false) {
        ofstream file(appFiles::USERS_LIST_FILE, ios::app);
        if (!file.is_open()) return;
		file << username << "|" << hashPassword << "|" << (premium ? "1" : "0") << "\n";
        ofstream playlists_file(appFiles::USERS_FILE + username + "_playlists.txt");
        ofstream likes_file(appFiles::USERS_FILE + username + "_likes.txt");
        ofstream history_file(appFiles::USERS_FILE + username + "_history.txt");
    }
};
