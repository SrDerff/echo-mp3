#include "Interface.h"
#include "RGB.h"
#include "Background.h"
#include <windows.h>
#include "covers.h"

using namespace std;

static const int BG_R = 24;
static const int BG_G = 27;
static const int BG_B = 39;

static const int PANEL_R = 62;
static const int PANEL_G = 168;
static const int PANEL_B = 210;

static const int TITLE_R = 66;
static const int TITLE_G = 220;
static const int TITLE_B = 244;

static const int TEXT_R = 213;
static const int TEXT_G = 214;
static const int TEXT_B = 222;

static const int SOFT_R = 232;
static const int SOFT_G = 205;
static const int SOFT_B = 152;

static const int DIM_R = 145;
static const int DIM_G = 148;
static const int DIM_B = 165;

static const int ACCENT_R = 198;
static const int ACCENT_G = 120;
static const int ACCENT_B = 255;

static const int SELECT_BG_R = 49;
static const int SELECT_BG_G = 62;
static const int SELECT_BG_B = 83;

static const int BAR_BG_R = 30;
static const int BAR_BG_G = 37;
static const int BAR_BG_B = 55;

namespace {
    constexpr int kLibraryVisibleRows = 17;
}

void Interface::writeAt(int x, int y, const string& text, int fr, int fg, int fb) {
    System::Console::SetCursorPosition(x, y);
    RGB::Write(text, fr, fg, fb);
}

void Interface::writeAt(int x, int y, const string& text, int fr, int fg, int fb, int br, int bg, int bb) {
    System::Console::SetCursorPosition(x, y);
    RGB::Write(text, fr, fg, fb, br, bg, bb);
}

void Interface::hLine(int x, int y, int len, char ch, int fr, int fg, int fb) {
    for (int i = 0; i < len; i++) {
        System::Console::SetCursorPosition(x + i, y);
        RGB::Write(ch, fr, fg, fb);
    }
}

void Interface::vLine(int x, int y, int len, char ch, int fr, int fg, int fb) {
    for (int i = 0; i < len; i++) {
        System::Console::SetCursorPosition(x, y + i);
        RGB::Write(ch, fr, fg, fb);
    }
}

void Interface::fillRect(int x, int y, int w, int h, char ch, int fr, int fg, int fb) {
    string row(w, ch);
    for (int i = 0; i < h; i++) {
        writeAt(x, y + i, row, fr, fg, fb);
    }
}

void Interface::fillRect(int x, int y, int w, int h, char ch, int fr, int fg, int fb, int br, int bg, int bb) {
    string row(w, ch);
    for (int i = 0; i < h; i++) {
        writeAt(x, y + i, row, fr, fg, fb, br, bg, bb);
    }
}

void Interface::drawBox(int x, int y, int w, int h, int fr, int fg, int fb) {
    char topLeft = char(218);
    char topRight = char(191);
    char bottomLeft = char(192);
    char bottomRight = char(217);
    char horizontal = char(196);
    char vertical = char(179);

    hLine(x + 1, y, w - 2, horizontal, fr, fg, fb);
    hLine(x + 1, y + h - 1, w - 2, horizontal, fr, fg, fb);
    vLine(x, y + 1, h - 2, vertical, fr, fg, fb);
    vLine(x + w - 1, y + 1, h - 2, vertical, fr, fg, fb);

    writeAt(x, y, string(1, topLeft), fr, fg, fb);
    writeAt(x + w - 1, y, string(1, topRight), fr, fg, fb);
    writeAt(x, y + h - 1, string(1, bottomLeft), fr, fg, fb);
    writeAt(x + w - 1, y + h - 1, string(1, bottomRight), fr, fg, fb);
}

void Interface::drawProgressBar(int x, int y, int w) {
    int filled = static_cast<int>(w * 0.52f);

    for (int i = 0; i < w; i++) {
        int r = (i < filled) ? (110 + (i * 2) % 80) : BAR_BG_R;
        int g = (i < filled) ? (210 + i % 30) : BAR_BG_G;
        int b = (i < filled) ? (180 + (i * 3) % 60) : BAR_BG_B;

        System::Console::SetCursorPosition(x + i, y);
        RGB::Write(' ', 0, 0, 0, r, g, b);
    }

    writeAt(x + filled - 1, y, " ", 0, 0, 0, 180, 255, 255);
}

void Interface::drawVolumeBar(int x, int y, int level) {
    string bar = "[";
    for (int i = 0; i < 6; i++) {
        bar += (i < level ? '=' : ' ');
    }
    bar += "]";
    writeAt(x, y, bar, 245, 166, 84);
}

void Interface::drawTabs(int x, int y, int selectedX) {
    drawBox(x, y, 196, 3, PANEL_R, PANEL_G, PANEL_B);
    fillRect(x + 1, y + 1, 194, 1, ' ', PANEL_R, PANEL_G, PANEL_B);

    struct TabInfo {
        string label;
        int offset;
    };

    const TabInfo tabs[] = {
        {"Library", 64},
        {"Playlists", 80},
        {"Liked", 100},
        {"Recom.", 111},
        {"Historial", 124},
        {"Search", 143}
    };

    for (int i = 0; i < 6; ++i) {
        int tabIndex = i + 1;
        int tabX = x + tabs[i].offset;
        int tabWidth = static_cast<int>(tabs[i].label.size()) + 4;

        if (selectedX == tabIndex) {
            fillRect(tabX, y + 1, tabWidth - 2, 1, ' ', 255, 255, 255, 40, 58, 78);
            drawBox(tabX - 1, y, tabWidth, 3, PANEL_R, PANEL_G, PANEL_B);
            writeAt(tabX + 1, y + 1, tabs[i].label, TITLE_R, TITLE_G, TITLE_B);
        }
        else {
            writeAt(tabX + 1, y + 1, tabs[i].label, TEXT_R, TEXT_G, TEXT_B);
        }
    }
}

void Interface::drawTableHeader(int x, int y, int selectedTab) {
    if (selectedTab == 1 || selectedTab == 3 || selectedTab == 5 || selectedTab == 6) {
        writeAt(x, y, "Artist", 71, 136, 182);
        writeAt(x + 52, y, "Title", 71, 136, 182);
        writeAt(x + 103, y, "Duration", 71, 136, 182);
        hLine(x, y + 1, 116, '-', PANEL_R, PANEL_G, PANEL_B);
    }
    else if (selectedTab == 2) {
        writeAt(x, y, "User  ", 71, 136, 182);
        writeAt(x + 52, y, "Title", 71, 136, 182);
        writeAt(x + 103, y, "Count", 71, 136, 182);
        hLine(x, y + 1, 116, '-', PANEL_R, PANEL_G, PANEL_B);
    }
    else if (selectedTab == 4) {
        writeAt(x, y, "Artist", 71, 136, 182);
        writeAt(x + 52, y, "Title", 71, 136, 182);
        writeAt(x + 103, y, "Score", 71, 136, 182);
        hLine(x, y + 1, 116, '-', PANEL_R, PANEL_G, PANEL_B);
    }
}

