#include "AppController.h"

namespace {
    constexpr int kVisibleLibraryRows = 17;
}


static string toLowerStr(string str) {
    for (char& c : str) c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    return str;
}

Song AppController::getLibrarySongAtVisibleIndex(int visibleIndex) {
    if (!durationSortActive) {
        return musicLib.getAllSongs()->getAt(visibleIndex);
    }

    vector<Song*> orderedSongs =
        durationSortAscending
        ? musicLib.getSongsSortedByDurationAscending()
        : musicLib.getSongsSortedByDurationDescending();

    if (visibleIndex < 0 || visibleIndex >= (int)orderedSongs.size() || orderedSongs[visibleIndex] == nullptr) {
        return Song();
    }

    return *(orderedSongs[visibleIndex]);
}

Song AppController::getLikedSongAtVisibleIndex(int visibleIndex) {
    vector<Song> likedSongs = musicLib.getLikedSongsVector();
    if (visibleIndex < 0 || visibleIndex >= static_cast<int>(likedSongs.size())) {
        return Song();
    }
    return likedSongs[visibleIndex];
}

Song AppController::getRecommendationSongAtVisibleIndex(int visibleIndex) {
    vector<RecommendationItem> recommendations = getSortedRecommendations();
    if (visibleIndex < 0 || visibleIndex >= static_cast<int>(recommendations.size())) {
        return Song();
    }
    return recommendations[visibleIndex].song;
}

vector<RecommendationItem> AppController::getSortedRecommendations() {
    vector<RecommendationItem> recommendations = musicLib.getRecommendedSongs();
    if (recommendationsSortActive && !recommendations.empty()) {
        QuickSort::quickSort(recommendations, 0, (int)recommendations.size() - 1, recommendationsSortAscending);
    }
    return recommendations;
}

AppController::AppController()
    : ui(100, 60),
    currentTab(Tab::LIBRARY),
    currentTabIndex(1),
    librarySelectedIndex(0),
    libraryTopIndex(0),
    playlistsSelectedIndex(0),
    playlistsTopIndex(0),
    queueSelectedIndex(0),
    queueTopIndex(0),
    insidePlaylist(false),
    openedPlaylistIndex(-1),
    playlistSongsSelectedIndex(0),
    playlistSongsTopIndex(0),
    likedSelectedIndex(0),
    likedTopIndex(0),
    recommendationsSelectedIndex(0),
    recommendationsTopIndex(0),
    durationSortActive(false),
    durationSortAscending(true),
    recommendationsSortActive(true),
    recommendationsSortAscending(false)
{
    FileManager::generateDataset(5);
    FileManager::loadSongs(musicLib);
    FileManager::loadPlaylists(musicLib);
    ui.displayMenu(musicLib, librarySelectedIndex, libraryTopIndex, false);
    musicLib.createDailyPlaylist();
}

AppController::~AppController() {
    FileManager::saveSongs(musicLib);
    FileManager::savePlaylists(musicLib);
}

void AppController::renderRefresh() {
    switch (currentTab) {
    case Tab::LIBRARY:
        ui.refreshLibraryRows(musicLib, librarySelectedIndex, libraryTopIndex, durationSortActive, durationSortAscending);
        break;
    case Tab::PLAYLISTS:
        if (insidePlaylist) {
            Playlist* playlist = musicLib.getPlaylist(openedPlaylistIndex);
            ui.refreshPlaylistSongsRows(playlist, playlistSongsSelectedIndex, playlistSongsTopIndex);
        }
        else
        {
            ui.refreshPlaylistRows(musicLib, playlistsSelectedIndex, playlistsTopIndex);
        }
        break;
    case Tab::LIKED:
        ui.refreshLikedRows(musicLib, likedSelectedIndex, likedTopIndex);
        break;
    case Tab::RECOMMENDATIONS:
        ui.refreshRecommendationsRows(musicLib, recommendationsSelectedIndex, recommendationsTopIndex, recommendationsSortActive, recommendationsSortAscending);
        break;
    case Tab::SEARCH:
        ui.refreshSearchRows(searchResults, searchSelectedIndex, searchTopIndex);
        break;
    case Tab::QUEUE:
        ui.refreshQueueRows(*musicLib.getSessionHistory(), queueSelectedIndex, queueTopIndex);
        break;
    default:
        ui.refreshLibraryRows(musicLib, librarySelectedIndex, libraryTopIndex, durationSortActive, durationSortAscending);
        break;
    }
}

