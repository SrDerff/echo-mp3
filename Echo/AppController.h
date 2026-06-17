#pragma once
#include "Interface.h"
#include "MusicLibrary.h"
#include "FileManager.h"
#include "GestorAudio.h"
#include <conio.h>
#include <windows.h>
#include <vector>
#include <string>

using namespace std;

enum class Tab {
    LIBRARY = 1,
    PLAYLISTS = 2,
    QUEUE = 3,
    SEARCH = 4
};

class AppController
{
private:
    Interface ui;
    MusicLibrary musicLib;
    Tab currentTab;
    int currentTabIndex;
    GestorAudio audio;

    int librarySelectedIndex;
    int libraryTopIndex;

    int playlistsSelectedIndex;
    int playlistsTopIndex;

    bool insidePlaylist;
    int openedPlaylistIndex;

    int playlistSongsSelectedIndex;
    int playlistSongsTopIndex;

    int queueSelectedIndex;
    int queueTopIndex;

    // Estado de busqueda (copias seguras, no punteros)
    vector<Song> searchResults;
    string searchQuery;
    int searchSelectedIndex;
    int searchTopIndex;

    void render(string type);
    void renderRefresh();
    void renderSwap();
    void handleInput();
    void moveDown();
    void moveUp();
    void playSelectedSong();
    void playSelectedPlaylistSong();
    void addSelectedSongToPlaylist();
    void removeSelectedSongFromPlaylist();
    string captureTextInput(const string& prompt);

    // Helpers de busqueda
    void moveDownSearch();
    void moveUpSearch();
    void performSearch();
    void playSelectedSearchSong();

    void moveDownQueue();
    void moveUpQueue();
    void playSelectedQueueSong();

public:
    AppController();
    ~AppController();

    AppController(const AppController&) = delete;
    AppController& operator=(const AppController&) = delete;

    void run();
};