# 📚 Documentación del Código — Echo MP3

> **10 funciones documentadas** en **4 archivos** del proyecto.
> Cada entrada incluye: **Importancia**, **BigO** y **Explicación detallada**.

---

# 📁 `main.cpp`

## `int main()`

**📍 Líneas:** 5–26

### Importancia

Punto de entrada único del ejecutable. Configura el entorno de consola necesario para que la interfaz gráfica ASCII funcione correctamente y luego transfiere el control total al controlador de la aplicación. Sin esta función el sistema no puede iniciarse.

### BigO

**`O(1)`** — Tiempo constante en todas las operaciones de inicialización. La función termina únicamente cuando `AppController::run()` retorna, por lo que la complejidad real del programa depende enteramente de lo que ocurra dentro del bucle principal. Espacio adicional `O(1)`; no se asignan estructuras dinámicas en este ámbito.

### Explicación detallada

1. Oculta el cursor parpadeante de la consola para mejorar la presentación visual.
2. Establece el título de la ventana como `"ECHO"`.
3. Redimensiona la ventana a 200 columnas × 60 filas para alojar toda la interfaz.
4. Crea una instancia de `AppController`, cuyo constructor carga datos del disco y renderiza el menú principal.
5. Llama a `AppController::run()`, que entra en un bucle infinito de lectura de teclado y actualización de pantalla. `main()` solo retorna cuando la app se cierra.
6. Devuelve 0 al sistema operativo indicando finalización exitosa.

---

# 📁 `FileManager.h`

## `static bool loadSongs(MusicLibrary& lib, const string& filename)`

**📍 Líneas:** 32–75

### Importancia

Función fundamental de persistencia que restaura el estado completo de la biblioteca musical desde el disco. Es el único punto donde los datos del archivo `"Data/songsList.txt"` se convierten en objetos `Song` en memoria. Sin esta función la aplicación no tendría canciones que reproducir.

### BigO

**`O(L × F + T × (N + A + B))`**

Donde **L** es la cantidad de líneas del archivo, **F** es la cantidad de campos por línea (~7: nombre, autor, fuente, duración, género, playcount, liked), y **T** es la cantidad de canciones insertadas exitosamente. Por cada canción se invoca `addSongToLibrary()` que tiene su propio costo `O(N + A + B)` como se detalló previamente. El parseo por línea involucra operaciones `O(F)` para los `getline`, `trim`, `stof`, `stoi` y comparaciones booleanas.

**Espacio:** `O(T)` para la biblioteca en memoria, más `O(L)` temporal para el buffer de lectura línea por línea.

### Explicación detallada

1. Abre el archivo `songsList.txt` en modo lectura. Si no se puede abrir (archivo faltante o sin permisos), retorna `false` y la biblioteca queda vacía.
2. Llama a `lib.clear()` para resetear la biblioteca, eliminando cualquier dato previo y todos los índices. Esto asegura que una recarga no duplique canciones.
3. Itera sobre cada línea del archivo usando `getline()`:
   - **a)** Elimina el caracter de retorno de carro `\r` al final si existe (común en archivos de Windows).
   - **b)** Aplica `trim()` para eliminar espacios en blanco al inicio y final.
   - **c)** Ignora líneas vacías.
4. Para cada línea no vacía, usa un `stringstream` y extrae los 7 campos separados por `|` mediante `getline(ss, campo, '|')`:
   - `name`, `author`, `source`, `durationStr`, `genre`, `playCountStr`, `likedStr`.
5. Crea una matriz dummy de 25×25 para la carátula (placeholder, no se usa).
6. Convierte los campos numéricos:
   - `durationStr` → `float` con `stof()`. Si falla, usa 0.0.
   - `playCountStr` → `int` con `stoi()`. Si falla, usa 0.
   - `likedStr` → `bool`: `true` si es `"1"`, `"true"` o `"True"`.