void AppController::renderSwap() {
    switch (currentTab) {
    case Tab::LIBRARY:
        ui.displayLibrary(musicLib, librarySelectedIndex, libraryTopIndex, durationSortActive, durationSortAscending);
        break;
    case Tab::PLAYLISTS:
        if (insidePlaylist) {
            Playlist* playlist = musicLib.getPlaylist(openedPlaylistIndex);
            ui.displayPlaylistSongs(playlist, playlistSongsSelectedIndex, playlistSongsTopIndex);
        }
        else
        {
            ui.displayPlaylists(musicLib, playlistsSelectedIndex, playlistsTopIndex);
        }
        break;
    case Tab::LIKED:
        ui.displayLiked(musicLib, likedSelectedIndex, likedTopIndex);
        break;
    case Tab::RECOMMENDATIONS:
        ui.displayRecommendations(musicLib, recommendationsSelectedIndex, recommendationsTopIndex, recommendationsSortActive, recommendationsSortAscending);
        break;
    case Tab::QUEUE:
        ui.displayQueue(musicLib, queueSelectedIndex, queueTopIndex);
        break;
    case Tab::SEARCH:
        ui.displaySearchWithResults(searchResults, searchSelectedIndex, searchTopIndex, searchQuery);
        break;
    }
}

void AppController::render(string type) {
    if (type == "refresh") {
        renderRefresh();
    }
    else if (type == "swap") {
        renderSwap();
    }
}

void AppController::moveDown() {
    if (currentTab == Tab::LIBRARY) {
        const int totalSongs = static_cast<int>(musicLib.getAllSongs()->getSize());
        if (totalSongs == 0) return;

        if (librarySelectedIndex < totalSongs - 1) {
            librarySelectedIndex++;
            if (librarySelectedIndex >= libraryTopIndex + kVisibleLibraryRows) {
                libraryTopIndex++;
            }
        }
    }
    else if (currentTab == Tab::PLAYLISTS && !insidePlaylist) {
        const int totalPlaylists = musicLib.getPlaylistCount();
        if (totalPlaylists == 0) return;
        if (playlistsSelectedIndex < totalPlaylists - 1) {
            playlistsSelectedIndex++;
            if (playlistsSelectedIndex >= playlistsTopIndex + kVisibleLibraryRows) {
                playlistsTopIndex++;
            }
        }
    }
    else if (currentTab == Tab::PLAYLISTS && insidePlaylist) {

        Playlist* playlist =
            musicLib.getPlaylist(openedPlaylistIndex);

        if (playlist == nullptr) return;

        int totalSongs = playlist->getSize();

        if (totalSongs == 0) return;

        if (playlistSongsSelectedIndex < totalSongs - 1) {

            playlistSongsSelectedIndex++;

            if (
                playlistSongsSelectedIndex >=
                playlistSongsTopIndex + kVisibleLibraryRows
                ) {
                playlistSongsTopIndex++;
            }
        }
    }
    else if (currentTab == Tab::LIKED) {
        const int totalSongs = static_cast<int>(musicLib.getLikedSongsVector().size());
        if (totalSongs == 0) return;

        if (likedSelectedIndex < totalSongs - 1) {
            likedSelectedIndex++;
            if (likedSelectedIndex >= likedTopIndex + kVisibleLibraryRows) {
                likedTopIndex++;
            }
        }
    }
    else if (currentTab == Tab::RECOMMENDATIONS) {
        const int totalSongs = static_cast<int>(musicLib.getRecommendedSongs().size());
        if (totalSongs == 0) return;

        if (recommendationsSelectedIndex < totalSongs - 1) {
            recommendationsSelectedIndex++;
            if (recommendationsSelectedIndex >= recommendationsTopIndex + kVisibleLibraryRows) {
                recommendationsTopIndex++;
            }
        }
    }
}

void AppController::moveUp() {

    if (currentTab == Tab::LIBRARY) {

        if (librarySelectedIndex > 0) {

            librarySelectedIndex--;

            if (librarySelectedIndex < libraryTopIndex) {
                libraryTopIndex--;
            }
        }
    }

    else if (currentTab == Tab::PLAYLISTS && !insidePlaylist) {

        if (playlistsSelectedIndex > 0) {

            playlistsSelectedIndex--;

            if (playlistsSelectedIndex < playlistsTopIndex) {
                playlistsTopIndex--;
            }
        }
    }

    else if (currentTab == Tab::PLAYLISTS && insidePlaylist) {

        if (playlistSongsSelectedIndex > 0) {

            playlistSongsSelectedIndex--;

            if (playlistSongsSelectedIndex < playlistSongsTopIndex) {
                playlistSongsTopIndex--;
            }
        }
    }
    else if (currentTab == Tab::LIKED) {
        if (likedSelectedIndex > 0) {
            likedSelectedIndex--;
            if (likedSelectedIndex < likedTopIndex) {
                likedTopIndex--;
            }
        }
    }
    else if (currentTab == Tab::RECOMMENDATIONS) {
        if (recommendationsSelectedIndex > 0) {
            recommendationsSelectedIndex--;
            if (recommendationsSelectedIndex < recommendationsTopIndex) {
                recommendationsTopIndex--;
            }
        }
    }
}

void AppController::moveDownSearch() {
    if (searchResults.empty()) return;
    if (searchSelectedIndex < static_cast<int>(searchResults.size()) - 1) {
        searchSelectedIndex++;
        if (searchSelectedIndex >= searchTopIndex + kVisibleLibraryRows) {
            searchTopIndex++;
        }
    }
}

void AppController::moveUpSearch() {
    if (searchResults.empty()) return;
    if (searchSelectedIndex > 0) {
        searchSelectedIndex--;
        if (searchSelectedIndex < searchTopIndex) {
            searchTopIndex--;
        }
    }
}

