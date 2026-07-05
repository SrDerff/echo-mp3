# ECHO-MP3

**Reproductor Musical en Consola — Documentación Completa**

---

- **Curso:** Estructuras de Datos
- **Ciclo:** 3
- **Universidad:** Universidad Peruana de Ciencias Aplicadas (UPC)
- **Lenguaje:** C++17
- **Plataforma:** Windows (x64)
- **Entorno:** Visual Studio 2022
- **Archivo de solución:** `Echo.sln`

---

## 1. Concepto

**Echo-MP3** es un reproductor de música funcional que se ejecuta íntegramente en la terminal de Windows. Combina una interfaz gráfica ASCII con colores RGB de 24 bits, reproducción de audio real mediante la API Multimedia de Windows (MCI), y un motor interno de búsqueda, recomendación y gestión de biblioteca musical. Su propósito principal es **demostrar la aplicación práctica de estructuras de datos y algoritmos de ordenamiento** en un producto de software real.

El proyecto implementa desde cero **6 estructuras de datos** (lista doblemente enlazada, lista circular, pila, tabla hash, árbol rojo-negro, árbol AVL y grafo) y **3 algoritmos de ordenamiento** (QuickSort, MergeSort, HeapSort), todos utilizados para funcionalidades concretas del reproductor.

---

## 2. Propósito

El propósito de Echo-MP3 es **académico-demostrativo**:

1. Aplicar estructuras de datos clásicas en un sistema funcional, no meramente teórico.
2. Comparar el rendimiento y la utilidad de diferentes estructuras (árboles balanceados vs. tablas hash vs. listas enlazadas) para distintos tipos de consultas.
3. Implementar algoritmos de ordenamiento y observar su comportamiento con volúmenes de datos variables.
4. Integrar múltiples componentes de software (UI, audio, persistencia, lógica de negocio) en un solo ejecutable.
5. Desarrollar una experiencia de usuario atractiva dentro de las limitaciones de una consola de texto.

---

## 3. Tecnologías

| Tecnología | Uso |
|------------|-----|
| **C++17** | Lenguaje de programación principal, uso de `std::vector`, `std::string`, `std::stringstream`, `std::fstream`, `std::random` |
| **Windows API (MCI)** | Reproducción de audio MP3 mediante `mciSendStringW` (librería `winmm.lib`) |
| **ANSI Escape Codes** | Colores RGB de 24 bits en consola (`\x1b[38;2;R;G;Bm`) habilitados con `ENABLE_VIRTUAL_TERMINAL_PROCESSING` |
| **Windows Console API** | Control de tamaño de ventana (`SetConsoleWindowInfo`), ocultación de cursor (`SetConsoleCursorInfo`), título de ventana (`SetConsoleTitle`) |
| **WinSock2** | (`ws2_32.lib`) Incluido pero no utilizado en la versión actual |
| **Visual Studio 2022** | Entorno de desarrollo y compilación |
| **Arquitectura** | x64, compilación en modo Debug y Release |

---

## 4. Arquitectura del Sistema

```
                    ┌─────────────┐
                    │   main.cpp   │
                    │  (entrada)   │
                    └──────┬──────┘
                           │
                    ┌──────▼──────┐
                    │  AppController │
                    │ (controlador)  │
                    └──┬──┬──┬──┬──┘
                       │  │  │  │
            ┌──────────┘  │  │  └──────────┐
            │             │  │              │
     ┌──────▼─────┐  ┌───▼──▼───┐  ┌───────▼──────┐
     │  Interface │  │ GestorAudio│  │  FileManager │
     │   (UI)     │  │  (audio)   │  │ (persistencia)│
     └────────────┘  └───────────┘  └──────┬───────┘
                                           │
                                    ┌──────▼───────┐
                                    │  MusicLibrary │
                                    │  (biblioteca) │
                                    └──┬──┬──┬──┬──┘
                                       │  │  │  │
                            ┌──────────┘  │  │  └──────────┐
                            │             │  │              │
                      ┌─────▼──┐   ┌──────▼──▼──────┐  ┌───▼──────┐
                      │  User  │   │  Estructuras   │  │ Sorting  │
                      │(usuario)│   │    de Datos    │  │Algoritmos│
                      └────────┘   └────────────────┘  └──────────┘
```