7. Construye un objeto `Song(name, author, source, duration, genre, dummyImg, playCount, liked)` y lo inserta en la biblioteca mediante `addSongToLibrary()`, que se encarga de indexarlo en todas las estructuras auxiliares.
8. Si alguna línea está mal formada (exception en `stof`/`stoi`), el `catch(...)` la ignora silenciosamente y continúa con la siguiente línea, garantizando que el resto del archivo se cargue correctamente.
9. Cierra el archivo y retorna `true` indicando éxito.

---

## `static bool generateDataset(int multiplier, const string& outputFilename, const string& backupFilename)`

**📍 Líneas:** 208–262

### Importancia

Generador de datasets de prueba que expande las canciones originales del archivo `"Data/songsList.txt"` en un conjunto múltiplo con metadatos variados (género, playcount, liked) y sufijos en el nombre. Es la única herramienta para crear datos de prueba masivos sin necesidad de archivos de audio adicionales, permitiendo evaluar el rendimiento de los algoritmos de ordenamiento (`MergeSort`, `QuickSort`, `HeapSort`) y de las estructuras de datos (`HashTable`, `RedBlackTree`, `AVLTree`) con cargas de trabajo realistas. Sin esta función, las pruebas de rendimiento estarían limitadas a las 60 canciones originales.

### BigO

**`O(B × M + T × G)`**

Donde **B** es la cantidad de canciones en el archivo backup (~60), **M** es el multiplicador (default 5), y **T** es el total de líneas generadas (`B × M`). Por cada línea de salida se ejecutan en `O(1)`: selección aleatoria de género, consulta de sufijo, generación de playCount y liked, y escritura formateada. El parseo de cada línea plantilla es `O(F)` donde **F** son los 7 campos separados por `|`.

**Espacio:** `O(T + B)` en memoria: `O(B)` para almacenar las líneas plantilla y `O(1)` adicional para el buffer de escritura línea por línea. En disco se escriben **T** líneas (el nuevo `songsList.txt`) y se preserva el backup original con **B** líneas.

### Explicación detallada

1. Verifica si el archivo de backup `"songsList_backup.txt"` existe en disco.
   - **Si NO existe:** es la primera ejecución del programa tras agregar el generador. Abre el `songsList.txt` actual, copia cada línea intacta al backup, y cierra ambos archivos. Esto preserva el dataset original.
   - **Si SÍ existe:** salta la creación del backup, pues ya fue respaldado en una ejecución anterior. El backup jamás se sobrescribe.
2. Abre el archivo de backup en modo lectura como plantilla base. Si no se puede abrir (archivo faltante o sin permisos), retorna `false`.
3. Inicializa la semilla del generador aleatorio con `srand(time(nullptr))` para que cada ejecución produzca datos distintos.
4. Define un arreglo de estructuras `GenreSuffix` que mapea cada uno de los 16 géneros a un sufijo textual (ej: `"Hip-Hop"` → `" - Remix"`, `"Rock"` → `" - Rock Version"`). Esto asegura que el nombre mostrado en pantalla refleje el género asignado.
5. Itera sobre cada línea del backup usando `getline()`:
   - **a)** Elimina el caracter de retorno de carro `\r` al final si existe.
   - **b)** Aplica `trim()` para eliminar espacios en blanco al inicio y final.
   - **c)** Ignora líneas vacías.
   - **d)** Agrega la línea limpia al vector `templateLines`.
6. Cierra el archivo de backup y abre (o sobrescribe) el archivo de salida `"Data/songsList.txt"` en modo escritura.
7. Para cada línea plantilla en `templateLines`:
   - **a)** Usa un `stringstream` para extraer los 7 campos separados por `|`:
     `name`, `author`, `source`, `durationStr`, `genre`, `playCountStr`, `likedStr`.
   - **b)** Repite `multiplier` veces (default 5):
     - **i)** Selecciona un índice aleatorio entre 0 y 15 (16 géneros).
     - **ii)** Obtiene el género y el sufijo correspondiente desde `genreMap`.
     - **iii)** Genera playCount aleatorio entre 0 y 500 (`rand() % 501`).
     - **iv)** Genera liked aleatorio (0 o 1) mediante `rand() % 2`.
     - **v)** Escribe la línea de salida en el formato:
       `nombre + sufijo | autor | source | duración | género | playCount | liked`