/*
Importancia:
Motor de busqueda interactiva que se ejecuta cada vez que el usuario escribe o
borra un caracter en la pestana Search. Es la unica via para localizar canciones
por nombre o artista cuando la biblioteca es grande. Su respuesta en vivo
mejora significativamente la experiencia de usuario.

BigO: O(K + N) en el peor caso
Donde K es la longitud del query y N es la cantidad de canciones en la biblioteca.
`searchByNamePrefix()` utiliza el arbol rojo-negro `indexByName` y su operacion
`prefixSearch()` que recorre el arbol encontrando nodos que coincidan con el
prefijo. Si este metodo no encuentra resultados, se recurre a `searchIncremental()`
que recorre toda la lista enlazada (O(N)) aplicando `string::find` en cada
cancion (O(K x M) por cancion donde M es la longitud del nombre/artista).
En el peor escenario (sin prefijo y query larga) se acerca a O(N x K x M).
Espacio O(N) para almacenar el vector de resultados.

Explicacion detallada:
1. Limpia el vector `searchResults` de la busqueda anterior.
2. Evalua la longitud del query:
   - Si esta vacio: devuelve todas las canciones de la biblioteca.
   - Si tiene 3 o mas caracteres: intenta primero `searchByNamePrefix()`
     que busca en el arbol rojo-negro `indexByName` usando el prefijo del
     query. Esto es eficiente porque el arbol mantiene las claves ordenadas
     y puede encontrar el rango de nodos que comienzan con el prefijo sin
     recorrer toda la estructura. Si encuentra al menos un resultado, lo usa.
     Si no encuentra ninguno, cae en `searchIncremental()` que recorre la
     lista enlazada completa y verifica si el query es substring del nombre
     o del artista (en minusculas).
   - Si tiene 1 o 2 caracteres: directamente usa `searchIncremental()`.
3. Reinicia los indices de seleccion y scroll (`searchSelectedIndex = 0`,
   `searchTopIndex = 0`) para que la interfaz muestre desde el primer resultado.
4. El vector de resultados se mantiene en memoria en `AppController` y se
   actualiza con cada tecla, permitiendo que la interfaz lo renderice sin
   recalcular la busqueda.
*/
void AppController::performSearch() {
    searchResults.clear();

    if (searchQuery.empty()) {
        searchResults = musicLib.getAllSongsVector();
    }
    else if (searchQuery.length() >= 3) {
        // Intentar prefijo primero
        vector<Song> prefixResults = musicLib.searchByNamePrefix(searchQuery);
        if (!prefixResults.empty()) {
            searchResults = prefixResults;
        }
        else {
            searchResults = musicLib.searchIncremental(searchQuery);
        }
    }
    else {
        searchResults = musicLib.searchIncremental(searchQuery);
    }

    searchSelectedIndex = 0;
    searchTopIndex = 0;
}

void AppController::playSelectedSearchSong() {
    if (currentTab != Tab::SEARCH || searchResults.empty()) return;
    Song& selectedSong = searchResults[searchSelectedIndex];
    if (audio.getActual() != selectedSong.getSource()) {
        audio.cerrar();
        if (audio.cargar(selectedSong.getSource())) {
            audio.reproducir();
        }
    }
    else {
        audio.reproducir();
    }
    musicLib.addToSessionHistory(selectedSong);
    ui.refreshHudSong(selectedSong.getName(), selectedSong.getAuthor());
}

void AppController::moveDownQueue() {
    Stack<Song>* history = musicLib.getSessionHistory();
    int totalSongs = static_cast<int>(history->size());
    if (totalSongs == 0) return;
    queueSelectedIndex++;
    if (queueSelectedIndex >= queueTopIndex + kVisibleLibraryRows) {
        queueTopIndex++;
    }
    if (queueSelectedIndex >= totalSongs) {
        queueSelectedIndex = 0;
    }
    
}

void AppController::moveUpQueue() {
    if (queueSelectedIndex > 0) {
        queueSelectedIndex--;
        if (queueSelectedIndex < queueTopIndex) {
            queueTopIndex--;
        }
    }
}

void AppController::playSelectedQueueSong() {
    if (currentTab != Tab::QUEUE) return;
    Stack<Song>* history = musicLib.getSessionHistory();
    if (history->isEmpty()) return;
    if (queueSelectedIndex >= static_cast<int>(history->size())) return;

    Song selectedSong = history->getAt(queueSelectedIndex);
    if (audio.getActual() != selectedSong.getSource()) {
        audio.cerrar();
        if (audio.cargar(selectedSong.getSource())) {
            audio.reproducir();
        }
    }
    else {
        audio.reproducir();
    }
    musicLib.addToSessionHistory(selectedSong);
    ui.refreshHudSong(selectedSong.getName(), selectedSong.getAuthor());
}