void Interface::drawQueueRows(int x, int y) {
    /*to-do*/
}

void Interface::drawRightPanelPlaceholder(int x, int y, int w, int h) {
    drawBox(x + 2, y + 3, w - 4, h - 4, PANEL_R, PANEL_G, PANEL_B);
    drawAlbumArt(x + 4, y + 4, blindingLights);
}

void Interface::getAlbumArtColor(int code, int& r, int& g, int& b) {
    switch (code) {
    case 1:  r = 232; g = 155; b = 53;  break;
    case 2:  r = 220; g = 142; b = 42;  break;
    case 3:  r = 227; g = 146; b = 43;  break;
    case 4:  r = 202; g = 120; b = 20;  break;
    case 5:  r = 201; g = 114; b = 20;  break;
    case 6:  r = 183; g = 101; b = 9;   break;
    case 7:  r = 166; g = 88;  b = 6;   break;
    case 8:  r = 161; g = 86;  b = 19;  break;
    case 9:  r = 89;  g = 26;  b = 10;  break;
    case 10: r = 46;  g = 17;  b = 7;   break;
    case 11: r = 38;  g = 19;  b = 7;   break;
    case 12: r = 115; g = 74;  b = 25;  break;
    case 13: r = 6;   g = 8;   b = 6;   break;
    case 14: r = 23;  g = 11;  b = 6;   break;
    case 15: r = 241; g = 175; b = 85;  break;
    case 16: r = 144; g = 19;  b = 8;   break;
    case 17: r = 39;  g = 8;   b = 6;   break;
    case 18: r = 120; g = 8;   b = 7;   break;
    case 19: r = 252; g = 207; b = 135; break;
    case 20: r = 249; g = 195; b = 111; break;
    case 21: r = 226; g = 122; b = 71;  break;
    case 22: r = 225; g = 41;  b = 23;  break;
    case 23: r = 135; g = 133; b = 77;  break;
    case 24: r = 179; g = 119; b = 68;  break;
    default: r = 0;   g = 0;   b = 0;   break;
    }
}

void Interface::drawAlbumArt(int x, int y, const int art[25][25]) {
    for (int row = 0; row < 25; row++) {
        for (int col = 0; col < 25; col++) {
            int r, g, b;
            getAlbumArtColor(art[row][col], r, g, b);

            System::Console::SetCursorPosition(x + col * 2, y + row);
            RGB::Write(char(219), r, g, b);
            RGB::Write(char(219), r, g, b);
        }
    }
}

static string formatDuration(float seconds) {
    if (seconds <= 0) return "--:--";

    int total = static_cast<int>(seconds);
    int minutes = total / 60;
    int secs = total % 60;

    string secText = (secs < 10 ? "0" : "") + to_string(secs);
    return to_string(minutes) + ":" + secText;
}

static string fitText(const string& text, size_t maxLen) {
    if (text.size() <= maxLen) return text;
    if (maxLen <= 3) return text.substr(0, maxLen);
    return text.substr(0, maxLen - 3) + "...";
}

void Interface::drawLibraryRows(int x, int y, MusicLibrary& library, int selectedIndex, int topIndex, bool durationSortActive, bool durationSortAscending) {
    vector<Song> songs = library.getAllSongsVector();

    if (durationSortActive) {
        if (durationSortAscending) {
            MergeSort::sortByDurationAscending(songs);
        }
        else {
            MergeSort::sortByDurationDescending(songs);
        }
    }

    int currentIndex = 0;
    int drawnRows = 0;

    while (currentIndex < topIndex && currentIndex < (int)songs.size()) {
        currentIndex++;
    }

    while (currentIndex < (int)songs.size() && drawnRows < kLibraryVisibleRows) {
        Song& song = songs[currentIndex];
        int yy = y + drawnRows * 2;
        bool selected = (currentIndex == selectedIndex);

        string artist = fitText(song.getAuthor(), 30);
        string title = fitText(song.getName(), 45);
        string duration = formatDuration(song.getDuration());

        fillRect(x - 1, yy, 117, 1, ' ', 255, 255, 255,
            selected ? SELECT_BG_R : BG_R,
            selected ? SELECT_BG_G : BG_G,
            selected ? SELECT_BG_B : BG_B);

        if (selected) {
            writeAt(x, yy, artist, TITLE_R, TITLE_G, TITLE_B, SELECT_BG_R, SELECT_BG_G, SELECT_BG_B);
            writeAt(x + 37, yy, title, TITLE_R, TITLE_G, TITLE_B, SELECT_BG_R, SELECT_BG_G, SELECT_BG_B);
            writeAt(x + 108, yy, duration, TITLE_R, TITLE_G, TITLE_B, SELECT_BG_R, SELECT_BG_G, SELECT_BG_B);
        }
        else {
            writeAt(x, yy, artist, SOFT_R, SOFT_G, SOFT_B, BG_R, BG_G, BG_B);
            writeAt(x + 37, yy, title, SOFT_R, SOFT_G, SOFT_B, BG_R, BG_G, BG_B);
            writeAt(x + 108, yy, duration, SOFT_R, SOFT_G, SOFT_B, BG_R, BG_G, BG_B);
        }

        currentIndex++;
        drawnRows++;
    }

    if (drawnRows == 0) {
        writeAt(x, y, "No hay canciones cargadas en la biblioteca.", DIM_R, DIM_G, DIM_B);
        return;
    }

    while (drawnRows < kLibraryVisibleRows) {
        int yy = y + drawnRows * 2;
        fillRect(x - 1, yy, 117, 1, ' ', 255, 255, 255, BG_R, BG_G, BG_B);
        drawnRows++;
    }
}