8. Cierra el archivo de salida y retorna `true` indicando éxito.

---

## `static bool loadLibrary(MusicLibrary& lib)`

**📍 Líneas:** 352–375

### Importancia

Función de alto nivel que orquesta la restauración completa del estado persistido de la aplicación. Es llamada desde `AppController::AppController()` al iniciar el programa. Sin esta función la biblioteca musical arrancaría vacía cada vez.

### BigO

**`O(S + P + C)`**

Donde **S** es la cantidad de canciones en el archivo `songsList.txt`, **P** es la cantidad de playlists en `playlists.txt`, y **C** es la cantidad total de referencias a canciones dentro de todas las playlists. Cada canción cargada invoca `addSongToLibrary()` que a su vez inserta en todos los índices auxiliares (`O(N + A + B)` por canción).

**Espacio:** `O(N)` adicional para almacenar en memoria toda la biblioteca resultante.

### Explicación detallada

1. Llama a `loadSongs()` que abre `"Data/songsList.txt"`, lee línea por línea, parsea los campos separados por `|`, construye objetos `Song` y los inserta en la biblioteca mediante `addSongToLibrary()`.
2. Llama a `loadPlaylists()` que abre `"Data/playlists.txt"`, lee nombres de playlists seguidos de referencias a canciones (rutas de archivo) hasta encontrar la marca `"[ENDPLAYLIST]"`, y las rellena buscando cada canción en la biblioteca ya cargada mediante `findSongBySource()`.
3. Retorna `true` solo si ambas operaciones fueron exitosas; si falla alguna, la biblioteca puede quedar en un estado parcialmente cargado.

---

# 📁 `AppController.cpp`

## `void performSearch()`

**📍 Líneas:** 305–340

### Importancia

Motor de búsqueda interactiva que se ejecuta cada vez que el usuario escribe o borra un caracter en la pestaña Search. Es la única vía para localizar canciones por nombre o artista cuando la biblioteca es grande. Su respuesta en vivo mejora significativamente la experiencia de usuario.

### BigO

**`O(K + N)` en el peor caso**

Donde **K** es la longitud del query y **N** es la cantidad de canciones en la biblioteca. `searchByNamePrefix()` utiliza el árbol rojo-negro `indexByName` y su operación `prefixSearch()` que recorre el árbol encontrando nodos que coincidan con el prefijo. Si este método no encuentra resultados, se recurre a `searchIncremental()` que recorre toda la lista enlazada (`O(N)`) aplicando `string::find` en cada canción (`O(K × M)` por canción donde **M** es la longitud del nombre/artista). En el peor escenario (sin prefijo y query larga) se acerca a `O(N × K × M)`.

**Espacio:** `O(N)` para almacenar el vector de resultados.

### Explicación detallada

1. Limpia el vector `searchResults` de la búsqueda anterior.
2. Evalúa la longitud del query:
   - **Si está vacío:** devuelve todas las canciones de la biblioteca.
   - **Si tiene 3 o más caracteres:** intenta primero `searchByNamePrefix()` que busca en el árbol rojo-negro `indexByName` usando el prefijo del query. Esto es eficiente porque el árbol mantiene las claves ordenadas y puede encontrar el rango de nodos que comienzan con el prefijo sin recorrer toda la estructura. Si encuentra al menos un resultado, lo usa. Si no encuentra ninguno, cae en `searchIncremental()` que recorre la lista enlazada completa y verifica si el query es substring del nombre o del artista (en minúsculas).
   - **Si tiene 1 o 2 caracteres:** directamente usa `searchIncremental()`.
3. Reinicia los índices de selección y scroll (`searchSelectedIndex = 0`, `searchTopIndex = 0`) para que la interfaz muestre desde el primer resultado.
4. El vector de resultados se mantiene en memoria en `AppController` y se actualiza con cada tecla, permitiendo que la interfaz lo renderice sin recalcular la búsqueda.