/*
Importancia:
Es el centro de comando de toda la interaccion usuario-sistema. Procesa cada tecla
presionada y decide que accion ejecutar: navegar entre pestanas, reproducir canciones,
pausar, dar like, gestionar playlists, buscar, etc. Sin esta funcion el usuario no
podria controlar el reproductor.

BigO: O(1) en el caso promedio, O(N) en el peor caso
La mayoria de las teclas (flechas, Enter, Espacio, Esc, +, -, L) se atienden en
tiempo constante O(1). Sin embargo, cuando el usuario escribe en la pestana de
busqueda, cada tecla dispara `performSearch()` que recorre toda la biblioteca
con costo O(N). Las teclas que guardan playlists tambien arrastran O(P) donde
P es la cantidad de playlists a serializar. El peor caso global se da durante
busqueda activa + guardado, dando O(N + P).
Espacio O(1) adicional, solo utiliza variables locales de tipo entero o puntero.

Explicacion detallada:
1. Verifica con `_kbhit()` si hay una tecla disponible en el buffer de entrada.
   Si no la hay, retorna inmediatamente para no bloquear el bucle principal.
2. Lee la tecla con `_getch()` y la evalua mediante una serie de condicionales:
   a) '+' -> Llama a `addSelectedSongToPlaylist()` para agregar la cancion actual
           a una playlist, creandola si no existe.
   b) '-' -> Llama a `removeSelectedSongFromPlaylist()` para quitar la cancion
             de una playlist especificada.
   c) Esc (27) -> Si esta dentro de una playlist, retrocede a la vista de listas.
                  Si ya esta en el nivel superior, guarda las playlists y cierra
                  la aplicacion con `exit(0)`.
   d) Enter (13) -> Segun la pestana activa, reproduce la cancion seleccionada,
                    entra a una playlist o ejecuta la busqueda.
   e) Espacio (32) -> Alterna entre pausa y reproduccion del audio actual.
   f) 'L' (108) -> Da o quita like a la cancion seleccionada en cualquier vista.
   g) 'A'/'S'/'D' -> Ordenan la biblioteca por duracion ascendente/descendente/desactivado.
   h) 'R'/'T' -> Ordenan las recomendaciones por score ascendente/descendente.
   i) 'O'/'P' -> Navegan entre playlists anterior/siguiente.
3. Si la pestana activa es SEARCH, las teclas imprimibles se acumulan en
   `searchQuery`, y cada cambio ejecuta `performSearch()` para actualizar
   resultados en vivo. Backspace elimina el ultimo caracter.
4. Las flechas arriba/abajo (224 seguido de 72/80) llaman a `moveUp()`/`moveDown()`
   segun la pestana activa, actualizando el indice seleccionado y el scroll.
5. Despues de cada accion que modifica la seleccion o los datos, llama a
   `render("refresh")` para actualizar solo las filas que cambiaron
   (renderizado parcial), o `render("swap")` para redibujar la pantalla completa
   (cambio de pestana).
*/
void AppController::handleInput() {
    if (!_kbhit()) return;

    int key = _getch();
    if (key == '+') {
        addSelectedSongToPlaylist();
        return;
    }
    if (key == '-') {
        removeSelectedSongFromPlaylist();
        return;
    }
    if (key == 27) {
        if (insidePlaylist) {
            insidePlaylist = false;
            openedPlaylistIndex = -1;
            playlistSongsSelectedIndex = 0;
            playlistSongsTopIndex = 0;
            render("swap");
            return;
        }

        FileManager::savePlaylists(musicLib);
        exit(0);
    }
    if (key == 13) {
        switch (currentTab) {
        case Tab::LIBRARY:
            playSelectedSong();
            break;
        case Tab::PLAYLISTS:
            if (!insidePlaylist) {
                insidePlaylist = true;
                openedPlaylistIndex = playlistsSelectedIndex;
                playlistSongsSelectedIndex = 0;
                playlistSongsTopIndex = 0;

                Playlist* playlist = musicLib.getPlaylist(openedPlaylistIndex);
                ui.displayPlaylistSongs(
                    playlist,
                    playlistSongsSelectedIndex,
                    playlistSongsTopIndex
                );
            }
            else {
                playSelectedPlaylistSong();
            }
            break;
        case Tab::LIKED:
            playSelectedLikedSong();
            break;
        case Tab::RECOMMENDATIONS:
            playSelectedRecommendationSong();
            break;
        case Tab::SEARCH:
            if (!searchResults.empty()) {
                playSelectedSearchSong();
            }
            break;
        case Tab::QUEUE:
            playSelectedQueueSong();
            break;
        }

        return;
    }
    else if (key == 32) {
        if (!audio.estaPausado()) audio.pausar();
        else audio.reproducir();
        return;
    }

    if (currentTab == Tab::SEARCH) {
        if (key == 8) {
            if (!searchQuery.empty()) {
                searchQuery.pop_back();
                performSearch();
                render("swap");
            }
            return;
        }
        if (key >= 32 && key <= 126) {
            searchQuery += static_cast<char>(key);
            performSearch();
            render("swap");
            return;
        }
    }

    if (currentTab == Tab::LIBRARY) {
        if (key == 'a' || key == 'A') {
            durationSortActive = true;
            durationSortAscending = true;
            render("swap");
            return;
        }

        if (key == 's' || key == 'S') {
            durationSortActive = true;
            durationSortAscending = false;
            render("swap");
            return;
        }

        if (key == 'd' || key == 'D') {
            durationSortActive = false;
            render("swap");
            return;
        }
    }

    if (currentTab == Tab::RECOMMENDATIONS) {
        if (key == 'r' || key == 'R') {
            recommendationsSortActive = true;
            recommendationsSortAscending = false;
            render("swap");
            return;
        }

        if (key == 't' || key == 'T') {
            recommendationsSortActive = true;
            recommendationsSortAscending = true;
            render("swap");
            return;
        }
    }

    if (currentTab == Tab::PLAYLISTS && !insidePlaylist) {
        if (key == 'o' || key == 'O') {
            musicLib.sortPlaylistsBySongCount(false);
            FileManager::savePlaylists(musicLib);
            render("swap");
            return;
        }

        if (key == 'p' || key == 'P') {
            musicLib.sortPlaylistsBySongCount(true);
            FileManager::savePlaylists(musicLib);
            render("swap");
            return;
        }
    }

    if (currentTab != Tab::SEARCH && (key == 'l' || key == 'L')) {
        toggleLikedSelectedSong();
        return;
    }

    if (key != 224) return;
    key = _getch();

    auto switchTab = [this](int nextTabIndex) {
        if (insidePlaylist) {
            insidePlaylist = false;
            openedPlaylistIndex = -1;
            playlistSongsSelectedIndex = 0;
            playlistSongsTopIndex = 0;
        }

        currentTabIndex = nextTabIndex;
        currentTab = static_cast<Tab>(nextTabIndex);
        render("swap");
    };

    switch (key) {
    case 75: {
        int nextTab = currentTabIndex - 1;
        if (nextTab < 1) nextTab = 6;
        switchTab(nextTab);
        break;
    }
    case 77: {
        int nextTab = currentTabIndex + 1;
        if (nextTab > 6) nextTab = 1;
        switchTab(nextTab);
        break;
    }
    case 63:
        durationSortActive = true;
        durationSortAscending = true;
        render("swap");
        break;
    case 64:
        durationSortActive = true;
        durationSortAscending = false;
        render("swap");
        break;
    case 80: {
        if (currentTab == Tab::LIBRARY) {
            int previousSelectedIndex = librarySelectedIndex;
            int previousTopIndex = libraryTopIndex;

            moveDown();

            if (durationSortActive) {
                render("swap");
            }
            else if (libraryTopIndex != previousTopIndex) {
                render("refresh");
            }
            else if (librarySelectedIndex != previousSelectedIndex) {
                ui.refreshLibrarySelection(
                    musicLib,
                    previousSelectedIndex,
                    librarySelectedIndex,
                    libraryTopIndex
                );
            }
        }
        else if (currentTab == Tab::PLAYLISTS && !insidePlaylist) {
            int previousSelectedIndex = playlistsSelectedIndex;
            int previousTopIndex = playlistsTopIndex;

            moveDown();

            if (playlistsTopIndex != previousTopIndex) {
                render("refresh");
            }
            else if (playlistsSelectedIndex != previousSelectedIndex) {
                ui.refreshPlaylistsSelection(
                    musicLib,
                    previousSelectedIndex,
                    playlistsSelectedIndex,
                    playlistsTopIndex
                );
            }
        }
        else if (currentTab == Tab::PLAYLISTS && insidePlaylist) {
            int previousSelectedIndex = playlistSongsSelectedIndex;
            int previousTopIndex = playlistSongsTopIndex;

            moveDown();

            Playlist* playlist = musicLib.getPlaylist(openedPlaylistIndex);

            if (playlistSongsTopIndex != previousTopIndex) {
                render("refresh");
            }
            else if (playlistSongsSelectedIndex != previousSelectedIndex) {
                ui.refreshPlaylistSongsSelection(
                    playlist,
                    previousSelectedIndex,
                    playlistSongsSelectedIndex,
                    playlistSongsTopIndex
                );
            }
        }
        else if (currentTab == Tab::LIKED) {
            int previousSelectedIndex = likedSelectedIndex;
            int previousTopIndex = likedTopIndex;

            moveDown();

            if (likedTopIndex != previousTopIndex) {
                render("refresh");
            }
            else if (likedSelectedIndex != previousSelectedIndex) {
                ui.refreshLikedRows(musicLib, likedSelectedIndex, likedTopIndex);
            }
        }
        else if (currentTab == Tab::RECOMMENDATIONS) {
            int previousSelectedIndex = recommendationsSelectedIndex;
            int previousTopIndex = recommendationsTopIndex;

            moveDown();

            if (recommendationsTopIndex != previousTopIndex) {
                render("refresh");
            }
            else if (recommendationsSelectedIndex != previousSelectedIndex) {
                ui.refreshRecommendationsRows(musicLib, recommendationsSelectedIndex, recommendationsTopIndex, recommendationsSortActive, recommendationsSortAscending);
            }
        }
        else if (currentTab == Tab::SEARCH) {
            int previousSelectedIndex = searchSelectedIndex;
            int previousTopIndex = searchTopIndex;
            moveDownSearch();
            if (searchTopIndex != previousTopIndex) {
                render("refresh");
            }
            else if (searchSelectedIndex != previousSelectedIndex) {
                ui.refreshSearchSelection(searchResults, previousSelectedIndex, searchSelectedIndex, searchTopIndex);
            }
        }
        else if (currentTab == Tab::QUEUE) {
            int previousSelectedIndex = queueSelectedIndex;
            int previousTopIndex = queueTopIndex;
            moveDownQueue();
            if (queueTopIndex != previousTopIndex) {
                render("refresh");
            }
            else if (queueSelectedIndex != previousSelectedIndex) {
                ui.refreshQueueSelection(*musicLib.getSessionHistory(), previousSelectedIndex, queueSelectedIndex, queueTopIndex);
            }
        }
        break;
    }
    case 72: {
        if (currentTab == Tab::LIBRARY) {
            int previousSelectedIndex = librarySelectedIndex;
            int previousTopIndex = libraryTopIndex;

            moveUp();

            if (durationSortActive) {
                render("swap");
            }
            else if (libraryTopIndex != previousTopIndex) {
                render("refresh");
            }
            else if (librarySelectedIndex != previousSelectedIndex) {
                ui.refreshLibrarySelection(
                    musicLib,
                    previousSelectedIndex,
                    librarySelectedIndex,
                    libraryTopIndex
                );
            }
        }
        else if (currentTab == Tab::PLAYLISTS && !insidePlaylist) {
            int previousSelectedIndex = playlistsSelectedIndex;
            int previousTopIndex = playlistsTopIndex;

            moveUp();

            if (playlistsTopIndex != previousTopIndex) {
                render("refresh");
            }
            else if (playlistsSelectedIndex != previousSelectedIndex) {
                ui.refreshPlaylistsSelection(
                    musicLib,
                    previousSelectedIndex,
                    playlistsSelectedIndex,
                    playlistsTopIndex
                );
            }
        }
        else if (currentTab == Tab::PLAYLISTS && insidePlaylist) {
            int previousSelectedIndex = playlistSongsSelectedIndex;
            int previousTopIndex = playlistSongsTopIndex;

            moveUp();

            Playlist* playlist = musicLib.getPlaylist(openedPlaylistIndex);

            if (playlistSongsTopIndex != previousTopIndex) {
                render("refresh");
            }
            else if (playlistSongsSelectedIndex != previousSelectedIndex) {
                ui.refreshPlaylistSongsSelection(
                    playlist,
                    previousSelectedIndex,
                    playlistSongsSelectedIndex,
                    playlistSongsTopIndex
                );
            }
        }
        else if (currentTab == Tab::LIKED) {
            int previousSelectedIndex = likedSelectedIndex;
            int previousTopIndex = likedTopIndex;

            moveUp();

            if (likedTopIndex != previousTopIndex) {
                render("refresh");
            }
            else if (likedSelectedIndex != previousSelectedIndex) {
                ui.refreshLikedRows(musicLib, likedSelectedIndex, likedTopIndex);
            }
        }
        else if (currentTab == Tab::RECOMMENDATIONS) {
            int previousSelectedIndex = recommendationsSelectedIndex;
            int previousTopIndex = recommendationsTopIndex;

            moveUp();

            if (recommendationsTopIndex != previousTopIndex) {
                render("refresh");
            }
            else if (recommendationsSelectedIndex != previousSelectedIndex) {
                ui.refreshRecommendationsRows(musicLib, recommendationsSelectedIndex, recommendationsTopIndex, recommendationsSortActive, recommendationsSortAscending);
            }
        }
        else if (currentTab == Tab::SEARCH) {
            int previousSelectedIndex = searchSelectedIndex;
            int previousTopIndex = searchTopIndex;
            moveUpSearch();
            if (searchTopIndex != previousTopIndex) {
                render("refresh");
            }
            else if (searchSelectedIndex != previousSelectedIndex) {
                ui.refreshSearchSelection(searchResults, previousSelectedIndex, searchSelectedIndex, searchTopIndex);
            }
        }
        else if (currentTab == Tab::QUEUE) {
            int previousSelectedIndex = queueSelectedIndex;
            int previousTopIndex = queueTopIndex;
            moveUpQueue();
            if (queueTopIndex != previousTopIndex) {
                render("refresh");
            }
            else if (queueSelectedIndex != previousSelectedIndex) {
                ui.refreshQueueSelection(*musicLib.getSessionHistory(), previousSelectedIndex, queueSelectedIndex, queueTopIndex);
            }
        }
        break;
    }
    default:
        break;
    }
}

