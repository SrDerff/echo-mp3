#include "AppController.h"

namespace {
    constexpr int kVisibleLibraryRows = 17;
}


static string toLowerStr(string str) {
    for (char& c : str) c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    return str;
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
    playlistSongsTopIndex(0)
{
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
        ui.refreshLibraryRows(musicLib, librarySelectedIndex, libraryTopIndex);
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
        case Tab::SEARCH:
        ui.refreshSearchRows(searchResults, searchSelectedIndex, searchTopIndex);
        break;
    case Tab::QUEUE:
        ui.refreshQueueRows(*musicLib.getSessionHistory(), queueSelectedIndex, queueTopIndex);
        break;
    default:
        ui.refreshLibraryRows(musicLib, librarySelectedIndex, libraryTopIndex);
        break;
    }
}

void AppController::renderSwap() {
    switch (currentTab) {
    case Tab::LIBRARY:
        ui.displayLibrary(musicLib, librarySelectedIndex, libraryTopIndex);
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

void AppController::performSearch() {
    searchResults.clear();
    string lowerQuery = toLowerStr(searchQuery);

    Node<Song>* curr = musicLib.getAllSongs()->getHead();
    while (curr != nullptr) {
        Song song = curr->getValue(); // copia segura

        bool match = false;
        if (searchQuery.empty()) {
            match = true;
        }
        else {
            string lowerName = toLowerStr(song.getName());
            string lowerArtist = toLowerStr(song.getAuthor());
            if (lowerName.find(lowerQuery) != string::npos ||
                lowerArtist.find(lowerQuery) != string::npos) {
                match = true;
            }
        }

        if (match) {
            searchResults.push_back(song); // copia al vector
        }
        curr = curr->next;
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

        // salir de playlist interna
        if (insidePlaylist) {

            insidePlaylist = false;

            render("swap");

            return;
        }

        FileManager::savePlaylists(musicLib);

        exit(0);
    }
    if (key == 13) {

        if (currentTab == Tab::LIBRARY) {
            playSelectedSong();
        }

        else if (currentTab == Tab::PLAYLISTS) {

            // abrir playlist
            if (!insidePlaylist) {

                insidePlaylist = true;
                openedPlaylistIndex = playlistsSelectedIndex;

                playlistSongsSelectedIndex = 0;
                playlistSongsTopIndex = 0;

                Playlist* playlist =
                    musicLib.getPlaylist(openedPlaylistIndex);

                ui.displayPlaylistSongs(
                    playlist,
                    playlistSongsSelectedIndex,
                    playlistSongsTopIndex
                );
            }

            // reproducir canción dentro playlist
            else {
                playSelectedPlaylistSong();
            }
        }
        else if (currentTab == Tab::SEARCH && !searchResults.empty()) {
            playSelectedSearchSong();
        }
        else if (currentTab == Tab::QUEUE) {
            playSelectedQueueSong();
        }

        return;
    }
    else if (key == 32) { // Espacio
        if (!audio.estaPausado()) audio.pausar();
        else audio.reproducir();
        return;
    }

    if (currentTab == Tab::SEARCH) {
        if (key == 8) { // Backspace
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

    if (key != 224) return;
    key = _getch();

    switch (key) {
    case 75:
        if (currentTab == Tab::LIBRARY) return;
        if (currentTab == Tab::PLAYLISTS) {
            currentTab = Tab::LIBRARY;
            render("swap");
        }
        else if (currentTab == Tab::QUEUE) {
            currentTab = Tab::PLAYLISTS;
            render("swap");
        }
        else if (currentTab == Tab::SEARCH) {
            currentTab = Tab::QUEUE;
            render("swap");
        }
        break;
    case 77:
        if (currentTab == Tab::SEARCH) return;
        if (currentTab == Tab::LIBRARY) {
            currentTab = Tab::PLAYLISTS;
            render("swap");
        }
        else if (currentTab == Tab::PLAYLISTS) {
            currentTab = Tab::QUEUE;
            render("swap");
        }
        else if (currentTab == Tab::QUEUE) {
            currentTab = Tab::SEARCH;
            render("swap");
        }
        break;
    case 80: {

        if (currentTab == Tab::LIBRARY) {

            int previousSelectedIndex = librarySelectedIndex;
            int previousTopIndex = libraryTopIndex;

            moveDown();

            if (libraryTopIndex != previousTopIndex) {
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
            else if (
                playlistsSelectedIndex != previousSelectedIndex
                ) {

                ui.refreshPlaylistsSelection(
                    musicLib,
                    previousSelectedIndex,
                    playlistsSelectedIndex,
                    playlistsTopIndex
                );
            }
        }

        else if (currentTab == Tab::PLAYLISTS && insidePlaylist) {

            int previousSelectedIndex =
                playlistSongsSelectedIndex;

            int previousTopIndex =
                playlistSongsTopIndex;

            moveDown();

            Playlist* playlist =
                musicLib.getPlaylist(openedPlaylistIndex);

            if (
                playlistSongsTopIndex != previousTopIndex
                ) {

                render("refresh");
            }
            else if (
                playlistSongsSelectedIndex !=
                previousSelectedIndex
                ) {

                ui.refreshPlaylistSongsSelection(
                    playlist,
                    previousSelectedIndex,
                    playlistSongsSelectedIndex,
                    playlistSongsTopIndex
                );
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

            if (libraryTopIndex != previousTopIndex) {

                render("refresh");
            }
            else if (
                librarySelectedIndex != previousSelectedIndex
                ) {

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
            else if (
                playlistsSelectedIndex != previousSelectedIndex
                ) {

                ui.refreshPlaylistsSelection(
                    musicLib,
                    previousSelectedIndex,
                    playlistsSelectedIndex,
                    playlistsTopIndex
                );
            }
        }

        else if (currentTab == Tab::PLAYLISTS && insidePlaylist) {

            int previousSelectedIndex =
                playlistSongsSelectedIndex;

            int previousTopIndex =
                playlistSongsTopIndex;

            moveUp();

            Playlist* playlist =
                musicLib.getPlaylist(openedPlaylistIndex);

            if (
                playlistSongsTopIndex != previousTopIndex
                ) {

                render("refresh");
            }
            else if (
                playlistSongsSelectedIndex !=
                previousSelectedIndex
                ) {

                ui.refreshPlaylistSongsSelection(
                    playlist,
                    previousSelectedIndex,
                    playlistSongsSelectedIndex,
                    playlistSongsTopIndex
                );
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
        }
        break;
        break;
    }
    default:
        break;
    }
}

void AppController::playSelectedSong() {
    if (currentTab != Tab::LIBRARY) return;

    Song selectedSong = musicLib.getAllSongs()->getAt(librarySelectedIndex);

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
    ui.refreshHud(musicLib, librarySelectedIndex, libraryTopIndex);
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

    Song song =
        musicLib.getAllSongs()->getAt(
            librarySelectedIndex
        );

    Playlist* playlist = nullptr;

    // buscar playlist existente
    for (int i = 0; i < musicLib.getPlaylistCount(); i++) {

        Playlist* p =
            musicLib.getPlaylist(i);

        if (p != nullptr &&
            p->getName() == playlistName) {

            playlist = p;
            break;
        }
    }

    // no existe -> crear
    if (playlist == nullptr) {
        musicLib.addPlaylist(
            playlistName
        );

        playlist =
            musicLib.getPlaylist(
                musicLib.getPlaylistCount() - 1
            );
    }

    if (playlist != nullptr) {
        playlist->addSong(song);
    }

    FileManager::savePlaylists(musicLib);

    ui.displayConsole();

    ui.writeAt(
        4,
        53,
        "Cancion agregada correctamente.",
        100, 255, 100
    );
}

void AppController::removeSelectedSongFromPlaylist() {

    if (currentTab != Tab::LIBRARY)
        return;

    string playlistName =
        captureTextInput("Eliminar de playlist: ");

    if (playlistName.empty())
        return;

    Playlist* playlist = nullptr;

    // buscar playlist
    for (int i = 0; i < musicLib.getPlaylistCount(); i++) {

        Playlist* p = musicLib.getPlaylist(i);

        if (p != nullptr &&
            p->getName() == playlistName) {

            playlist = p;
            break;
        }
    }

    // no existe
    if (playlist == nullptr) {

        ui.displayConsole();

        ui.writeAt(
            4,
            53,
            "La playlist no existe.",
            255, 120, 120
        );

        return;
    }

    Song selectedSong =
        musicLib.getAllSongs()->getAt(
            librarySelectedIndex
        );

    bool found = false;

    // verificar si la canción pertenece
    for (int i = 0; i < playlist->getSize(); i++) {

        Song song = playlist->getSongAt(i);

        if (
            song.getName() == selectedSong.getName() &&
            song.getAuthor() == selectedSong.getAuthor() &&
            song.getSource() == selectedSong.getSource()
            ) {

            playlist->removeSong(i); // asumiendo que existe

            if (playlist->getSize() == 0) {

                for (int j = 0; j < musicLib.getPlaylistCount(); j++) {

                    Playlist* current =
                        musicLib.getPlaylist(j);

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

        ui.writeAt(
            4,
            53,
            "Cancion eliminada correctamente.",
            100, 255, 100
        );
    }
    else {

        ui.displayConsole();

        ui.writeAt(
            4,
            53,
            "La cancion no pertenece a la playlist.",
            255, 120, 120
        );
    }
}

void AppController::run() {
    while (true) {
        handleInput(); // Maneja la entrada del usuario en cada iteracion del loop principal.
        ui.drawSpectrum(130, 50, !audio.estaPausado()); // Dibuja el espectro en la parte inferior de la pantalla.
    }
}