---

## `void handleInput()`

**📍 Líneas:** 424–467

### Importancia

Es el centro de comando de toda la interacción usuario-sistema. Procesa cada tecla presionada y decide qué acción ejecutar: navegar entre pestañas, reproducir canciones, pausar, dar like, gestionar playlists, buscar, etc. Sin esta función el usuario no podría controlar el reproductor.

### BigO

**`O(1)` en el caso promedio, `O(N)` en el peor caso**

La mayoría de las teclas (flechas, Enter, Espacio, Esc, `+`, `-`, `L`) se atienden en tiempo constante `O(1)`. Sin embargo, cuando el usuario escribe en la pestaña de búsqueda, cada tecla dispara `performSearch()` que recorre toda la biblioteca con costo `O(N)`. Las teclas que guardan playlists también arrastran `O(P)` donde **P** es la cantidad de playlists a serializar. El peor caso global se da durante búsqueda activa + guardado, dando `O(N + P)`.

**Espacio:** `O(1)` adicional, solo utiliza variables locales de tipo entero o puntero.

### Explicación detallada

1. Verifica con `_kbhit()` si hay una tecla disponible en el buffer de entrada. Si no la hay, retorna inmediatamente para no bloquear el bucle principal.
2. Lee la tecla con `_getch()` y la evalúa mediante una serie de condicionales:
   - **a)** `'+'` → Llama a `addSelectedSongToPlaylist()` para agregar la canción actual a una playlist, creándola si no existe.
   - **b)** `'-'` → Llama a `removeSelectedSongFromPlaylist()` para quitar la canción de una playlist especificada.
   - **c)** `Esc (27)` → Si está dentro de una playlist, retrocede a la vista de listas. Si ya está en el nivel superior, guarda las playlists y cierra la aplicación con `exit(0)`.
   - **d)** `Enter (13)` → Según la pestaña activa, reproduce la canción seleccionada, entra a una playlist o ejecuta la búsqueda.
   - **e)** `Espacio (32)` → Alterna entre pausa y reproducción del audio actual.
   - **f)** `'L' (108)` → Da o quita like a la canción seleccionada en cualquier vista.
   - **g)** `'A'/'S'/'D'` → Ordenan la biblioteca por duración ascendente/descendente/desactivado.
   - **h)** `'R'/'T'` → Ordenan las recomendaciones por score ascendente/descendente.
   - **i)** `'O'/'P'` → Navegan entre playlists anterior/siguiente.
3. Si la pestaña activa es `SEARCH`, las teclas imprimibles se acumulan en `searchQuery`, y cada cambio ejecuta `performSearch()` para actualizar resultados en vivo. Backspace elimina el último caracter.
4. Las flechas arriba/abajo (`224` seguido de `72/80`) llaman a `moveUp()`/`moveDown()` según la pestaña activa, actualizando el índice seleccionado y el scroll.
5. Después de cada acción que modifica la selección o los datos, llama a `render("refresh")` para actualizar solo las filas que cambiaron (renderizado parcial), o `render("swap")` para redibujar la pantalla completa (cambio de pestaña).

---

## `void run()`

**📍 Líneas:** 1211–1235

### Importancia

Contiene el game-loop (bucle principal) de la aplicación. Es el corazón del programa que se ejecuta ininterrumpidamente desde que inicia hasta que el usuario presiona `Esc` para salir. Sin este bucle la aplicación se cerraría inmediatamente.

### BigO

**`O(1)` por iteración**

Cada ciclo ejecuta dos operaciones de costo constante: `handleInput()` que en el caso promedio responde en `O(1)` (solo leer una tecla), y `drawSpectrum()` que redibuja una sección fija de la pantalla con 21 barras de 6 píxeles cada una, también `O(1)` por tener un tamaño constante. El bucle es infinito, por lo que la complejidad total es `O(∞)` hasta que se fuerza la salida con `exit()`.

**Espacio:** `O(1)` adicional.

### Explicación detallada