void AppController::playSelectedLikedSong() {
    if (currentTab != Tab::LIKED) return;

    Song selectedSong = getLikedSongAtVisibleIndex(likedSelectedIndex);
    if (selectedSong.getSource().empty()) return;

    if (audio.getActual() != selectedSong.getSource()) {
        audio.cerrar();
        if (audio.cargar(selectedSong.getSource())) {
            audio.reproducir();
        }
    }
    else {
        audio.reproducir();
    }

    musicLib.addToSessionHistory(selectedSong);
    ui.refreshHudSong(selectedSong.getName(), selectedSong.getAuthor());
}

void AppController::playSelectedRecommendationSong() {
    if (currentTab != Tab::RECOMMENDATIONS) return;

    Song selectedSong = getRecommendationSongAtVisibleIndex(recommendationsSelectedIndex);
    if (selectedSong.getSource().empty()) return;

    if (audio.getActual() != selectedSong.getSource()) {
        audio.cerrar();
        if (audio.cargar(selectedSong.getSource())) {
            audio.reproducir();
        }
    }
    else {
        audio.reproducir();
    }

    musicLib.addToSessionHistory(selectedSong);
    ui.refreshHudSong(selectedSong.getName(), selectedSong.getAuthor());
}

void AppController::toggleLikedSelectedSong() {
    Song selectedSong;
    bool hasSong = true;

    switch (currentTab) {
    case Tab::LIBRARY:
        selectedSong = getLibrarySongAtVisibleIndex(librarySelectedIndex);
        break;
    case Tab::PLAYLISTS:
        if (insidePlaylist) {
            Playlist* playlist = musicLib.getPlaylist(openedPlaylistIndex);
            if (playlist == nullptr) return;
            selectedSong = playlist->getSongAt(playlistSongsSelectedIndex);
        }
        else {
            hasSong = false;
        }
        break;
    case Tab::LIKED:
        selectedSong = getLikedSongAtVisibleIndex(likedSelectedIndex);
        break;
    case Tab::RECOMMENDATIONS:
        selectedSong = getRecommendationSongAtVisibleIndex(recommendationsSelectedIndex);
        break;
    case Tab::QUEUE:
        if (!musicLib.getSessionHistory()->isEmpty()) {
            selectedSong = musicLib.getSessionHistory()->getAt(queueSelectedIndex);
        }
        else {
            hasSong = false;
        }
        break;
    default:
        hasSong = false;
        break;
    }

    if (!hasSong || selectedSong.getSource().empty()) return;

    bool wasLiked = selectedSong.isLiked();

    if (!musicLib.toggleLikedBySource(selectedSong.getSource())) return;

    if (currentTab == Tab::LIKED) {
        int totalSongs = static_cast<int>(musicLib.getLikedSongsVector().size());
        if (totalSongs == 0) {
            likedSelectedIndex = 0;
            likedTopIndex = 0;
        }
        else {
            if (likedSelectedIndex >= totalSongs) likedSelectedIndex = totalSongs - 1;
            if (likedTopIndex > likedSelectedIndex) likedTopIndex = likedSelectedIndex;
        }
    }

    if (currentTab == Tab::RECOMMENDATIONS) {
        int totalSongs = static_cast<int>(musicLib.getRecommendedSongs().size());
        if (totalSongs == 0) {
            recommendationsSelectedIndex = 0;
            recommendationsTopIndex = 0;
        }
        else {
            if (recommendationsSelectedIndex >= totalSongs) recommendationsSelectedIndex = totalSongs - 1;
            if (recommendationsTopIndex > recommendationsSelectedIndex) recommendationsTopIndex = recommendationsSelectedIndex;
        }
    }

    render("swap");

    ui.displayConsole();
    if (wasLiked) {
        ui.writeAt(4, 53, "Quitada de Liked.", 255, 180, 120);
    }
    else {
        ui.writeAt(4, 53, "Agregada a Liked.", 120, 255, 120);
    }
}