### Flujo de ejecución

1. **`main()`** — Configura la consola (200x60, cursor oculto, título "ECHO"), crea una instancia de `AppController` y llama a `app.run()`.
2. **`AppController` (constructor)** — Carga las canciones desde `Data/songsList.txt` mediante `FileManager::loadSongs()`, muestra la pantalla de bienvenida.
3. **`AppController::run()`** — Bucle infinito que ejecuta dos operaciones por iteración:
   - `handleInput()` — Procesa teclas presionadas (navegación, reproducción, búsqueda, etc.)
   - `drawSpectrum()` — Anima el visualizador de espectro en la parte inferior
4. **Pantalla de bienvenida** — El usuario puede iniciar sesión, registrarse o salir.
5. **Menú principal** — 6 pestañas navegables con las funciones del reproductor.
6. **Salida** — Tecla `Esc` para retroceder o salir de la aplicación.

---

## 5. Estructuras de Datos Implementadas

Echo-MP3 implementa **7 estructuras de datos** desde cero (sin usar STL excepto `vector` y `string`):

### 5.1 Node\<T\> (`Node.h`)

| Propiedad | Descripción |
|-----------|-------------|
| **Tipo** | Struct template |
| **Campos** | `T value`, `Node<T>* next`, `Node<T>* prev` |
| **Constructor** | `Node(T val)` — inicializa valor y punteros a `nullptr` |
| **Propósito** | Nodo genérico para listas doblemente enlazadas |

### 5.2 DoublyLinkedList\<T\> (`DoublyLinkedList.h`)

| Propiedad | Descripción |
|-----------|-------------|
| **Tipo** | Lista doblemente enlazada genérica |
| **Campos** | `Node<T>* head`, `Node<T>* tail`, `int size` |
| **Operaciones** | `insertAtHead`, `insertAtTail`, `insertAt`, `removeNode`, `removeAt`, `getAt`, `getHead`, `getTail`, `clear`, `isEmpty` |
| **Complejidad** | Inserción/eliminación en cabeza/cola: `O(1)`. Búsqueda por índice: `O(n)` |
| **Uso en el proyecto** | Contenedor principal de canciones (`MusicLibrary::allSongs`), canciones en playlists, canciones por artista/álbum |
| **Extras** | Copy constructor, move constructor, operador de asignación por copia y movimiento |

### 5.3 CircularDoublyLinkedList\<T\> (`CircularDoublyLinkedList.h`)

| Propiedad | Descripción |
|-----------|-------------|
| **Tipo** | Lista circular doblemente enlazada genérica |
| **Diferencias** | `tail->next == head`, `head->prev == tail` |
| **Operaciones** | `insert`, `remove`, `getAt`, `next`, `prev`, `getCurrent`, `setCurrent`, `clear` |
| **Uso en el proyecto** | Cola de reproducción (`MusicLibrary::queue`) — navegación circular entre canciones |

### 5.4 Stack\<T\> (`Stack.h`)

| Propiedad | Descripción |
|-----------|-------------|
| **Tipo** | Pila LIFO (lista enlazada simple) |
| **Campos** | `StackNode<T>* topNode`, `int count` |
| **Operaciones** | `push`, `pop`, `top`, `getAt`, `clear`, `isEmpty`, `getCount` |
| **Complejidad** | `push`, `pop`, `top`: `O(1)`. `getAt`: `O(n)` |
| **Uso en el proyecto** | Historial de sesión del usuario (`User::sessionHistory`, `MusicLibrary::sessionHistory`) |

### 5.5 HashTable\<K, V\> (`HashTable.h`)

