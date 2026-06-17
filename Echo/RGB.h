#pragma once
#include <windows.h>
#include <iostream>
#include <string>

class RGB
{
private:
    static bool ansiEnabled;

public:
    static void Init()
    {
        if (ansiEnabled) return;

        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut == INVALID_HANDLE_VALUE) return;

        DWORD mode = 0;
        if (!GetConsoleMode(hOut, &mode)) return;

        SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        ansiEnabled = true;
    }

    static void Write(char c, int r, int g, int b)
    {
        Init();
        std::cout << "\x1b[38;2;" << r << ";" << g << ";" << b << "m"
            << c
            << "\x1b[0m";
    }

    static void Write(const char* text, int r, int g, int b)
    {
        Init();
        std::cout << "\x1b[38;2;" << r << ";" << g << ";" << b << "m"
            << text
            << "\x1b[0m";
    }

    static void Write(const std::string& text, int r, int g, int b)
    {
        Init();
        std::cout << "\x1b[38;2;" << r << ";" << g << ";" << b << "m"
            << text
            << "\x1b[0m";
    }

    static void WriteLine(char c, int r, int g, int b)
    {
        Write(c, r, g, b);
        std::cout << '\n';
    }

    static void WriteLine(const char* text, int r, int g, int b)
    {
        Write(text, r, g, b);
        std::cout << '\n';
    }

    static void WriteLine(const std::string& text, int r, int g, int b)
    {
        Write(text, r, g, b);
        std::cout << '\n';
    }

    static void Write(char c, int fr, int fg, int fb, int br, int bg, int bb)
    {
        Init();
        std::cout << "\x1b[38;2;" << fr << ";" << fg << ";" << fb
            << ";48;2;" << br << ";" << bg << ";" << bb << "m"
            << c
            << "\x1b[0m";
    }

    static void Write(const char* text, int fr, int fg, int fb, int br, int bg, int bb)
    {
        Init();
        std::cout << "\x1b[38;2;" << fr << ";" << fg << ";" << fb
            << ";48;2;" << br << ";" << bg << ";" << bb << "m"
            << text
            << "\x1b[0m";
    }

    static void Write(const std::string& text, int fr, int fg, int fb, int br, int bg, int bb)
    {
        Init();
        std::cout << "\x1b[38;2;" << fr << ";" << fg << ";" << fb
            << ";48;2;" << br << ";" << bg << ";" << bb << "m"
            << text
            << "\x1b[0m";
    }

    static void WriteLine(char c, int fr, int fg, int fb, int br, int bg, int bb)
    {
        Write(c, fr, fg, fb, br, bg, bb);
        std::cout << '\n';
    }

    static void WriteLine(const char* text, int fr, int fg, int fb, int br, int bg, int bb)
    {
        Write(text, fr, fg, fb, br, bg, bb);
        std::cout << '\n';
    }

    static void WriteLine(const std::string& text, int fr, int fg, int fb, int br, int bg, int bb)
    {
        Write(text, fr, fg, fb, br, bg, bb);
        std::cout << '\n';
    }

    static void Reset()
    {
        std::cout << "\x1b[0m";
    }
};