void Interface::drawLikedRows(int x, int y, MusicLibrary& library, int selectedIndex, int topIndex) {
    vector<Song> songs = library.getLikedSongsVector();

    int currentIndex = 0;
    int drawnRows = 0;

    while (currentIndex < topIndex && currentIndex < (int)songs.size()) {
        currentIndex++;
    }

    while (currentIndex < (int)songs.size() && drawnRows < kLibraryVisibleRows) {
        Song& song = songs[currentIndex];
        int yy = y + drawnRows * 2;
        bool selected = (currentIndex == selectedIndex);

        string artist = fitText(song.getAuthor(), 30);
        string title = fitText(song.getName(), 45);
        string duration = formatDuration(song.getDuration());

        fillRect(x - 1, yy, 117, 1, ' ', 255, 255, 255,
            selected ? SELECT_BG_R : BG_R,
            selected ? SELECT_BG_G : BG_G,
            selected ? SELECT_BG_B : BG_B);

        if (selected) {
            writeAt(x, yy, artist, TITLE_R, TITLE_G, TITLE_B, SELECT_BG_R, SELECT_BG_G, SELECT_BG_B);
            writeAt(x + 37, yy, title, TITLE_R, TITLE_G, TITLE_B, SELECT_BG_R, SELECT_BG_G, SELECT_BG_B);
            writeAt(x + 108, yy, duration, TITLE_R, TITLE_G, TITLE_B, SELECT_BG_R, SELECT_BG_G, SELECT_BG_B);
        }
        else {
            writeAt(x, yy, artist, SOFT_R, SOFT_G, SOFT_B, BG_R, BG_G, BG_B);
            writeAt(x + 37, yy, title, SOFT_R, SOFT_G, SOFT_B, BG_R, BG_G, BG_B);
            writeAt(x + 108, yy, duration, SOFT_R, SOFT_G, SOFT_B, BG_R, BG_G, BG_B);
        }

        currentIndex++;
        drawnRows++;
    }

    if (drawnRows == 0) {
        writeAt(x, y, "Marca canciones con L para verlas aqui.", DIM_R, DIM_G, DIM_B);
        return;
    }

    while (drawnRows < kLibraryVisibleRows) {
        int yy = y + drawnRows * 2;
        fillRect(x - 1, yy, 117, 1, ' ', 255, 255, 255, BG_R, BG_G, BG_B);
        drawnRows++;
    }
}

void Interface::drawRecommendationRows(int x, int y, MusicLibrary& library, int selectedIndex, int topIndex, bool recommendationsSortActive, bool recommendationsSortAscending) {
    vector<RecommendationItem> recommendations = library.getRecommendedSongs();

    if (recommendationsSortActive && !recommendations.empty()) {
        QuickSort::quickSort(recommendations, 0, (int)recommendations.size() - 1, recommendationsSortAscending);
    }

    int currentIndex = 0;
    int drawnRows = 0;

    while (currentIndex < topIndex && currentIndex < (int)recommendations.size()) {
        currentIndex++;
    }

    while (currentIndex < (int)recommendations.size() && drawnRows < kLibraryVisibleRows) {
        RecommendationItem& item = recommendations[currentIndex];
        int yy = y + drawnRows * 2;
        bool selected = (currentIndex == selectedIndex);

        string artist = fitText(item.song.getAuthor(), 30);
        string title = fitText(item.song.getName(), 45);
        string score = to_string(item.score);

        fillRect(x - 1, yy, 117, 1, ' ', 255, 255, 255,
            selected ? SELECT_BG_R : BG_R,
            selected ? SELECT_BG_G : BG_G,
            selected ? SELECT_BG_B : BG_B);

        if (selected) {
            writeAt(x, yy, artist, TITLE_R, TITLE_G, TITLE_B, SELECT_BG_R, SELECT_BG_G, SELECT_BG_B);
            writeAt(x + 37, yy, title, TITLE_R, TITLE_G, TITLE_B, SELECT_BG_R, SELECT_BG_G, SELECT_BG_B);
            writeAt(x + 108, yy, score, TITLE_R, TITLE_G, TITLE_B, SELECT_BG_R, SELECT_BG_G, SELECT_BG_B);
        }
        else {
            writeAt(x, yy, artist, SOFT_R, SOFT_G, SOFT_B, BG_R, BG_G, BG_B);
            writeAt(x + 37, yy, title, SOFT_R, SOFT_G, SOFT_B, BG_R, BG_G, BG_B);
            writeAt(x + 108, yy, score, SOFT_R, SOFT_G, SOFT_B, BG_R, BG_G, BG_B);
        }

        currentIndex++;
        drawnRows++;
    }

    if (drawnRows == 0) {
        writeAt(x, y, "Marca canciones con L para generar recomendaciones.", DIM_R, DIM_G, DIM_B);
        return;
    }

    while (drawnRows < kLibraryVisibleRows) {
        int yy = y + drawnRows * 2;
        fillRect(x - 1, yy, 117, 1, ' ', 255, 255, 255, BG_R, BG_G, BG_B);
        drawnRows++;
    }
}

void Interface::drawPlaylistsRows(int x, int y, MusicLibrary& library, int selectedIndex, int topIndex) {
    int currentIndex = topIndex;
    int drawnRows = 0;

    while (currentIndex < library.getPlaylistCount() && drawnRows < kLibraryVisibleRows) {
        Playlist* playlist = library.getPlaylist(currentIndex);
        if (playlist == nullptr) break;

        int yy = y + drawnRows * 2;
        bool selected = (currentIndex == selectedIndex);

        string user = "You";
        string title = fitText(playlist->getName(), 45);
        string count = to_string(playlist->getSize());

        fillRect(x - 1, yy, 117, 1, ' ', 255, 255, 255,
            selected ? SELECT_BG_R : BG_R,
            selected ? SELECT_BG_G : BG_G,
            selected ? SELECT_BG_B : BG_B);

        if (selected) {
            writeAt(x, yy, user, TITLE_R, TITLE_G, TITLE_B, SELECT_BG_R, SELECT_BG_G, SELECT_BG_B);
            writeAt(x + 37, yy, title, TITLE_R, TITLE_G, TITLE_B, SELECT_BG_R, SELECT_BG_G, SELECT_BG_B);
            writeAt(x + 108, yy, count, TITLE_R, TITLE_G, TITLE_B, SELECT_BG_R, SELECT_BG_G, SELECT_BG_B);
        }
        else {
            writeAt(x, yy, user, SOFT_R, SOFT_G, SOFT_B, BG_R, BG_G, BG_B);
            writeAt(x + 37, yy, title, SOFT_R, SOFT_G, SOFT_B, BG_R, BG_G, BG_B);
            writeAt(x + 108, yy, count, SOFT_R, SOFT_G, SOFT_B, BG_R, BG_G, BG_B);
        }

        currentIndex++;
        drawnRows++;
    }

    if (drawnRows == 0) {
        writeAt(x, y, "No hay playlists creadas.", DIM_R, DIM_G, DIM_B);
        return;
    }

    while (drawnRows < kLibraryVisibleRows) {
        int yy = y + drawnRows * 2;
        fillRect(x - 1, yy, 117, 1, ' ', 255, 255, 255, BG_R, BG_G, BG_B);
        drawnRows++;
    }
}