| Propiedad | Descripción |
|-----------|-------------|
| **Tipo** | Tabla hash con encadenamiento separado |
| **Función hash** | `djb2` para strings; hash multiplicativo para tipos numéricos |
| **Política de rehash** | Cuando el factor de carga > 0.75, duplica el número de buckets |
| **Capacidad inicial** | 16 buckets |
| **Operaciones** | `insert`, `find`, `findPtr`, `remove`, `contains`, `clear`, `getAllItems` |
| **Complejidad** | Promedio `O(1)`, peor caso `O(n)` por colisiones |
| **Uso en el proyecto** | 4 tablas hash: `indexBySource` (ruta → Song*), `indexByArtist` (artista → vector<Song*>), `indexByGenre` (género → vector<Song*>), `User::likedSongs` (ruta → bool) |

### 5.6 RedBlackTree\<K, V\> (`RedBlackTree.h`)

| Propiedad | Descripción |
|-----------|-------------|
| **Tipo** | Árbol rojo-negro con orientación izquierda (Left-Leaning Red-Black Tree) |
| **Coloración** | Nodos rojos y negros con rotaciones para mantener balance |
| **Operaciones** | `insert`, `search`, `searchPtr`, `remove`, `clear`, `inorder`, `prefixSearch` |
| **Complejidad** | `O(log n)` en todas las operaciones |
| **Uso en el proyecto** | `indexByName` — índice de canciones por nombre. `prefixSearch()` permite búsqueda eficiente por prefijo (para la búsqueda en vivo) |

### 5.7 AVLTree\<K, V\> (`AVLTree.h`)

| Propiedad | Descripción |
|-----------|-------------|
| **Tipo** | Árbol AVL (auto-balanceable por alturas) |
| **Balanceo** | Factor de balance: -1, 0, 1. Rotaciones simple y doble |
| **Operaciones** | `insert`, `search`, `searchPtr`, `remove`, `clear`, `inorder`, `reverseInorder`, `rangeSearch` |
| **Complejidad** | `O(log n)` en todas las operaciones |
| **Uso en el proyecto** | 2 árboles AVL: `indexByDuration` (duración → Song) y `indexByPlayCount` (reproducciones → Song). `rangeSearch()` permite consultas por rango de duración. `reverseInorder()` obtiene las canciones más reproducidas |

### 5.8 Graph\<T\> (`Graph.h`)

| Propiedad | Descripción |
|-----------|-------------|
| **Tipo** | Grafo no dirigido ponderado (lista de adyacencia) |
| **Campos** | `vector<T> vertices`, `vector<vector<Edge>> adjacency` (Edge: `int to`, `int weight`) |
| **Operaciones** | `addVertex`, `addEdge`, `findVertexIndex` (con predicado), `forEachNeighbor` (patron visitor) |
| **Uso en el proyecto** | Motor de recomendación: vértices = canciones, aristas = similitud (mismo artista +40, mismo género +30, duración similar +10) |

---

## 6. Algoritmos de Ordenamiento

### 6.1 QuickSort (`SortEngine.h` y `QuickSort.h`)

| Propiedad | Descripción |
|-----------|-------------|
| **Implementación** | Dos versiones: sobre `vector<T*>` con comparador (SortEngine) y sobre `vector<T>` (QuickSort) |
| **Estrategia** | División y conquista con pivote |
| **Complejidad** | Promedio `O(n log n)`, peor `O(n²)` |
| **Uso en el proyecto** | Ordenar canciones por nombre alfabéticamente (`MusicLibrary::getSongsSortedByName`), ordenar recomendaciones por puntaje (`Interface::drawRecommendationRows`) |

### 6.2 MergeSort (`MergeSort.h`)

| Propiedad | Descripción |
|-----------|-------------|
| **Implementación** | MergeSort genérico + helpers `sortByDurationAscending` y `sortByDurationDescending` |
| **Estrategia** | División y conquista con mezcla |
| **Complejidad** | `O(n log n)` en todos los casos |
| **Uso en el proyecto** | Ordenar biblioteca por duración (teclas A/S), tanto para `vector<Song*>` como `vector<Song>` |