1. Itera perpetuamente mientras la aplicación esté activa.
2. En cada iteración llama a `handleInput()`, que verifica si hay una tecla disponible con `_kbhit()`. Si no la hay, retorna inmediatamente sin bloqueo, permitiendo que el ciclo continúe fluidamente.
3. Después ejecuta `drawSpectrum()` que dibuja un visualizador de espectro animado en la parte inferior de la pantalla usando ondas seno. El parámetro `!audio.estaPausado()` controla si la animación avanza o se congela.
4. El único modo de salir del bucle es cuando `handleInput()` captura la tecla `Esc` (código 27), que invoca `exit(0)` para finalizar el proceso.

---

# 📁 `MusicLibrary.h`

## `void addSongToLibrary(const Song& song)`

**📍 Líneas:** 167–208

### Importancia

Función nuclear que inserta una canción en la lista principal y la indexa en los **6 sistemas de búsqueda optimizada** (hash por fuente, hash por artista, hash por género, árbol rojo-negro por nombre, AVL por duración, AVL por reproducciones). También actualiza las entidades de artista y álbum. Es la única puerta de entrada para incorporar canciones a la biblioteca.

### BigO

**`O(N + A + B)`**

Donde **N** es la cantidad actual de canciones (inserción al final de la lista enlazada: `O(1)` si hay puntero a la cola, pero puede ser `O(N)` si no), **A** es la cantidad de artistas existentes (búsqueda lineal para encontrar coincidencia) y **B** es la cantidad de álbumes existentes (búsqueda lineal similar). Las inserciones en los índices `HashTable` son `O(1)` promedio y `O(N)` en el peor caso de colisiones. Las inserciones en `RedBlackTree` y `AVLTree` son `O(log M)` donde **M** es la cantidad de elementos en cada árbol.

**Espacio:** `O(1)` adicional, pues la canción ya se almacena en la lista y los índices solo guardan punteros o copias ligeras.

### Explicación detallada

1. Toma una copia de la canción (`normalizedSong`) para no modificar el original.
2. Normaliza el género invocando `normalizeGenre()`, que asigna un género estandarizado (Rock, Pop, Hip Hop, Latin, Electronic, Ballad, Other) según una tabla fija de nombres de canción conocidos. Esto garantiza consistencia en los filtros por género y en el sistema de recomendaciones.
3. Inserta la canción al final de la lista doblemente enlazada `allSongs`.
4. Obtiene un puntero al nodo recién insertado para trabajar con la misma instancia en memoria en todos los índices.
5. Indexa en los 6 sistemas de búsqueda:
   - `indexBySource`: `HashTable` clave=fuente (ruta archivo), valor=puntero a `Song`.
   - `indexByArtist`: `HashTable` clave=nombre artista, valor=vector de punteros.
   - `indexByGenre`: `HashTable` clave=género normalizado, valor=vector de punteros.
   - `indexByName`: `RedBlackTree` clave=nombre canción, valor=copia de `Song`.
   - `indexByDuration`: `AVLTree` clave=duración (float), valor=copia de `Song`.
   - `indexByPlayCount`: `AVLTree` clave=conteo reproducciones (int), valor=copia.
6. Busca si el artista ya existe en el vector `artists`. Si existe, agrega la canción a su lista; si no, crea un nuevo artista y lo agrega al vector.
7. Busca si existe un álbum con el mismo título (género) y mismo artista. Si existe, agrega la canción como pista; si no, crea un nuevo álbum y lo agrega al vector. Esto organiza las canciones en una estructura jerárquica Artista → Álbum → Canción.

---

## `vector<RecommendationItem> getRecommendedSongs()`

**📍 Líneas:** 354–388

### Importancia

Implementa el algoritmo de recomendación del reproductor. Analiza el historial de reproducción de la sesión actual y las canciones marcadas con "Me gusta" para calcular un puntaje de relevancia para cada canción no-likeada. Es la base de la pestaña "Recom." y permite al usuario descubrir música relacionada con sus gustos actuales.

### BigO