void Interface::drawSpectrum(int x, int y, bool playing) {

    static int frame = 0;

    if (playing) frame++;

    vector<vector<int>> colors = {
        {197,120,255}, {188,126,255}, {177,138,255}, {165,152,255}, {152,166,255},
        {140,180,255}, {128,194,255}, {112,206,255}, {92,214,247}, {74,220,240}
    };

    const int barCount = 21;
    const int maxHeight = 6;

    for (int i = 0; i < barCount; i++) {
        double wave1 = sin(frame * 0.20 + i * 0.55);
        double wave2 = sin(frame * 0.11 + i * 0.30);
        int h = 1 + static_cast<int>(((wave1 + wave2 + 2.0) / 4.0) * (maxHeight - 1));

        int ci = (i < (int)colors.size()) ? i : (int)colors.size() - 1;
        int r = colors[ci][0];
        int g = colors[ci][1];
        int b = colors[ci][2];

        for (int j = 0; j < maxHeight; j++) {
            System::Console::SetCursorPosition(x + i * 3, y - j);

            if (j < h) {
                RGB::Write("  ", 0, 0, 0, r, g, b);
            }
            else {
                RGB::Write("  ", 24, 27, 39, 22, 24, 37);
            }
        }
    }
}

void Interface::drawBottomSeekbar(int x, int y, int w) {
    drawBox(x, y, w, 3, PANEL_R, PANEL_G, PANEL_B);

    int innerWidth = w - 2;

    for (int i = 0; i < innerWidth / 2; i++) {

        int r = (i < 100) ? (94 + i) : 34;
        int g = (i < 100) ? 210 : 41;
        int b = (i < 100) ? 176 + (i / 2) : 61;

        System::Console::SetCursorPosition(x + 1 + (i * 2), y + 1);

        RGB::Write("  ", 0, 0, 0, r, g, b);
    }

}

void Interface::displayBackground() {
    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            int screenX = x * 2;
            System::Console::SetCursorPosition(screenX, y);

            if (backgroundMatrix[y][x] == 1) {
                RGB::Write(char(219), 58, 66, 78, 0, 0, 0);
                RGB::Write(char(219), 58, 66, 78, 0, 0, 0);
            }
            else if (backgroundMatrix[y][x] == 2) {
                RGB::Write(char(219), 255, 255, 255, 0, 0, 0);
                RGB::Write(char(219), 255, 255, 255, 0, 0, 0);
            }
            else if (backgroundMatrix[y][x] == 3) {
                RGB::Write(char(219), 208, 206, 206, 0, 0, 0);
                RGB::Write(char(219), 208, 206, 206, 0, 0, 0);
            }
            else if (backgroundMatrix[y][x] == 4) {
                RGB::Write(char(219), 229, 149, 149, 0, 0, 0);
                RGB::Write(char(219), 229, 149, 149, 0, 0, 0);
            }
            else if (backgroundMatrix[y][x] == 5) {
                RGB::Write(char(219), 120, 144, 168, 0, 0, 0);
                RGB::Write(char(219), 120, 144, 168, 0, 0, 0);
            }
        }
    }
}

void Interface::displayHud(MusicLibrary& library, int selectedIndex, int topIndex) {
    drawBox(2, 1, 196, 6, 170, 176, 204);
    int index = selectedIndex;
    int totalSongs = static_cast<int>(library.getAllSongs()->getSize());

    if (index < 0 || index >= totalSongs) {
        writeAt(87, 3, "Sin cancion seleccionada", TITLE_R, TITLE_G, TITLE_B);
        writeAt(83, 4, "", SOFT_R, SOFT_G, SOFT_B);
        return;
    }

    Song currentSong = library.getAllSongs()->getAt(index);
    string name = currentSong.getName();
    string artist = currentSong.getAuthor();

    writeAt(4, 3, "|Playing|", 169, 177, 204);
    //writeAt(4, 4, "2:46 / 5:21", SOFT_R, SOFT_G, SOFT_B);

    writeAt(87, 3, name, TITLE_R, TITLE_G, TITLE_B);
    writeAt(83, 4, artist, SOFT_R, SOFT_G, SOFT_B);

    writeAt(176, 3, "Volume:", DIM_R, DIM_G, DIM_B);
    drawVolumeBar(184, 3, 5);
    writeAt(193, 3, "80%", 245, 166, 84);

    writeAt(161, 4, "Repeat", DIM_R, DIM_G, DIM_B);
    writeAt(168, 4, "/", DIM_R, DIM_G, DIM_B);
    writeAt(170, 4, "Random", DIM_R, DIM_G, DIM_B);
    writeAt(178, 4, "/", DIM_R, DIM_G, DIM_B);
    writeAt(180, 4, "Consume", ACCENT_R, 148, 255);
    writeAt(188, 4, "/", DIM_R, DIM_G, DIM_B);
    writeAt(190, 4, "Single", DIM_R, DIM_G, DIM_B);
}

void Interface::refreshHud(MusicLibrary& library, int selectedIndex, int topIndex)
{
    this->fillRect(80, 3, 40, 2, ' ', 22, 24, 37, 22, 24, 37);
    int totalSongs = static_cast<int>(library.getAllSongs()->getSize());
    if (selectedIndex < 0 || selectedIndex >= totalSongs) return;

    Song currentSong = library.getAllSongs()->getAt(selectedIndex);
    string name = currentSong.getName();
    string artist = currentSong.getAuthor();
    int center = 196 / 2;
    int nameX = center - name.size() / 2;
    int artistX = center - artist.size() / 2;
    writeAt(nameX, 3, name, TITLE_R, TITLE_G, TITLE_B);
    writeAt(artistX, 4, artist, SOFT_R, SOFT_G, SOFT_B);
}

void Interface::displayTab() {
    drawTabs(2, 8, 1);
}

void Interface::displayLibrary(MusicLibrary& library, int selectedIndex, int topIndex, bool durationSortActive, bool durationSortAscending) {
    fillRect(4, 11, 20, 1, ' ', PANEL_R, PANEL_G, PANEL_B);
    fillRect(3, 13, 118, 38, ' ', PANEL_R, PANEL_G, PANEL_B);
    drawTabs(2, 8, 1);
    drawBox(2, 12, 196, 40, PANEL_R, PANEL_G, PANEL_B);
    drawTableHeader(4, 14, 1);
    drawLibraryRows(4, 17, library, selectedIndex, topIndex, durationSortActive, durationSortAscending);
    vLine(121, 13, 38, '|', PANEL_R, PANEL_G, PANEL_B);
    drawRightPanelPlaceholder(131, 13, 58, 31);
}

void Interface::displayLiked(MusicLibrary& library, int selectedIndex, int topIndex) {
    fillRect(4, 11, 20, 1, ' ', PANEL_R, PANEL_G, PANEL_B);
    fillRect(3, 13, 118, 38, ' ', PANEL_R, PANEL_G, PANEL_B);
    drawTabs(2, 8, 3);
    drawBox(2, 12, 196, 40, PANEL_R, PANEL_G, PANEL_B);
    drawTableHeader(4, 14, 3);
    drawLikedRows(4, 17, library, selectedIndex, topIndex);
    vLine(121, 13, 38, '|', PANEL_R, PANEL_G, PANEL_B);
    drawRightPanelPlaceholder(131, 13, 58, 31);
}

