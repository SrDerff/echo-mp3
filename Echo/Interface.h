#pragma once
#include "MusicLibrary.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include "User.h"
using namespace std;

class Interface
{
private:
    int height;
    int width;

public: void writeAt(int x, int y, const string& text, int fr, int fg, int fb);
public:void writeAt(int x, int y, const string& text, int fr, int fg, int fb, int br, int bg, int bb);
private:
    void hLine(int x, int y, int len, char ch, int fr, int fg, int fb);
    void vLine(int x, int y, int len, char ch, int fr, int fg, int fb);
public: void fillRect(int x, int y, int w, int h, char ch, int fr, int fg, int fb);
public:    void fillRect(int x, int y, int w, int h, char ch, int fr, int fg, int fb, int br, int bg, int bb);
private:
    void drawBox(int x, int y, int w, int h, int fr, int fg, int fb);
    void drawProgressBar(int x, int y, int w);
    void drawVolumeBar(int x, int y, int level);
    void drawTabs(int x, int y, int selectedX);
    void drawTableHeader(int x, int y, int selectedTab);
    void drawLibraryRows(int x, int y, MusicLibrary& library, int selectedIndex, int topIndex, bool durationSortActive, bool durationSortAscending);
    void drawLikedRows(int x, int y, MusicLibrary& library, User& user, int selectedIndex, int topIndex);
    void drawRecommendationRows(int x, int y, MusicLibrary& library, User&user, int selectedIndex, int topIndex, bool recommendationsSortActive, bool recommendationsSortAscending);
    void drawQueueRows(int x, int y);
    void drawPlaylistsRows(int x, int y, User& currentUser, int selectedIndex, int topIndex);
    void drawPlaylistSongsRows(int x, int y, Playlist* playlist, int selectedIndex, int topIndex);
    void drawRightPanelPlaceholder(int x, int y, int w, int h);
    void drawBottomSeekbar(int x, int y, int w);
    void drawAlbumArt(int x, int y, const int art[25][25]);
    void getAlbumArtColor(int code, int& r, int& g, int& b);

    // Recibe vector de COPIAS, no punteros
    void drawResultRows(int x, int y, vector<Song>& results, int selectedIndex, int topIndex);

public:
    Interface() {}
    Interface(int width, int height) : width(width), height(height) {}
    ~Interface() {}

    void displayHud(MusicLibrary& library, int selectedIndex, int topIndex);
    void displayTab();
    void displayQueue(MusicLibrary& library, User&user, int selectedIndex, int topIndex);
    void displayPlaylists(User& currentUser, int selectedIndex, int topIndex);
    void displayPlaylistSongs(Playlist* playlist, int selectedIndex, int topIndex);
    void displayLibrary(MusicLibrary& library, int selectedIndex, int topIndex, bool durationSortActive, bool durationSortAscending);
    void displayLiked(MusicLibrary& library, User&user, int selectedIndex, int topIndex);
    void displayRecommendations(MusicLibrary& library, User&user, int selectedIndex, int topIndex, bool recommendationsSortActive, bool recommendationsSortAscending);
    void displaySearch(MusicLibrary& library, int selectedIndex, int topIndex);
    void displayMenu(MusicLibrary& library, int selectedIndex, int topIndex, bool playing, bool durationSortActive = false, bool durationSortAscending = true);
    void displayBackground();
    void drawSpectrum(int x, int y, bool playing);
    void displayHelp();
    void displayWelcomeScreen(int selectedIndex);
    void refreshWelcomeSelection(int previousSelectedIndex, int selectedIndex);

    void refreshLibrarySelection(MusicLibrary& library, int previousSelectedIndex, int selectedIndex, int topIndex);
    void refreshHud(MusicLibrary& library, int selectedIndex, int topIndex);
    void refreshLibraryRows(MusicLibrary& library, int selectedIndex, int topIndex, bool durationSortActive, bool durationSortAscending);
    void refreshLikedRows(MusicLibrary& library,User&user, int selectedIndex, int topIndex);
    void refreshRecommendationsRows(MusicLibrary& library, User& user, int selectedIndex, int topIndex, bool recommendationsSortActive, bool recommendationsSortAscending);

	//Login & Register screens
    void displayRegisterInterface();
	void displayLoginInterface();
	void backToWelcomeScreen(int selectedIndex=1);

    //Para lo de queue
    void displayQueueSongs(Stack<Song>& library, int selectedIndex, int topIndex);
    void refreshQueueRows(Stack<Song>& library, int selectedIndex, int topIndex);
    void refreshQueueSelection(Stack<Song>& library, int previousSelectedIndex, int selectedIndex, int topIndex);

    // Metodos para renderizar busqueda (con copias seguras)
    void displaySearchWithResults(vector<Song>& results, int selectedIndex, int topIndex, const string& query);
    void refreshSearchRows(vector<Song>& results, int selectedIndex, int topIndex);
    void refreshSearchSelection(vector<Song>& results, int previousSelectedIndex, int selectedIndex, int topIndex);
    void refreshHudSong(const string& name, const string& artist);
    void refreshPlaylistRows(User& currentUser, int selectedIndex, int topIndex);
    void refreshPlaylistSongsRows(Playlist* playlist, int selectedIndex, int topIndex);
    void refreshPlaylistSongsSelection(Playlist* playlist, int previousSelectedIndex, int selectedIndex, int topIndex);
    void refreshPlaylistsSelection(User& currentUser, int previousSelectedIndex, int selectedIndex, int topIndex);
    void displayConsole();
};
