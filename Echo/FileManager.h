#pragma once
#include "MusicLibrary.h"
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include <cstdlib>
<<<<<<< HEAD
=======
#include <algorithm>
>>>>>>> 0c6f778 (AvanceNew)
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
    /*
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
    }*/

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
            for (int i = 0; i < pl.getSize(); i++) {
                Song n_song = pl.getSongAt(i);
                file << n_song.getSource() << endl;
            }
            file << "[ENDPLAYLIST]" << endl;
        }
    }

    static void saveLikesPerUser(const HashTable<string, bool>& likes, const string& filepath) {
        ofstream file(filepath);
        if (!file.is_open()) return;

        vector<pair<string, bool>>likes_storage=likes.getAllItems();
        for (int i = 0; i < likes.size(); i++) {
            if (likes_storage[i].second) {
                file << likes_storage[i].first<<endl;
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
    }*/

    /*
    Importancia:
    Generador de datasets de prueba que expande las canciones originales del archivo
    "Data/songsList.txt" en un conjunto multiplo con metadatos variados (genero,
    playcount, liked) y sufijos en el nombre. Es la unica herramienta para crear
    datos de prueba masivos sin necesidad de archivos de audio adicionales, permitiendo
    evaluar el rendimiento de los algoritmos de ordenamiento (MergeSort, QuickSort,
    HeapSort) y de las estructuras de datos (HashTable, RedBlackTree, AVLTree) con
    cargas de trabajo realistas. Sin esta funcion, las pruebas de rendimiento estarian
    limitadas a las 60 canciones originales.

    BigO: O(B x M + T x G)
    Donde B es la cantidad de canciones en el archivo backup (~60), M es el multiplicador
    (default 5), y T es el total de lineas generadas (B x M). Por cada linea de
    salida se ejecutan en O(1): seleccion aleatoria de genero, consulta de sufijo,
    generacion de playCount y liked, y escritura formateada. El parseo de cada
    linea plantilla es O(F) donde F son los 7 campos separados por '|'.
    Espacio O(T + B) en memoria: O(B) para almacenar las lineas plantilla y O(1)
    adicional para el buffer de escritura linea por linea. En disco se escriben
    T lineas (el nuevo songsList.txt) y se preserva el backup original con B lineas.

    Explicacion detallada:
    1. Verifica si el archivo de backup "songsList_backup.txt" existe en disco.
       - Si NO existe: es la primera ejecucion del programa tras agregar el
         generador. Abre el songsList.txt actual, copia cada linea intacta al
         backup, y cierra ambos archivos. Esto preserva el dataset original.
       - Si SI existe: salta la creacion del backup, pues ya fue respaldado
         en una ejecucion anterior. El backup jamas se sobrescribe.
    2. Abre el archivo de backup en modo lectura como plantilla base. Si no se
       puede abrir (archivo faltante o sin permisos), retorna false.
    3. Inicializa la semilla del generador aleatorio con `srand(time(nullptr))`
       para que cada ejecucion produzca datos distintos.
    4. Define un arreglo de estructuras `GenreSuffix` que mapea cada uno de los
       16 generos a un sufijo textual (ej: "Hip-Hop" -> " - Remix", "Rock" ->
       " - Rock Version"). Esto asegura que el nombre mostrado en pantalla
       refleje el genero asignado.
    5. Itera sobre cada linea del backup usando `getline()`:
       a) Elimina el caracter de retorno de carro '\r' al final si existe.
       b) Aplica `trim()` para eliminar espacios en blanco al inicio y final.
       c) Ignora lineas vacias.
       d) Agrega la linea limpia al vector `templateLines`.
    6. Cierra el archivo de backup y abre (o sobrescribe) el archivo de salida
       "Data/songsList.txt" en modo escritura.
    7. Para cada linea plantilla en `templateLines`:
       a) Usa un `stringstream` para extraer los 7 campos separados por '|':
          name, author, source, durationStr, genre, playCountStr, likedStr.
       b) Repite `multiplier` veces (default 5):
          i)  Selecciona un indice aleatorio entre 0 y 15 (16 generos).
          ii) Obtiene el genero y el sufijo correspondiente desde `genreMap`.
          iii) Genera playCount aleatorio entre 0 y 500 (`rand() % 501`).
          iv) Genera liked aleatorio (0 o 1) mediante `rand() % 2`.
          v)  Escribe la linea de salida en el formato:
              `nombre + sufijo | autor | source | duracion | genero | playCount | liked`
    8. Cierra el archivo de salida y retorna true indicando exito.
    */
    static bool generateDataset(int multiplier = 5, const string& outputFilename = "..\\Data\\songsList.txt", const string& backupFilename = "..\\Data\\songsList_backup.txt") {
        // Primera ejecucion: crear backup desde el songsList.txt original
        {
            ifstream checkBackup(backupFilename);
            bool backupExists = checkBackup.is_open();
            checkBackup.close();

            if (!backupExists) {
                ifstream original(outputFilename);
                if (!original.is_open()) return false;
                ofstream backup(backupFilename);
                string line;
                while (getline(original, line)) {
                    backup << line << "\n";
                }
                original.close();
                backup.close();
            }
        }

        // Leer siempre desde el backup (original limpio)
        ifstream input(backupFilename);
        if (!input.is_open()) return false;

        srand(static_cast<unsigned>(time(nullptr)));

        struct GenreSuffix {
            const char* genre;
            const char* suffix;
        };
        GenreSuffix genreMap[] = {
            {"Hip-Hop", " - Remix"},
            {"Electronic", " - Remix"},
            {"Reggaeton", " - Remix"},
            {"Rock", " - Rock Version"},
            {"Metal", " - Rock Version"},
            {"Pop", " - Pop Version"},
            {"K-Pop", " - Pop Version"},
            {"Alternative", " - Alt Version"},
            {"Indie", " - Alt Version"},
            {"R&B", " - RnB Mix"},
            {"Soul", " - RnB Mix"},
            {"Funk", " - RnB Mix"},
            {"Blues", " - RnB Mix"},
            {"Jazz", " - Jazz Version"},
            {"Country", " - Country Mix"},
            {"Latin", " - Latin Version"}
        };
        const int genreCount = sizeof(genreMap) / sizeof(genreMap[0]);

        vector<string> templateLines;
        string line;
        while (getline(input, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            line = trim(line);
            if (!line.empty()) templateLines.push_back(line);
        }
        input.close();

        ofstream output(outputFilename);
        if (!output.is_open()) return false;

        for (const string& tpl : templateLines) {
            stringstream ss(tpl);
            string name, author, source, durationStr, genre, playCountStr, likedStr;
            getline(ss, name, '|');
            getline(ss, author, '|');
            getline(ss, source, '|');
            getline(ss, durationStr, '|');
            getline(ss, genre, '|');
            getline(ss, playCountStr, '|');
            getline(ss, likedStr, '|');

            for (int i = 0; i < multiplier; ++i) {
                int idx = rand() % genreCount;
                string newGenre = genreMap[idx].genre;
                string suffix = genreMap[idx].suffix;
                int newPlayCount = rand() % 501;
                int newLiked = rand() % 2;
                output << name << suffix << "|" << author << "|" << source << "|"
                    << durationStr << "|" << newGenre << "|"
                    << newPlayCount << "|" << newLiked << "\n";
            }
        }

        output.close();
        return true;
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
<<<<<<< HEAD
=======

>>>>>>> 0c6f778 (AvanceNew)