void AppController::playSelectedSong() {
    if (currentTab != Tab::LIBRARY) return;

    Song selectedSong = getLibrarySongAtVisibleIndex(librarySelectedIndex);

    if (audio.getActual() != selectedSong.getSource()) {
        audio.cerrar();
        if (audio.cargar(selectedSong.getSource())) {
            audio.reproducir();
        }
    }
    else {
        audio.reproducir();
    }

    musicLib.addToSessionHistory(selectedSong);
    ui.refreshHudSong(selectedSong.getName(), selectedSong.getAuthor());
}

void AppController::playSelectedPlaylistSong() {

    if (!insidePlaylist) return;

    Playlist* playlist =
        musicLib.getPlaylist(openedPlaylistIndex);

    if (playlist == nullptr) return;

    Song selectedSong =
        playlist->getSongAt(playlistSongsSelectedIndex);

    if (audio.getActual() != selectedSong.getSource()) {

        audio.cerrar();

        if (audio.cargar(selectedSong.getSource())) {
            audio.reproducir();
        }
    }
    else {
        audio.reproducir();
    }
    musicLib.addToSessionHistory(selectedSong);
}

string AppController::captureTextInput(const string& prompt) {
    string input = "";

    ui.displayConsole();
    ui.writeAt(
        4,
        53,
        prompt,
        255, 255, 255
    );

    while (true) {

        int key = _getch();

        if (key == 13) { // Enter
            break;
        }

        if (key == 8) { // Backspace
            if (!input.empty()) {
                input.pop_back();

                ui.fillRect(
                    4 + prompt.size(),
                    53,
                    40,
                    1,
                    ' ',
                    255, 255, 255,
                    22, 24, 37
                );

                ui.writeAt(
                    4 + prompt.size(),
                    53,
                    input,
                    255, 255, 255
                );
            }
            continue;
        }

        if (isprint(key)) {
            input += static_cast<char>(key);

            ui.writeAt(
                4 + prompt.size(),
                53,
                input,
                255, 255, 255
            );
        }
    }

    ui.displayConsole();
    ui.displayHelp();

    return input;
}