### 6.3 HeapSort (`HeapSort.h`)

| Propiedad | Descripción |
|-----------|-------------|
| **Implementación** | HeapSort genérico con control ascendente/descendente + `sortBySongCount` para playlists |
| **Estrategia** | Construcción de heap binario y extracción ordenada |
| **Complejidad** | `O(n log n)` en todos los casos |
| **Uso en el proyecto** | Ordenar playlists por cantidad de canciones (teclas O/P) |

---

## 7. Funcionalidades del Reproductor

### 7.1 Reproducción de Audio

- **Motor:** Clase `GestorAudio` que utiliza la API Multimedia de Windows (`mciSendStringW`).
- **Formatos soportados:** MP3 (y cualquier formato que MCI pueda manejar).
- **Operaciones:**
  - `cargar(ruta)` — Abre un archivo de audio y establece volumen inicial al 80%.
  - `reproducir()` — Reproduce desde el inicio o reanuda si estaba en pausa.
  - `pausar()` — Pausa la reproducción actual.
  - `detener()` — Detiene y rebobina al inicio.
  - `cerrar()` — Cierra el archivo de audio.
  - `setPosicion(segundos)` — Adelanta o retrocede a una posición específica.
  - `setVolumen(0-100)` — Controla el volumen.
  - `getDuracion()` / `getPosicion()` — Consulta duración total y posición actual.
  - `estaReproduciendo()` / `estaPausado()` — Consulta el estado.
- **Controles:** Enter (reproducir), Espacio (pausa/reanudar).

### 7.2 Biblioteca Musical

- Almacenamiento central en `MusicLibrary` con lista doblemente enlazada `allSongs`.
- Indexación en **6 estructuras de datos** para diferentes tipos de consulta.
- Visualización en pestaña "Library" con scroll virtual.
- Ordenamiento por duración (teclas A/S/D) y por nombre (navegación N/B).

### 7.3 Búsqueda en Vivo

- **Dos modos de búsqueda:**
  1. **Búsqueda por prefijo** (prefix search): Usa el árbol rojo-negro `indexByName` para encontrar canciones cuyo nombre comience con el texto ingresado. Eficiente: `O(log n + m)`.
  2. **Búsqueda incremental** (substring search): Recorre toda la lista enlazada y verifica si el query es substring del nombre o del artista (case-insensitive). Usada como respaldo o para queries cortos.
- **Interfaz:** Pestaña "Search" con resultados en vivo mientras el usuario escribe.
- **Tecla Backspace:** Elimina el último carácter y actualiza la búsqueda.

### 7.4 Playlists

- Cada usuario puede tener **múltiples playlists** (capacidad reservada de 100).
- **Operaciones:**
  - **Tecla `+`:** Agrega la canción seleccionada a una playlist (la crea si no existe).
  - **Tecla `-`:** Elimina la canción seleccionada de una playlist (la elimina si queda vacía).
- **Vista:** Pestaña "Playlists" lista las playlists con su conteo de canciones. Enter para abrir y ver canciones, Esc para retroceder.
- **Ordenamiento:** Teclas O/P para ordenar playlists por cantidad de canciones (HeapSort).
- **Daily Mix:** Generado automáticamente al iniciar sesión (~30 minutos de música optimizada).

### 7.5 Likes

- **Tecla `L`:** Alterna el estado de "me gusta" de la canción seleccionada.
- **Vista:** Pestaña "Liked" muestra solo las canciones marcadas.
- **Persistencia:** Guardado por usuario en `<username>_likes.txt`.
- **Almacenamiento:** Tabla hash `HashTable<string, bool>` keyeada por ruta de archivo.

### 7.6 Historial de Sesión

- Cada canción reproducida se agrega a una pila (`Stack<Song>`) en el objeto `User`.
- **Vista:** Pestaña "Historial" muestra las canciones reproducidas (más reciente primero).
- Filtro de duplicados consecutivos.