void Interface::displayRecommendations(MusicLibrary& library, int selectedIndex, int topIndex, bool recommendationsSortActive, bool recommendationsSortAscending) {
    fillRect(4, 11, 20, 1, ' ', PANEL_R, PANEL_G, PANEL_B);
    fillRect(3, 13, 118, 38, ' ', PANEL_R, PANEL_G, PANEL_B);
    drawTabs(2, 8, 4);
    drawBox(2, 12, 196, 40, PANEL_R, PANEL_G, PANEL_B);
    drawTableHeader(4, 14, 4);
    drawRecommendationRows(4, 17, library, selectedIndex, topIndex, recommendationsSortActive, recommendationsSortAscending);
    vLine(121, 13, 38, '|', PANEL_R, PANEL_G, PANEL_B);
    drawRightPanelPlaceholder(131, 13, 58, 31);
}

void Interface::displayPlaylists(MusicLibrary& library, int selectedIndex, int topIndex) {
    fillRect(4, 11, 20, 1, ' ', PANEL_R, PANEL_G, PANEL_B);
    fillRect(3, 13, 118, 38, ' ', PANEL_R, PANEL_G, PANEL_B);
    drawTabs(2, 8, 2);
    drawBox(2, 12, 196, 40, PANEL_R, PANEL_G, PANEL_B);
    drawTableHeader(4, 14, 2);
    drawPlaylistsRows(4, 17, library, selectedIndex, topIndex);

    vLine(121, 13, 38, '|', PANEL_R, PANEL_G, PANEL_B);
}

void Interface::displayPlaylistSongs(
    Playlist* playlist,
    int selectedIndex,
    int topIndex
) {
    fillRect(4, 11, 20, 1, ' ', PANEL_R, PANEL_G, PANEL_B);
    fillRect(3, 14, 118, 38, ' ', PANEL_R, PANEL_G, PANEL_B);

    drawTabs(2, 8, 2);

    drawBox(2, 12, 196, 40,
        PANEL_R, PANEL_G, PANEL_B);

    if (playlist != nullptr) {
        writeAt(
            4,
            14,
            "Playlist: " + playlist->getName(),
            TITLE_R, TITLE_G, TITLE_B
        );
    }

    drawTableHeader(4, 16, 1);

    drawPlaylistSongsRows(
        4,
        19,
        playlist,
        selectedIndex,
        topIndex
    );

    vLine(121, 14, 38, '|',
        PANEL_R, PANEL_G, PANEL_B);
}

void Interface::drawPlaylistSongsRows(
    int x,
    int y,
    Playlist* playlist,
    int selectedIndex,
    int topIndex
) {
    if (playlist == nullptr) return;

    int currentIndex = topIndex;
    int drawnRows = 0;

    while (
        currentIndex < playlist->getSize() &&
        drawnRows < kLibraryVisibleRows
        ) {
        Song song = playlist->getSongAt(currentIndex);

        int yy = y + drawnRows * 2 - 1;

        bool selected = (currentIndex == selectedIndex);

        string artist = fitText(song.getAuthor(), 30);
        string title = fitText(song.getName(), 45);
        string duration = formatDuration(song.getDuration());

        fillRect(
            x - 1,
            yy,
            117,
            1,
            ' ',
            255, 255, 255,
            selected ? SELECT_BG_R : BG_R,
            selected ? SELECT_BG_G : BG_G,
            selected ? SELECT_BG_B : BG_B
        );

        if (selected) {
            writeAt(
                x,
                yy,
                artist,
                TITLE_R, TITLE_G, TITLE_B,
                SELECT_BG_R, SELECT_BG_G, SELECT_BG_B
            );

            writeAt(
                x + 37,
                yy,
                title,
                TITLE_R, TITLE_G, TITLE_B,
                SELECT_BG_R, SELECT_BG_G, SELECT_BG_B
            );

            writeAt(
                x + 108,
                yy,
                duration,
                TITLE_R, TITLE_G, TITLE_B,
                SELECT_BG_R, SELECT_BG_G, SELECT_BG_B
            );
        }
        else {
            writeAt(
                x,
                yy,
                artist,
                SOFT_R, SOFT_G, SOFT_B,
                BG_R, BG_G, BG_B
            );

            writeAt(
                x + 37,
                yy,
                title,
                SOFT_R, SOFT_G, SOFT_B,
                BG_R, BG_G, BG_B
            );

            writeAt(
                x + 108,
                yy,
                duration,
                SOFT_R, SOFT_G, SOFT_B,
                BG_R, BG_G, BG_B
            );
        }

        currentIndex++;
        drawnRows++;
    }

    if (drawnRows == 0) {
        writeAt(
            x,
            y,
            "La playlist no contiene canciones.",
            DIM_R, DIM_G, DIM_B
        );
    }
}

void Interface::displayQueue(MusicLibrary& library, int selectedIndex, int topIndex) {
    fillRect(4, 11, 20, 1, ' ', PANEL_R, PANEL_G, PANEL_B);
    fillRect(3, 13, 118, 38, ' ', PANEL_R, PANEL_G, PANEL_B);
    drawTabs(2, 8, 5);
    drawBox(2, 12, 196, 40, PANEL_R, PANEL_G, PANEL_B);
    drawTableHeader(4, 14, 5);
    displayQueueSongs(*library.getSessionHistory(), selectedIndex, topIndex);
    vLine(121, 13, 38, '|', PANEL_R, PANEL_G, PANEL_B);
    drawRightPanelPlaceholder(131, 13, 58, 31);
}
void Interface::displaySearch(MusicLibrary& library, int selectedIndex, int topIndex) {
    fillRect(3, 13, 118, 38, ' ', PANEL_R, PANEL_G, PANEL_B);
    drawTabs(2, 8, 6);
}
void Interface::displayConsole() {
    fillRect(3, 53, 50, 3, ' ', PANEL_R, PANEL_G, PANEL_B);
    drawBox(2, 52, 196, 5, PANEL_R, PANEL_G, PANEL_B);
}
void Interface::displayHelp() {
    // [Enter]
    writeAt(110, 53, "[Enter]", ACCENT_R, 148, 255);
    writeAt(117, 53, ": Reproducir / Entrar", 220, 220, 220);

    // [Espacio]
    writeAt(142, 53, "[Espacio]", ACCENT_R, 148, 255);
    writeAt(150, 53, ": Pausar / Reanudar", 220, 220, 220);

    // [+]
    writeAt(172, 53, "[+]", ACCENT_R, 148, 255);
    writeAt(175, 53, ": Agregar a playlist", 220, 220, 220);

    // [L]
    writeAt(110, 55, "[L]", ACCENT_R, 148, 255);
    writeAt(113, 55, ": Like", 220, 220, 220);

    // [A / S / D]
    writeAt(122, 55, "[A/S/D]", ACCENT_R, 148, 255);
    writeAt(130, 55, ": Duracion", 220, 220, 220);

    // [R / T]
    writeAt(143, 55, "[R/T]", ACCENT_R, 148, 255);
    writeAt(150, 55, ": Score", 220, 220, 220);

    // [O / P]
    writeAt(160, 55, "[O/P]", ACCENT_R, 148, 255);
    writeAt(167, 55, ": Playlists", 220, 220, 220);

    // [Esc]
    writeAt(181, 55, "[Esc]", ACCENT_R, 148, 255);
    writeAt(187, 55, ": Salir", 220, 220, 220);
}