void AppController::addSelectedSongToPlaylist() {
    if (currentTab != Tab::LIBRARY)
        return;

    string playlistName =
        captureTextInput("Nombre playlist: ");

    if (playlistName.empty())
        return;

    Song song = getLibrarySongAtVisibleIndex(librarySelectedIndex);

    Playlist* playlist = nullptr;

    for (int i = 0; i < musicLib.getPlaylistCount(); i++) {
        Playlist* p = musicLib.getPlaylist(i);

        if (p != nullptr && p->getName() == playlistName) {
            playlist = p;
            break;
        }
    }

    if (playlist == nullptr) {
        musicLib.addPlaylist(playlistName);
        playlist = musicLib.getPlaylist(musicLib.getPlaylistCount() - 1);
    }

    if (playlist != nullptr) {
        playlist->addSong(song);
    }

    FileManager::savePlaylists(musicLib);

    ui.displayConsole();
    ui.writeAt(4, 53, "Cancion agregada correctamente.", 100, 255, 100);
}

void AppController::removeSelectedSongFromPlaylist() {
    if (currentTab != Tab::LIBRARY)
        return;

    string playlistName =
        captureTextInput("Eliminar de playlist: ");

    if (playlistName.empty())
        return;

    Playlist* playlist = nullptr;

    for (int i = 0; i < musicLib.getPlaylistCount(); i++) {
        Playlist* p = musicLib.getPlaylist(i);

        if (p != nullptr && p->getName() == playlistName) {
            playlist = p;
            break;
        }
    }

    if (playlist == nullptr) {
        ui.displayConsole();
        ui.writeAt(4, 53, "La playlist no existe.", 255, 120, 120);
        return;
    }

    Song selectedSong = getLibrarySongAtVisibleIndex(librarySelectedIndex);

    bool found = false;

    for (int i = 0; i < playlist->getSize(); i++) {
        Song song = playlist->getSongAt(i);

        if (
            song.getName() == selectedSong.getName() &&
            song.getAuthor() == selectedSong.getAuthor() &&
            song.getSource() == selectedSong.getSource()
            ) {
            playlist->removeSong(i);

            if (playlist->getSize() == 0) {
                for (int j = 0; j < musicLib.getPlaylistCount(); j++) {
                    Playlist* current = musicLib.getPlaylist(j);

                    if (current == playlist) {
                        musicLib.removePlaylist(j);
                        break;
                    }
                }
            }

            found = true;
            break;
        }
    }

    if (found) {
        FileManager::savePlaylists(musicLib);

        ui.displayConsole();
        ui.writeAt(4, 53, "Cancion eliminada correctamente.", 100, 255, 100);
    }
    else {
        ui.displayConsole();
        ui.writeAt(4, 53, "La cancion no pertenece a la playlist.", 255, 120, 120);
    }
}