### 7.7 Sistema de Recomendación

**Dos motores de recomendación complementarios:**

**A. Basado en grafo (`MusicLibrary::getRecommendedSongs`):**
- Construye un grafo ponderado donde los vértices son todas las canciones.
- Peso de aristas = similitud: mismo artista (+40), mismo género (+30), duración similar (+10).
- Por cada canción en el historial, refuerza los vecinos con `10 + pesoArista`.
- Excluye canciones ya en el historial.

**B. Basado en historial y likes (`User::getRecommendedSongs`):**
- Por cada canción no-likeada:
  - Coincidencia con historial: +10 mismo artista, +15 mismo género.
  - Coincidencia con likes: +20 mismo género.
- Puntaje acumulado para ranking.

**Ordenamiento:** QuickSort por puntaje (teclas R/T para ascendente/descendente).

### 7.8 Visualizador de Espectro

- **Ubicación:** Coordenadas (130, 50) en la interfaz.
- **Animación:** 21 barras que oscilan usando ondas seno combinadas (2 ondas por barra).
- **Colores:** Degradado de púrpura a cian.
- **Comportamiento:** Se congela cuando el audio está en pausa.

### 7.9 Persistencia de Datos

| Archivo | Ubicación | Formato | Propósito |
|---------|-----------|---------|-----------|
| `songsList.txt` | `Data/` | `nombre\|artista\|fuente\|duración\|género\|playcount\|liked` | Base de datos de canciones |
| `users_list.txt` | `Data/users/` | `usuario\|hashPassword\|premium` | Cuentas de usuario |
| `<usuario>_playlists.txt` | `Data/users/` | `[ENDPLAYLIST]` como separador | Playlists por usuario |
| `<usuario>_likes.txt` | `Data/users/` | Una ruta por línea | Canciones likeadas |
| `<usuario>_history.txt` | `Data/users/` | Una ruta por línea | Historial de sesión persistido |

---

## 8. Interfaz de Usuario

### 8.1 Configuración de Consola

- **Dimensiones:** 200 columnas × 60 filas.
- **Cursor:** Oculto (`SetConsoleCursorInfo`).
- **Colores:** ANSI 24-bit RGB mediante códigos de escape.
- **Fuente:** Raster (por defecto de la consola).

### 8.2 Sistema de Tabs

| # | Nombre | Tecla | Contenido |
|---|--------|-------|-----------|
| 1 | **Library** | ← → | Todas las canciones de la biblioteca |
| 2 | **Playlists** | ← → | Playlists del usuario o canciones dentro de una |
| 3 | **Liked** | ← → | Canciones marcadas con "me gusta" |
| 4 | **Recom.** | ← → | Recomendaciones con puntaje |
| 5 | **Historial** | ← → | Canciones reproducidas en la sesión |
| 6 | **Search** | ← → | Búsqueda en vivo |

### 8.3 Paleta de Colores

| Constante | RGB | Uso |
|-----------|-----|-----|
| `BG_R, BG_G, BG_B` | (24, 27, 39) | Fondo oscuro de la interfaz |
| `PANEL_R, PANEL_G, PANEL_B` | (62, 168, 210) | Bordes de paneles (cian) |
| `TITLE_R, TITLE_G, TITLE_B` | (213, 214, 222) | Texto de títulos (blanco) |
| `SOFT_R, SOFT_G, SOFT_B` | (232, 205, 152) | Texto normal (crema) |
| `DIM_R, DIM_G, DIM_B` | (145, 148, 165) | Texto tenue (gris) |
| `SELECT_BG_R, SELECT_BG_G, SELECT_BG_B` | (49, 62, 83) | Fondo de selección |

### 8.4 Controles de Teclado