void Interface::refreshLibraryRows(MusicLibrary& library, int selectedIndex, int topIndex, bool durationSortActive, bool durationSortAscending) {
    drawLibraryRows(4, 17, library, selectedIndex, topIndex, durationSortActive, durationSortAscending);
}

void Interface::refreshLikedRows(MusicLibrary& library, int selectedIndex, int topIndex) {
    drawLikedRows(4, 17, library, selectedIndex, topIndex);
}

void Interface::refreshRecommendationsRows(MusicLibrary& library, int selectedIndex, int topIndex, bool recommendationsSortActive, bool recommendationsSortAscending) {
    drawRecommendationRows(4, 17, library, selectedIndex, topIndex, recommendationsSortActive, recommendationsSortAscending);
}

void Interface::refreshLibrarySelection(MusicLibrary& library, int previousSelectedIndex, int selectedIndex, int topIndex) {
    int oldVisibleRow = previousSelectedIndex - topIndex;
    int newVisibleRow = selectedIndex - topIndex;

    Node<Song>* curr = library.getAllSongs()->getHead();
    int index = 0;

    while (curr != nullptr) {
        if (index == previousSelectedIndex && oldVisibleRow >= 0 && oldVisibleRow < kLibraryVisibleRows) {
            int y = 17 + oldVisibleRow * 2;
            const Song& song = curr->getValue();

            string artist = fitText(song.getAuthor(), 30);
            string title = fitText(song.getName(), 45);
            string duration = formatDuration(song.getDuration());

            fillRect(3, y, 117, 1, ' ', 255, 255, 255, BG_R, BG_G, BG_B);
            writeAt(4, y, artist, SOFT_R, SOFT_G, SOFT_B, BG_R, BG_G, BG_B);
            writeAt(41, y, title, SOFT_R, SOFT_G, SOFT_B, BG_R, BG_G, BG_B);
            writeAt(112, y, duration, SOFT_R, SOFT_G, SOFT_B, BG_R, BG_G, BG_B);
        }

        if (index == selectedIndex && newVisibleRow >= 0 && newVisibleRow < kLibraryVisibleRows) {
            int y = 17 + newVisibleRow * 2;
            const Song& song = curr->getValue();

            string artist = fitText(song.getAuthor(), 30);
            string title = fitText(song.getName(), 45);
            string duration = formatDuration(song.getDuration());

            fillRect(3, y, 117, 1, ' ', 255, 255, 255, SELECT_BG_R, SELECT_BG_G, SELECT_BG_B);
            writeAt(4, y, artist, TITLE_R, TITLE_G, TITLE_B, SELECT_BG_R, SELECT_BG_G, SELECT_BG_B);
            writeAt(41, y, title, TITLE_R, TITLE_G, TITLE_B, SELECT_BG_R, SELECT_BG_G, SELECT_BG_B);
            writeAt(112, y, duration, TITLE_R, TITLE_G, TITLE_B, SELECT_BG_R, SELECT_BG_G, SELECT_BG_B);
        }

        curr = curr->next;
        index++;
    }
}

void Interface::refreshPlaylistSongsSelection(
    Playlist* playlist,
    int previousSelectedIndex,
    int selectedIndex,
    int topIndex
) {

    if (playlist == nullptr) return;

    int oldVisibleRow =
        previousSelectedIndex - topIndex;

    int newVisibleRow =
        selectedIndex - topIndex;

    // quitar highlight anterior
    if (
        oldVisibleRow >= 0 &&
        oldVisibleRow < kLibraryVisibleRows
        ) {

        int y = 18 + oldVisibleRow * 2;

        Song song =
            playlist->getSongAt(previousSelectedIndex);

        string artist =
            fitText(song.getAuthor(), 30);

        string title =
            fitText(song.getName(), 45);

        string duration =
            formatDuration(song.getDuration());

        fillRect(
            3,
            y,
            117,
            1,
            ' ',
            255, 255, 255,
            BG_R, BG_G, BG_B
        );

        writeAt(
            4,
            y,
            artist,
            SOFT_R, SOFT_G, SOFT_B,
            BG_R, BG_G, BG_B
        );

        writeAt(
            41,
            y,
            title,
            SOFT_R, SOFT_G, SOFT_B,
            BG_R, BG_G, BG_B
        );

        writeAt(
            112,
            y,
            duration,
            SOFT_R, SOFT_G, SOFT_B,
            BG_R, BG_G, BG_B
        );
    }

    // dibujar nueva seleccion
    if (
        newVisibleRow >= 0 &&
        newVisibleRow < kLibraryVisibleRows
        ) {

        int y = 18 + newVisibleRow * 2;

        Song song =
            playlist->getSongAt(selectedIndex);

        string artist =
            fitText(song.getAuthor(), 30);

        string title =
            fitText(song.getName(), 45);

        string duration =
            formatDuration(song.getDuration());

        fillRect(
            3,
            y,
            117,
            1,
            ' ',
            255, 255, 255,
            SELECT_BG_R,
            SELECT_BG_G,
            SELECT_BG_B
        );

        writeAt(
            4,
            y,
            artist,
            TITLE_R, TITLE_G, TITLE_B,
            SELECT_BG_R,
            SELECT_BG_G,
            SELECT_BG_B
        );

        writeAt(
            41,
            y,
            title,
            TITLE_R, TITLE_G, TITLE_B,
            SELECT_BG_R,
            SELECT_BG_G,
            SELECT_BG_B
        );

        writeAt(
            112,
            y,
            duration,
            TITLE_R, TITLE_G, TITLE_B,
            SELECT_BG_R,
            SELECT_BG_G,
            SELECT_BG_B
        );
    }
}

