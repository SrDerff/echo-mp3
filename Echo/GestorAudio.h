#pragma once
#include <windows.h>
#include <mmsystem.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;

#pragma comment(lib, "winmm.lib")

class GestorAudio {
private:
    string alias;
    string rutaActual;
    bool cargado;
    bool pausado;
    int volume;

    wstring toWide(const string& text) {
        return wstring(text.begin(), text.end());
    }

    int cmdDebug(const wstring& cmd) {
        int ret = mciSendStringW(cmd.c_str(), NULL, 0, NULL);
        if (ret != 0) {
            wchar_t errorText[256];
            mciGetErrorStringW(ret, errorText, 256);
            //wcerr << L"[MCI ERROR " << ret << L"] " << errorText << endl;
            //wcerr << L"[MCI CMD] " << cmd << endl;
        }
        return ret;
    }

    wstring cmdReturnText(const wstring& cmd) {
        wchar_t buffer[128] = { 0 };
        int ret = mciSendStringW(cmd.c_str(), buffer, 128, NULL);
        if (ret != 0) {
            wchar_t errorText[256];
            mciGetErrorStringW(ret, errorText, 256);
            wcerr << L"[MCI ERROR " << ret << L"] " << errorText << endl;
            wcerr << L"[MCI CMD] " << cmd << endl;
            return L"";
        }
        return buffer;
    }

    int cmdReturnInt(const wstring& cmd) {
        wstring result = cmdReturnText(cmd);
        return result.empty() ? 0 : _wtoi(result.c_str());
    }

    string generateAlias() {
        static int counter = 0;
        return "mp3_" + to_string(GetTickCount()) + "_" + to_string(counter++);
    }

    bool archivoExiste(const string& ruta) {
        ifstream f(ruta);
        bool existe = f.is_open();
        if (f.is_open()) f.close();
        return existe;
    }

public:
    GestorAudio() : alias(generateAlias()), rutaActual(""), cargado(false), pausado(true), volume(500) {}

    ~GestorAudio() {
        cerrar();
    }

    bool cargar(const string& ruta) {
        if (ruta.empty()) {
            cerr << "[ERROR] La ruta esta vacia." << endl;
            return false;
        }

        if (!archivoExiste(ruta)) {
            cerr << "[ERROR] El archivo no existe o no se puede leer:" << endl;
            cerr << "        " << ruta << endl;
            return false;
        }

        cerrar();

        //cerr << "[INFO] Intentando abrir: " << ruta << endl;

        wstring rutaW = toWide(ruta);
        wstring aliasW = toWide(alias);

        wstringstream ss;
        ss << L"open \"" << rutaW << L"\" alias " << aliasW;

        int ret = mciSendStringW(ss.str().c_str(), NULL, 0, NULL);
        if (ret != 0) {
            wchar_t errorText[256];
            mciGetErrorStringW(ret, errorText, 256);
            wcerr << L"[ERROR MCI " << ret << L"] " << errorText << endl;
            cargado = false;
            return false;
        }

        cmdDebug(L"set " + aliasW + L" time format milliseconds");

        rutaActual = ruta;
        cargado = true;
        pausado = false;
        setVolumen(80);

        //cerr << "[OK] Archivo cargado correctamente." << endl;
        this->pausado = false;
        return true;
    }

    void reproducir() {
        if (!cargado) {
            //cerr << "[ERROR] No hay archivo cargado. Usa 'cargar()' primero." << endl;
            return;
        }

        wstring aliasW = toWide(alias);

        if (pausado) {
            cmdDebug(L"resume " + aliasW);
            pausado = false;
        }
        else {
            cmdDebug(L"play " + aliasW);
        }
    }

    void pausar() {
        if (cargado && !pausado) {
            cmdDebug(L"pause " + toWide(alias));
            pausado = true;
        }
    }

    void detener() {
        if (!cargado) return;

        wstring aliasW = toWide(alias);
        cmdDebug(L"stop " + aliasW);
        cmdDebug(L"seek " + aliasW + L" to start");
        pausado = false;
    }

    void cerrar() {
        if (cargado) {
            cmdDebug(L"close " + toWide(alias));
            cargado = false;
            pausado = false;
            rutaActual = "";
        }
    }

    float getDuracion() {
        if (!cargado) return 0.0f;
        int ms = cmdReturnInt(L"status " + toWide(alias) + L" length");
        return ms / 1000.0f;
    }

    float getPosicion() {
        if (!cargado) return 0.0f;
        int ms = cmdReturnInt(L"status " + toWide(alias) + L" position");
        return ms / 1000.0f;
    }

    void setPosicion(float segundos) {
        if (!cargado || segundos < 0) return;

        int ms = static_cast<int>(segundos * 1000);
        wstringstream ss;
        ss << L"seek " << toWide(alias) << L" to " << ms;
        cmdDebug(ss.str());

        if (!pausado) {
            cmdDebug(L"play " + toWide(alias));
        }
    }

    void setVolumen(int porcentaje) {
        if (porcentaje < 0) porcentaje = 0;
        if (porcentaje > 100) porcentaje = 100;

        volume = porcentaje * 10;

        if (cargado) {
            wstringstream ss;
            ss << L"setaudio " << toWide(alias) << L" volume to " << volume;
            cmdDebug(ss.str());
        }
    }

    int getVolumen() const {
        return volume / 10;
    }

    bool estaPausado() const {
        return pausado;
    }

    bool estaCargado() const {
        return cargado;
    }

    bool estaReproduciendo() {
        if (!cargado) return false;
        wstring estado = cmdReturnText(L"status " + toWide(alias) + L" mode");
        return estado == L"playing";
    }

    string getActual() const {
        return rutaActual;
    }
};