| Tecla | Acción |
|-------|--------|
| **← →** | Navegar entre pestañas |
| **↑ ↓** | Navegar por elementos de la lista |
| **Enter** | Reproducir canción / abrir playlist |
| **Espacio** | Pausar / reanudar reproducción |
| **Esc** | Retroceder nivel / salir de la aplicación |
| **L** | Dar / quitar like |
| **+** | Agregar canción a playlist |
| **-** | Quitar canción de playlist |
| **A** | Ordenar biblioteca por duración ascendente |
| **S** | Ordenar biblioteca por duración descendente |
| **D** | Desactivar ordenamiento |
| **R** | Ordenar recomendaciones por puntaje descendente |
| **T** | Ordenar recomendaciones por puntaje ascendente |
| **O** | Ordenar playlists por cantidad ascendente |
| **P** | Ordenar playlists por cantidad descendente |
| **N** | Siguiente canción alfabéticamente |
| **B** | Canción anterior alfabéticamente |
| **Caracteres imprimibles** | Escribir en la búsqueda |

### 8.5 Pantalla de Bienvenida

- Arte ASCII decorativo con barras de ecualizador animadas.
- Símbolos de nota musical.
- Esquema de colores cian y púrpura.
- 3 opciones: Login (1), Register (2), Exit (3).

---

## 9. Sistema de Usuarios

### 9.1 Registro

- Solicita nombre de usuario y contraseña.
- La contraseña se almacena como hash numérico simple (no criptográfico).
- Crea archivos de datos vacíos para el nuevo usuario.
- Genera automáticamente un "Daily Mix" al registrarse.

### 9.2 Inicio de Sesión

- Verifica credenciales contra `Data/users/users_list.txt`.
- Carga playlists, likes e historial del usuario.
- Genera "Daily Mix" si no existe.

### 9.3 Datos por Usuario

Cada usuario tiene sus propios archivos en `Data/users/`:
- `<usuario>_playlists.txt`
- `<usuario>_likes.txt`
- `<usuario>_history.txt`

---

## 10. Normalización de Géneros

La función `MusicLibrary::normalizeGenre()` mapea nombres de canciones a géneros estandarizados mediante una tabla de búsqueda de 60 entradas:

**Géneros reconocidos:**
- Rock, Pop, Hip Hop, Latin, Electronic, Ballad, Other (por defecto)

Este mapeo se aplica automáticamente al agregar canciones a la biblioteca, garantizando consistencia en los filtros por género y en el sistema de recomendación.

---

## 11. Listado Completo de Archivos

### Código Fuente (`Echo/`)

| Archivo | Líneas | Propósito |
|---------|--------|-----------|
| `main.cpp` | 35 | Punto de entrada del programa |
| `AppController.h` | 110 | Cabecera del controlador principal |
| `AppController.cpp` | 1516 | Lógica principal del reproductor |
| `Interface.h` | 93 | Cabecera del motor de interfaz |
| `Interface.cpp` | 1842 | Renderizado de la interfaz ASCII |
| `RGB.h` | 117 | Utilidades de color ANSI 24-bit |
| `RGB.cpp` | 2 | Inicialización de variables estáticas |
| `Song.h` | 110 | Entidad Canción |
| `Artist.h` | 38 | Entidad Artista |
| `Album.h` | 37 | Entidad Álbum |
| `Playlist.h` | 74 | Entidad Lista de Reproducción |
| `User.h` | 196 | Entidad Usuario |
| `UserManager.h` | 44 | Gestión de usuarios |
| `Node.h` | 19 | Nodo genérico para listas |
| `DoublyLinkedList.h` | 227 | Lista doblemente enlazada |
| `CircularDoublyLinkedList.h` | 97 | Lista circular doblemente enlazada |
| `Stack.h` | 99 | Pila (LIFO) |
| `HashTable.h` | 180 | Tabla hash con encadenamiento |
| `RedBlackTree.h` | 275 | Árbol rojo-negro (LLRB) |
| `AVLTree.h` | 234 | Árbol AVL |
| `Graph.h` | 59 | Grafo no dirigido ponderado |
| `SortEngine.h` | 44 | QuickSort para punteros |
| `QuickSort.h` | 67 | QuickSort para valores + RecommendationItem |
| `MergeSort.h` | 88 | MergeSort + helpers de duración |
| `HeapSort.h` | 82 | HeapSort + ordenamiento de playlists |
| `SearchSong.h` | 73 | Utilidades de búsqueda |
| `FileManager.h` | 566 | Persistencia de datos |
| `GestorAudio.h` | 216 | Reproducción de audio (MCI) |
| `EchoService.h` | 17 | Stub de capa de servicio |
| `Auth.h` | 11 | Stub de autenticación |
| `background.h` | 65 | Matriz de fondo de pantalla de bienvenida |
| `covers.h` | 31 | Carátula de álbum de muestra |