void Interface::refreshPlaylistsSelection(
    MusicLibrary& library,
    int previousSelectedIndex,
    int selectedIndex,
    int topIndex
) {

    int oldVisibleRow =
        previousSelectedIndex - topIndex;

    int newVisibleRow =
        selectedIndex - topIndex;

    // quitar highlight anterior
    if (
        oldVisibleRow >= 0 &&
        oldVisibleRow < kLibraryVisibleRows
        ) {

        int y = 17 + oldVisibleRow * 2;

        Playlist* playlist =
            library.getPlaylist(previousSelectedIndex);

        if (playlist != nullptr) {

            string user = "You";

            string title =
                fitText(playlist->getName(), 45);

            string count =
                to_string(playlist->getSize());

            fillRect(
                3,
                y,
                117,
                1,
                ' ',
                255, 255, 255,
                BG_R, BG_G, BG_B
            );

            writeAt(
                4,
                y,
                user,
                SOFT_R, SOFT_G, SOFT_B,
                BG_R, BG_G, BG_B
            );

            writeAt(
                41,
                y,
                title,
                SOFT_R, SOFT_G, SOFT_B,
                BG_R, BG_G, BG_B
            );

            writeAt(
                112,
                y,
                count,
                SOFT_R, SOFT_G, SOFT_B,
                BG_R, BG_G, BG_B
            );
        }
    }

    // dibujar nueva seleccion
    if (
        newVisibleRow >= 0 &&
        newVisibleRow < kLibraryVisibleRows
        ) {

        int y = 17 + newVisibleRow * 2;

        Playlist* playlist =
            library.getPlaylist(selectedIndex);

        if (playlist != nullptr) {

            string user = "You";

            string title =
                fitText(playlist->getName(), 45);

            string count =
                to_string(playlist->getSize());

            fillRect(
                3,
                y,
                117,
                1,
                ' ',
                255, 255, 255,
                SELECT_BG_R,
                SELECT_BG_G,
                SELECT_BG_B
            );

            writeAt(
                4,
                y,
                user,
                TITLE_R, TITLE_G, TITLE_B,
                SELECT_BG_R,
                SELECT_BG_G,
                SELECT_BG_B
            );

            writeAt(
                41,
                y,
                title,
                TITLE_R, TITLE_G, TITLE_B,
                SELECT_BG_R,
                SELECT_BG_G,
                SELECT_BG_B
            );

            writeAt(
                112,
                y,
                count,
                TITLE_R, TITLE_G, TITLE_B,
                SELECT_BG_R,
                SELECT_BG_G,
                SELECT_BG_B
            );
        }
    }
}

void Interface::refreshPlaylistRows(MusicLibrary& lib, int selectedIndex, int topIndex) {
    drawPlaylistsRows(4, 17, lib, selectedIndex, topIndex);
}
void Interface::refreshPlaylistSongsRows(Playlist* playlist, int selectedIndex, int topIndex) {
    drawPlaylistSongsRows(4, 19, playlist, selectedIndex, topIndex);
}

void Interface::displayMenu(MusicLibrary& library, int selectedIndex, int topIndex, bool playing, bool durationSortActive, bool durationSortAscending) {
    displayHud(library, selectedIndex, topIndex);
    displayLibrary(library, selectedIndex, topIndex, durationSortActive, durationSortAscending);
    displayConsole();
    displayHelp();
    drawBottomSeekbar(2, 57, 196);
    drawSpectrum(130, 50, playing);
}

void Interface::displaySearchWithResults(vector<Song>& results, int selectedIndex, int topIndex, const string& query) {
    drawTabs(2, 8, 6);
    fillRect(3, 13, 118, 38, ' ', PANEL_R, PANEL_G, PANEL_B);

    string queryLine = "Search: " + query + "_";
    writeAt(4, 11, queryLine, TEXT_R, TEXT_G, TEXT_B);

    drawBox(2, 12, 196, 40, PANEL_R, PANEL_G, PANEL_B);
    drawTableHeader(4, 14, 6);
    drawResultRows(4, 17, results, selectedIndex, topIndex);
    vLine(121, 13, 38, '|', PANEL_R, PANEL_G, PANEL_B);
    drawRightPanelPlaceholder(131, 13, 58, 31);
}

void Interface::drawResultRows(int x, int y, vector<Song>& results, int selectedIndex, int topIndex) {
    int currentIndex = 0;
    int drawnRows = 0;

    while (currentIndex < topIndex && currentIndex < (int)results.size()) {
        currentIndex++;
    }

    while (currentIndex < (int)results.size() && drawnRows < kLibraryVisibleRows) {
        Song& song = results[currentIndex]; // referencia a copia segura en el vector
        int yy = y + drawnRows * 2;
        bool selected = (currentIndex == selectedIndex);

        string artist = fitText(song.getAuthor(), 30);
        string title = fitText(song.getName(), 45);
        string duration = formatDuration(song.getDuration());

        fillRect(x - 1, yy, 117, 1, ' ', 255, 255, 255,
            selected ? SELECT_BG_R : BG_R,
            selected ? SELECT_BG_G : BG_G,
            selected ? SELECT_BG_B : BG_B);

        if (selected) {
            writeAt(x, yy, artist, TITLE_R, TITLE_G, TITLE_B, SELECT_BG_R, SELECT_BG_G, SELECT_BG_B);
            writeAt(x + 37, yy, title, TITLE_R, TITLE_G, TITLE_B, SELECT_BG_R, SELECT_BG_G, SELECT_BG_B);
            writeAt(x + 108, yy, duration, TITLE_R, TITLE_G, TITLE_B, SELECT_BG_R, SELECT_BG_G, SELECT_BG_B);
        }
        else {
            writeAt(x, yy, artist, SOFT_R, SOFT_G, SOFT_B, BG_R, BG_G, BG_B);
            writeAt(x + 37, yy, title, SOFT_R, SOFT_G, SOFT_B, BG_R, BG_G, BG_B);
            writeAt(x + 108, yy, duration, SOFT_R, SOFT_G, SOFT_B, BG_R, BG_G, BG_B);
        }

        currentIndex++;
        drawnRows++;
    }

    if (drawnRows == 0) {
        writeAt(x, y, "No se encontraron resultados.", DIM_R, DIM_G, DIM_B);
        return;
    }

    while (drawnRows < kLibraryVisibleRows) {
        int yy = y + drawnRows * 2;
        fillRect(x - 1, yy, 117, 1, ' ', 255, 255, 255, BG_R, BG_G, BG_B);
        drawnRows++;
    }
}

void Interface::refreshSearchRows(vector<Song>& results, int selectedIndex, int topIndex) {
    drawResultRows(4, 17, results, selectedIndex, topIndex);
}

