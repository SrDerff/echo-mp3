#include "AppController.h"
#include <iostream>
using namespace std;

int main() {
    System::Console::CursorVisible = false;
    SetConsoleTitle(L"ECHO");
    System::Console::SetWindowSize(200, 60);

    AppController app;
    app.run();
    return 0;
}