### Datos (`Data/`)

| Archivo | Propósito |
|---------|-----------|
| `songsList.txt` | Base de datos de canciones |
| `users/users_list.txt` | Cuentas de usuario |
| `users/<usuario>_playlists.txt` | Playlists por usuario |
| `users/<usuario>_likes.txt` | Likes por usuario |
| `users/<usuario>_history.txt` | Historial por usuario |

### Otros

| Archivo | Propósito |
|---------|-----------|
| `Canciones/` | Archivos MP3 |
| `Echo.sln` | Archivo de solución de Visual Studio |
| `echo-mp3.sln` | Archivo de solución alternativo |
| `Echo/Echo.vcxproj` | Archivo de proyecto de Visual Studio |
| `README.md` | README del proyecto |
| `documentacion.md` | Documentación técnica previa |

---

## 12. Diagrama de Clases (Relaciones Principales)

```
┌─────────────┐       ┌──────────────────┐
│  AppController│──────>│  GestorAudio     │
│              │       │  (audio MCI)     │
│  - audio     │       └──────────────────┘
│  - library   │
│  - interface │       ┌──────────────────┐
│  - currentUser│──────>│  User            │
│  - currentTab │       │  - playlists[]   │
└──────┬───────┘       │  - likedSongs    │
       │               │  - sessionHistory│
       │               └──────────────────┘
       │
       │               ┌──────────────────┐
       ├──────────────>│  Interface       │
       │               │  (renderizado)   │
       │               └──────────────────┘
       │
       │               ┌──────────────────┐
       └──────────────>│  MusicLibrary    │
                       │  - allSongs (DLL)│
                       │  - queue (CDLL)  │
                       │  - indexBySource │
                       │  - indexByArtist │
                       │  - indexByGenre  │
                       │  - indexByName   │
                       │  - indexByDuration│
                       │  - indexByPlayCount│
                       │  - artists[]     │
                       │  - albums[]     │
                       └──────────────────┘
```

---

## 13. Conclusiones

Echo-MP3 demuestra la aplicación práctica de las siguientes estructuras de datos y algoritmos en un sistema funcional:

1. **Listas doblemente enlazadas** — Almacenamiento secuencial de canciones con inserción/eliminación eficiente.
2. **Listas circulares** — Cola de reproducción con navegación cíclica.
3. **Pilas** — Historial de sesión (LIFO).
4. **Tablas hash** — Índices de acceso rápido por fuente, artista y género (`O(1)` promedio).
5. **Árboles rojo-negro** — Búsqueda por prefijo de nombre (`O(log n)`) con `prefixSearch`.
6. **Árboles AVL** — Índices por duración y reproducciones, consultas por rango.
7. **Grafos** — Motor de recomendación basado en similitud entre canciones.
8. **QuickSort** — Ordenamiento de canciones por nombre y recomendaciones por puntaje.
9. **MergeSort** — Ordenamiento estable por duración.
10. **HeapSort** — Ordenamiento de playlists por cantidad de canciones.

El proyecto integra estas estructuras en un producto completo con interfaz gráfica en terminal, reproducción de audio real, persistencia de datos y experiencia de usuario pulida, demostrando que las estructuras de datos clásicas siguen siendo fundamentales para construir software funcional y eficiente.

---

*Documentación generada para el proyecto Echo-MP3 — UPC, Estructuras de Datos, Ciclo 3*