**`O(T × (H + L))`**

Donde **T** es la cantidad total de canciones en la biblioteca (sin incluir liked), **H** es el tamaño del historial de sesión (`Stack`) y **L** es la cantidad de canciones liked. Por cada canción candidata se recorre todo el historial (**H**) y todas las liked (**L**) para acumular puntajes.

**Espacio:** `O(T)` para almacenar el vector de recomendaciones resultante, más `O(T)` temporal por las copias de `getAllSongsVector()` y `getLikedSongsVector()`.

### Explicación detallada

1. Obtiene dos vectores auxiliares: todas las canciones (`getAllSongsVector`) y solo las canciones liked (`getLikedSongsVector`).
2. Si no hay canciones en la biblioteca, retorna un vector vacío.
3. Obtiene la cantidad de elementos en el historial de sesión (`sessionHistory`), que es un `Stack` donde cada reproducción agrega un elemento.
4. Itera sobre todas las canciones de la biblioteca. Omite las que ya están liked (no tiene sentido recomendar lo que ya le gusta al usuario).
5. Para cada canción candidata calcula un puntaje inicializado en 0:
   - **a)** Recorre todo el historial de sesión y por cada canción escuchada:
     - Si coincide el artista, suma **+10** puntos.
     - Si ambos tienen géneros conocidos y coinciden, suma **+15** puntos.
   - **b)** Recorre todas las canciones liked y por cada una:
     - Si ambos tienen géneros conocidos y coinciden, suma **+20** puntos.
       *(La coincidencia de género con liked pesa más que con historial)*
6. Agrega la canción y su puntaje como un `RecommendationItem` al vector.
7. Retorna el vector que luego puede ser ordenado por `QuickSort` según el puntaje para mostrar las más relevantes primero.

---

## `vector<Song> searchIncremental(const string& query)`

**📍 Líneas:** 533–569

### Importancia

Implementa la búsqueda por substring (coincidencia parcial) sobre el nombre y el artista de cada canción. Es el método de respaldo cuando la búsqueda por prefijo (que usa el árbol rojo-negro) no encuentra resultados. Permite al usuario encontrar canciones incluso si no recuerda el nombre exacto o escribe una parte del título o del artista.

### BigO

**`O(N × (K + M))`**

Donde **N** es la cantidad de canciones en la biblioteca, **K** es la longitud del query (después de normalizar a minúsculas), y **M** es la longitud promedio de los campos `name` y `author` (también normalizados). Por cada canción se aplica `string::find()` que internamente tiene complejidad `O(K × M)` en el peor caso (algoritmo de búsqueda ingenua sin KMP). Adicionalmente, cada normalización recorre el string completo `O(len)` para convertir a minúsculas.

**Espacio:** `O(N)` para el vector de resultados, más `O(K + M)` temporal para las conversiones a minúsculas de query, name y author en cada iteración.

### Explicación detallada

1. Si el query está vacío, retorna todas las canciones de la biblioteca (equivalente a `getAllSongsVector()`).
2. Convierte el query completo a minúsculas usando `toLowerStr()` para que la búsqueda sea case-insensitive (no distinga mayúsculas/minúsculas).
3. Recorre toda la lista doblemente enlazada `allSongs` con un puntero `curr` desde la cabeza hasta el final (`O(N)`).
4. Para cada nodo:
   - **a)** Obtiene la referencia a la canción (`curr->getValue()`).
   - **b)** Convierte `song.getName()` y `song.getAuthor()` a minúsculas.
   - **c)** Verifica si el query normalizado es substring del nombre normalizado usando `lowerName.find(lowerQuery)`. Si no coincide, verifica también contra el autor con `lowerArtist.find(lowerQuery)`.
   - **d)** Si alguna de las dos verificaciones es positiva (devuelve `!= npos`), agrega la canción al vector de resultados.
5. Retorna el vector con todas las canciones que coinciden parcialmente. La interfaz se encarga de mostrar estos resultados ordenados según el orden de la lista original (por inserción).

---

> **Fin de la documentación**