void Interface::refreshSearchSelection(vector<Song>& results, int previousSelectedIndex, int selectedIndex, int topIndex) {
    int oldVisibleRow = previousSelectedIndex - topIndex;
    int newVisibleRow = selectedIndex - topIndex;

    if (oldVisibleRow >= 0 && oldVisibleRow < kLibraryVisibleRows && previousSelectedIndex < (int)results.size()) {
        int y = 17 + oldVisibleRow * 2;
        Song& song = results[previousSelectedIndex];

        string artist = fitText(song.getAuthor(), 30);
        string title = fitText(song.getName(), 45);
        string duration = formatDuration(song.getDuration());

        fillRect(3, y, 117, 1, ' ', 255, 255, 255, BG_R, BG_G, BG_B);
        writeAt(4, y, artist, SOFT_R, SOFT_G, SOFT_B, BG_R, BG_G, BG_B);
        writeAt(41, y, title, SOFT_R, SOFT_G, SOFT_B, BG_R, BG_G, BG_B);
        writeAt(112, y, duration, SOFT_R, SOFT_G, SOFT_B, BG_R, BG_G, BG_B);
    }

    if (newVisibleRow >= 0 && newVisibleRow < kLibraryVisibleRows && selectedIndex < (int)results.size()) {
        int y = 17 + newVisibleRow * 2;
        Song& song = results[selectedIndex];

        string artist = fitText(song.getAuthor(), 30);
        string title = fitText(song.getName(), 45);
        string duration = formatDuration(song.getDuration());

        fillRect(3, y, 117, 1, ' ', 255, 255, 255, SELECT_BG_R, SELECT_BG_G, SELECT_BG_B);
        writeAt(4, y, artist, TITLE_R, TITLE_G, TITLE_B, SELECT_BG_R, SELECT_BG_G, SELECT_BG_B);
        writeAt(41, y, title, TITLE_R, TITLE_G, TITLE_B, SELECT_BG_R, SELECT_BG_G, SELECT_BG_B);
        writeAt(112, y, duration, TITLE_R, TITLE_G, TITLE_B, SELECT_BG_R, SELECT_BG_G, SELECT_BG_B);
    }
}

void Interface::refreshHudSong(const string& name, const string& artist) {
    this->fillRect(80, 3, 40, 2, ' ', 22, 24, 37, 22, 24, 37);
    int center = 196 / 2;
    int nameX = center - (int)name.size() / 2;
    int artistX = center - (int)artist.size() / 2;
    writeAt(nameX, 3, name, TITLE_R, TITLE_G, TITLE_B);
    writeAt(artistX, 4, artist, SOFT_R, SOFT_G, SOFT_B);
}

void Interface::displayQueueSongs(Stack<Song>& history, int selectedIndex, int topIndex) {
    int x = 4;
    int y = 17;
    int currentIndex = 0;
    int drawnRows = 0;

    while (currentIndex < topIndex && currentIndex < (int)history.size()) {
        currentIndex++;
    }

    while (currentIndex < (int)history.size() && drawnRows < kLibraryVisibleRows) {
        Song song = history.getAt(currentIndex);
        int yy = y + drawnRows * 2;
        bool selected = (currentIndex == selectedIndex);

        string artist = fitText(song.getAuthor(), 30);
        string title = fitText(song.getName(), 45);
        string duration = formatDuration(song.getDuration());

        fillRect(x - 1, yy, 117, 1, ' ', 255, 255, 255,
            selected ? SELECT_BG_R : BG_R,
            selected ? SELECT_BG_G : BG_G,
            selected ? SELECT_BG_B : BG_B);

        if (selected) {
            writeAt(x, yy, artist, TITLE_R, TITLE_G, TITLE_B, SELECT_BG_R, SELECT_BG_G, SELECT_BG_B);
            writeAt(x + 37, yy, title, TITLE_R, TITLE_G, TITLE_B, SELECT_BG_R, SELECT_BG_G, SELECT_BG_B);
            writeAt(x + 108, yy, duration, TITLE_R, TITLE_G, TITLE_B, SELECT_BG_R, SELECT_BG_G, SELECT_BG_B);
        }
        else {
            writeAt(x, yy, artist, SOFT_R, SOFT_G, SOFT_B, BG_R, BG_G, BG_B);
            writeAt(x + 37, yy, title, SOFT_R, SOFT_G, SOFT_B, BG_R, BG_G, BG_B);
            writeAt(x + 108, yy, duration, SOFT_R, SOFT_G, SOFT_B, BG_R, BG_G, BG_B);
        }

        currentIndex++;
        drawnRows++;
    }

    if (drawnRows == 0) {
        writeAt(x, y, "No hay canciones en el historial.", DIM_R, DIM_G, DIM_B);
        return;
    }

    while (drawnRows < kLibraryVisibleRows) {
        int yy = y + drawnRows * 2;
        fillRect(x - 1, yy, 117, 1, ' ', 255, 255, 255, BG_R, BG_G, BG_B);
        drawnRows++;
    }
}

void Interface::refreshQueueRows(Stack<Song>& history, int selectedIndex, int topIndex) {
    fillRect(4, 11, 20, 1, ' ', PANEL_R, PANEL_G, PANEL_B);
    displayQueueSongs(history, selectedIndex, topIndex);
}

void Interface::refreshQueueSelection(Stack<Song>& history, int previousSelectedIndex, int selectedIndex, int topIndex) {
    int oldVisibleRow = previousSelectedIndex - topIndex;
    int newVisibleRow = selectedIndex - topIndex;

    if (oldVisibleRow >= 0 && oldVisibleRow < kLibraryVisibleRows && previousSelectedIndex < (int)history.size()) {
        int y = 17 + oldVisibleRow * 2;
        Song song = history.getAt(previousSelectedIndex);

        string artist = fitText(song.getAuthor(), 30);
        string title = fitText(song.getName(), 45);
        string duration = formatDuration(song.getDuration());

        fillRect(3, y, 117, 1, ' ', 255, 255, 255, BG_R, BG_G, BG_B);
        writeAt(4, y, artist, SOFT_R, SOFT_G, SOFT_B, BG_R, BG_G, BG_B);
        writeAt(41, y, title, SOFT_R, SOFT_G, SOFT_B, BG_R, BG_G, BG_B);
        writeAt(112, y, duration, SOFT_R, SOFT_G, SOFT_B, BG_R, BG_G, BG_B);
    }

    if (newVisibleRow >= 0 && newVisibleRow < kLibraryVisibleRows && selectedIndex < (int)history.size()) {
        int y = 17 + newVisibleRow * 2;
        Song song = history.getAt(selectedIndex);

        string artist = fitText(song.getAuthor(), 30);
        string title = fitText(song.getName(), 45);
        string duration = formatDuration(song.getDuration());

        fillRect(3, y, 117, 1, ' ', 255, 255, 255, SELECT_BG_R, SELECT_BG_G, SELECT_BG_B);
        writeAt(4, y, artist, TITLE_R, TITLE_G, TITLE_B, SELECT_BG_R, SELECT_BG_G, SELECT_BG_B);
        writeAt(41, y, title, TITLE_R, TITLE_G, TITLE_B, SELECT_BG_R, SELECT_BG_G, SELECT_BG_B);
        writeAt(112, y, duration, TITLE_R, TITLE_G, TITLE_B, SELECT_BG_R, SELECT_BG_G, SELECT_BG_B);
    }
}