/*
Importancia:
Contiene el game-loop (bucle principal) de la aplicacion. Es el corazon del programa
que se ejecuta ininterrumpidamente desde que inicia hasta que el usuario presiona
Esc para salir. Sin este bucle la aplicacion se cerraria inmediatamente.

BigO: O(1) por iteracion
Cada ciclo ejecuta dos operaciones de costo constante: `handleInput()` que en el
caso promedio responde en O(1) (solo leer una tecla), y `drawSpectrum()` que
redibuja una seccion fija de la pantalla con 21 barras de 6 pixeles cada una,
tambien O(1) por tener un tamano constante. El bucle es infinito, por lo que la
complejidad total es O(infty) hasta que se fuerza la salida con exit().
Espacio O(1) adicional.

Explicacion detallada:
1. Itera perpetuamente mientras la aplicacion este activa.
2. En cada iteracion llama a `handleInput()`, que verifica si hay una tecla
   disponible con `_kbhit()`. Si no la hay, retorna inmediatamente sin bloqueo,
   permitiendo que el ciclo continue fluidamente.
3. Despues ejecuta `drawSpectrum()` que dibuja un visualizador de espectro
   animado en la parte inferior de la pantalla usando ondas seno.
   El parametro `!audio.estaPausado()` controla si la animacion avanza o se congela.
4. El unico modo de salir del bucle es cuando `handleInput()` captura la tecla
   Esc (codigo 27), que invoca `exit(0)` para finalizar el proceso.
*/
void AppController::run() {
    while (true) {
        handleInput(); // Maneja la entrada del usuario en cada iteracion del loop principal.
        ui.drawSpectrum(130, 50, !audio.estaPausado()); // Dibuja el espectro en la parte inferior de la pantalla.
    }
}



