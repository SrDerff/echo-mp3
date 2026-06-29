#include "AppController.h"
#include <iostream>
using namespace std;

/*
Importancia:
Punto de entrada unico del ejecutable. Configura el entorno de consola necesario para
que la interfaz grafica ASCII funcione correctamente y luego transfiere el control
total al controlador de la aplicacion. Sin esta funcion el sistema no puede iniciarse.

BigO: O(1)
Tiempo constante en todas las operaciones de inicializacion. La funcion termina
unicamente cuando `AppController::run()` retorna, por lo que la complejidad
real del programa depende enteramente de lo que ocurra dentro del bucle principal.
Espacio adicional O(1); no se asignan estructuras dinamicas en este ambito.

Explicacion detallada:
1. Oculta el cursor parpadeante de la consola para mejorar la presentacion visual.
2. Establece el titulo de la ventana como "ECHO".
3. Redimensiona la ventana a 200 columnas x 60 filas para alojar toda la interfaz.
4. Crea una instancia de `AppController`, cuyo constructor carga datos del disco
   y renderiza el menu principal.
5. Llama a `AppController::run()`, que entra en un bucle infinito de lectura de
   teclado y actualizacion de pantalla. main() solo retorna cuando la app se cierra.
6. Devuelve 0 al sistema operativo indicando finalizacion exitosa.
*/
int main() {
    System::Console::CursorVisible = false;
    SetConsoleTitle(L"ECHO");
    System::Console::SetWindowSize(200, 60);

    AppController app;
    app.run();
    return 0;
}
