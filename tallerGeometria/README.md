# Taller Geometría — Procesamiento de Imágenes

Este proyecto es un programa interactivo escrito en **C++**, el cual permite explorar conceptos de geometría computacional directamente sobre imágenes reales. Se ejecuta en la terminal, se le pasa una foto, y permite dibujar líneas, polígonos, círculos, aplicar transformaciones matemáticas y analizar regiones de la imagen, todo construido desde cero sin depender de las funciones "automáticas" de OpenCV para dibujar o transformar.

---

## ¿Qué se necesita para poder usarlo?

Antes de compilar o ejecutar el proyecto es necesario tener instalado en el computador:

- **C++ 17 o superior** — el lenguaje en que está escrito el programa.
- **CMake 3.10 o superior** — la herramienta que organiza y dirige la compilación.
- **OpenCV** — una librería que el programa usa para abrir imágenes y mostrarlas en pantalla (aunque los algoritmos de dibujo están implementados a mano).

En macOS con Homebrew es posible instalar lo que falta así:

```bash
brew install cmake opencv
```

---

## Cómo compilar el proyecto

Se abre la terminal, se navega hasta la carpeta del proyecto y se ejecutan los siguientes comandos:

```bash
cd tallerGeometria
cmake -B build -S .
cmake --build build
```

Esto crea la carpeta `build/` y dentro de ella el ejecutable llamado `geometria`.

---

## Cómo ejecutar el programa

```bash
cd tallerGeometria/build
./geometria
```

El programa solicitará la **ruta de la imagen** con la que se desea trabajar. Se puede usar cualquier imagen `.jpeg` o `.png` que esté disponible en el computador, por ejemplo:

```
Ruta de la imagen: ../imagenprueba.jpeg
```

El `../` significa "sube un nivel desde donde se encuentra" (en este caso sale de `build/` y entra a `tallerGeometria/`).

También es posible escribir la ruta completa:

```
Ruta de la imagen: /Users/tu_usuario/Desktop/imagenprueba.jpeg
```

---

## Estructura del proyecto

Así se ven las carpetas y archivos al abrir el proyecto:

```
tallerGeometria/
│
├── CMakeLists.txt          ← Le dice a CMake cómo compilar todo
├── main.cpp                ← El punto de entrada: carga la imagen y muestra el menú principal
│
├── include/
│   └── geometria.h         ← Definiciones compartidas (structs, funciones utilitarias)
│
├── src/
│   ├── puntos_vectores.cpp ← Paso 1: operaciones con puntos y vectores 2D
│   ├── lineas.cpp          ← Paso 2: dibujo de líneas y detección de bordes
│   ├── poligonos.cpp       ← Paso 3: polígonos, área, perímetro y relleno
│   ├── circulos.cpp        ← Paso 4: círculos y elipses
│   ├── transformaciones.cpp← Paso 5: traslación, rotación y escalado con matrices
│   └── procesamiento.cpp   ← Paso 6: ROI, transformación local y Ray Casting
│
├── build/                  ← Se genera al compilar (no modificar a mano)
│   └── geometria           ← El ejecutable final
│
└── imagenprueba.jpeg       ← Imagen de ejemplo (puede ser cualquier .jpeg o .png)
```

---

## Descripción de cada archivo

### `CMakeLists.txt`

Es como la "receta de construcción" del proyecto. Indica a CMake qué archivos compilar, qué versión de C++ usar (la 17) y que necesita la librería OpenCV. Sin este archivo no sería posible compilar el proyecto de manera organizada.

---

### `include/geometria.h`

Este archivo es el "cerebro compartido" del proyecto. Define las piezas que todos los demás archivos usan:

**`struct Vec2D`** — Representa un vector o punto en 2 dimensiones con coordenadas `x` e `y`. Tiene operaciones incorporadas:
- Suma (`+`) y resta (`-`) de vectores
- Multiplicación por un número escalar (`*`)
- **Producto punto** (`dot`): mide qué tan parecidas son las direcciones de dos vectores
- **Producto cruzado** (`cross`): indica la orientación relativa entre dos vectores
- **Magnitud**: la longitud del vector, calculada con el teorema de Pitágoras: `sqrt(x² + y²)`

**`Mat3`** — Es simplemente un arreglo de 3×3 números decimales que representa una **matriz homogénea**, la herramienta matemática usada para las transformaciones geométricas.

**`setPixel`** — Una función pequeña que pinta un solo pixel en la imagen con un color dado. Antes de pintarlo verifica que el pixel sí esté dentro de los límites de la imagen para no causar errores.

**`mostrarYEsperar`** — Abre una ventana con la imagen resultante y espera a que el usuario presione cualquier tecla para cerrarla.

---

### `main.cpp`

Es el punto de entrada del programa. Hace tres cosas:
1. Solicita la ruta de la imagen, la carga con OpenCV y verifica que se haya abierto correctamente (si no, muestra un error y cierra el programa).
2. Muestra el **menú principal** en un ciclo.
3. Llama a la función correspondiente a cada paso según la opción elegida.

---

### `src/puntos_vectores.cpp` — Paso 1

Se encarga de las operaciones más básicas de geometría: puntos y vectores.

**Opciones disponibles:**

**[1] Consultar color de un pixel**
El usuario ingresa las coordenadas `(x, y)` de un pixel y el programa muestra exactamente qué color tiene en formato **BGR** (Azul, Verde, Rojo). Por ejemplo, `B=0, G=0, R=255` significa rojo puro. El origen `(0,0)` está en la esquina **superior izquierda** de la imagen.

**[2] Desplazar imagen con un vector**
El usuario ingresa un vector `(dx, dy)` y cada pixel de la imagen se mueve a su nueva posición `(x+dx, y+dy)`. Los píxeles que salen del borde de la imagen desaparecen y la zona que queda vacía se ve negra. El desplazamiento se implementa **píxel a píxel** sin usar funciones de OpenCV.

**[3] Marcar un punto en la imagen**
El usuario ingresa una posición `(x, y)` y un radio `r`, y el programa dibuja un disco rojo en ese punto. El disco se construye recorriendo el cuadrado de lado `2r` alrededor del centro y pintando solo los píxeles que cumplen `dx² + dy² ≤ r²` (la ecuación del círculo).

**[4] Operaciones entre vectores**
El usuario ingresa dos vectores `A` y `B` y el programa calcula:
- `A + B` y `A - B`: suma y resta componente a componente
- `A * 2`: escala el vector al doble de su tamaño
- `A · B` (dot product): si el resultado es 0, los vectores son perpendiculares
- `A × B` (cross product): si es positivo, B está a la izquierda de A; si es negativo, a la derecha

---

### `src/lineas.cpp` — Paso 2

Trabaja con el trazado de líneas rectas y la detección de bordes en la imagen.

**Opciones disponibles:**

**[1] Dibujar línea entre dos puntos (Bresenham)**
El usuario ingresa el punto inicial `(x0, y0)`, el punto final `(x1, y1)` y un grosor en píxeles. El programa usa el **algoritmo de Bresenham** (inventado en 1965) para dibujar la línea usando solo sumas y restas de números enteros, sin necesitar decimales ni trigonometría. La idea es que en cada paso se decide si el siguiente píxel va recto o en diagonal, acumulando el "error" para que la línea se vea lo más recta posible. Antes de mostrar la imagen se imprime información del segmento: longitud, pendiente y el vector `AB`.

**[2] Detectar bordes con Sobel**
El usuario ingresa un valor de **umbral** (número entre 0 y 255 aproximadamente) y el programa detecta los bordes de la imagen usando el **filtro de Sobel**, implementado manualmente siguiendo estos pasos:
1. Se convierte la imagen a escala de grises calculando `0.299×R + 0.587×G + 0.114×B` por cada píxel (fórmula estándar de luminancia).
2. Se aplican dos "kernels" o plantillas 3×3 sobre cada píxel para calcular el gradiente horizontal (Gx) y vertical (Gy).
3. Se calcula la magnitud total del gradiente: `sqrt(Gx² + Gy²)`.
4. Si esa magnitud supera el umbral, el píxel se marca blanco (es un borde). Si no, queda negro.

Un umbral bajo detecta muchos bordes incluyendo ruido; uno alto solo muestra los bordes más definidos.

**[3] Dibujar múltiples líneas**
El usuario ingresa cuántas líneas desea y las coordenadas de cada una. Todas se dibujan sobre la misma imagen base en colores distintos (verde, rojo, azul, cian, magenta) para diferenciarlas.

---

### `src/poligonos.cpp` — Paso 3

Trabaja con figuras cerradas de varios vértices (polígonos).

**Opciones disponibles:**

**[1] Dibujar contorno del polígono**
El usuario ingresa la cantidad de vértices y sus coordenadas `(x, y)` en orden. El programa conecta cada par de vértices consecutivos con una línea Bresenham y cierra la figura al final. Los vértices se marcan con cuadrados rojos. El contorno se dibuja en color amarillo (cian en escala BGR).

**[2] Rellenar polígono con Scanline**
Además de dibujar el contorno, rellena el interior del polígono usando el **algoritmo Scanline Fill**:
1. Se recorre cada fila horizontal de píxeles (cada "scanline") entre el vértice más alto y el más bajo del polígono.
2. Para cada fila, se calcula en qué columnas `x` la línea horizontal cruza los lados del polígono.
3. Se ordenan esas columnas de menor a mayor y se pintan los píxeles entre el primero y el segundo cruce, entre el tercero y el cuarto, y así sucesivamente (regla de paridad).

Después de dibujar se muestra el área (fórmula de Shoelace) y el perímetro del polígono.

**[3] Calcular área y perímetro**
Solo calcula números, no modifica la imagen:
- **Área** con la **fórmula de Shoelace** (también llamada fórmula de Gauss): `A = 0.5 × |Σ(xi×yj − xj×yi)|`, donde se suman los productos cruzados de vértices consecutivos.
- **Perímetro**: suma de las distancias euclidianas entre cada par de vértices consecutivos.
- **Circularidad**: `C = (4π × A) / P²`. Vale `1.0` si la figura es un círculo perfecto y se acerca a `0` si la figura es muy irregular o alargada.

---

### `src/circulos.cpp` — Paso 4

Dibuja figuras curvas: círculos y elipses.

**Opciones disponibles:**

**[1] Dibujar círculo (Bresenham)**
El usuario ingresa el centro `(cx, cy)` y el radio `r`. El programa usa el **algoritmo de Bresenham para círculos**, que aprovecha que un círculo tiene simetría de 8 octantes: solo calcula los píxeles del primer octante (el arco de 0° a 45°) y los refleja automáticamente a los otros 7, reduciendo el trabajo a la mitad de la mitad. El centro se marca en rojo y se muestran el área y el perímetro calculados con las fórmulas `π×r²` y `2×π×r`.

**[2] Dibujar elipse (Midpoint)**
El usuario ingresa el centro y dos radios: `rx` (semieje horizontal) y `ry` (semieje vertical). Se usa el **algoritmo Midpoint para elipses**, que divide el trazado en dos regiones según cómo varía la pendiente de la curva:
- Región 1: cuando la pendiente es menor que 1 (la curva es más "ancha" que "alta"), avanza principalmente en X.
- Región 2: cuando la pendiente supera 1 (la curva es más "alta" que "ancha"), avanza principalmente en Y.

El área aproximada de la elipse es `π × rx × ry`.

**[3] Dibujar círculos concéntricos**
El usuario ingresa el centro, el radio máximo y la cantidad de círculos deseados. El programa distribuye los radios uniformemente entre `rmax/n` y `rmax` y dibuja cada círculo en un color diferente (verde, azul, rojo, cian, magenta, amarillo), todos compartiendo el mismo centro.

---

### `src/transformaciones.cpp` — Paso 5

Aplica transformaciones geométricas a toda la imagen usando **matrices homogéneas 3×3**. Una matriz homogénea permite expresar traslaciones, rotaciones y escalados con la misma operación matemática (multiplicación de matrices), lo que hace posible combinarlas fácilmente.

> **Importante:** cada transformación en este paso **modifica la imagen permanentemente** durante la sesión. Si se aplica una rotación y luego una traslación, la segunda se aplica sobre la imagen ya rotada.

Todas las transformaciones usan **mapeo inverso**: en lugar de preguntar "¿a dónde va este píxel?", se pregunta "¿de dónde viene el píxel que se quiere pintar en (x, y)?". Esto evita huecos en la imagen resultante.

**Opciones disponibles:**

**[1] Traslación**
El usuario ingresa `tx` (desplazamiento horizontal) y `ty` (desplazamiento vertical). La matriz aplicada es:

```
| 1  0  tx |   | x |   | x + tx |
| 0  1  ty | × | y | = | y + ty |
| 0  0   1 |   | 1 |   |   1    |
```

**[2] Rotación alrededor del centro**
El usuario ingresa el ángulo en grados. El programa calcula una matriz compuesta `M = T(centro) × R(θ) × T(-centro)` que primero mueve el centro de la imagen al origen, rota, y devuelve el centro a su lugar. La matriz de rotación es:

```
| cos(θ)  -sin(θ)  0 |
| sin(θ)   cos(θ)  0 |
|   0        0     1 |
```

Ángulo positivo → sentido antihorario. Ángulo negativo → sentido horario.

**[3] Escalado**
El usuario ingresa los factores `sx` (horizontal) y `sy` (vertical). La matriz es:

```
| sx   0   0 |
|  0  sy   0 |
|  0   0   1 |
```

- `sx = sy` → escala proporcional (la imagen se agranda o reduce sin deformarse)
- `sx ≠ sy` → escala no proporcional (la imagen se estira o aplana)
- Valores menores a 1 reducen; valores mayores a 1 amplían.

**[4] Composición (Traslación + Rotación + Escalado)**
El usuario ingresa todos los parámetros a la vez. El programa combina las tres matrices en una sola: `M = T × R × S`. El orden de aplicación es: primero escala, luego rota, luego traslada. La matriz compuesta se imprime en pantalla antes de aplicarla.

---

### `src/procesamiento.cpp` — Paso 6

Combina geometría y procesamiento de imagen para trabajar sobre **regiones específicas** definidas por polígonos.

**Opciones disponibles:**

**[1] Recortar ROI con polígono (máscara)**
ROI significa *Region Of Interest* (Región de Interés). El usuario ingresa los vértices de un polígono y el programa construye una máscara: recorre cada píxel de la imagen y usa el **algoritmo Ray Casting** para decidir si está dentro o fuera del polígono. Solo los píxeles interiores se copian al resultado; el exterior queda completamente negro.

**[2] Aplicar rotación solo dentro de una región**
El usuario ingresa los vértices de un polígono y un ángulo. El programa rota únicamente los píxeles que están dentro del polígono, usando el **centroide** del polígono (el promedio de todos los vértices) como punto de pivote. Los píxeles del exterior no se modifican. El contorno del polígono se dibuja en amarillo como referencia.

**[3] Detectar si un punto está dentro de una figura (Ray Casting)**
El **algoritmo Ray Casting** funciona así: desde el punto que se desea verificar se lanza un rayo imaginario horizontal hacia la derecha. Se cuenta cuántos lados del polígono cruza ese rayo:
- Si el número de cruces es **impar** → el punto está **DENTRO** (se marca en verde en la imagen).
- Si el número de cruces es **par** → el punto está **FUERA** (se marca en rojo en la imagen).

Este algoritmo funciona con cualquier tipo de polígono, tanto convexo (sin "huecos" en los bordes) como cóncavo (con entrantes o formas irregulares).

---

## Flujo completo de uso (ejemplo paso a paso)

```
1.  Se ejecuta ./geometria
2.  Se escribe la ruta: ../imagenprueba.jpeg
3.  El programa confirma las dimensiones de la imagen cargada
4.  Se ve el menú principal y se elige, por ejemplo, [4] Circulos y Elipses
5.  Dentro del submenú se elige [1] Dibujar circulo
6.  El programa explica el algoritmo de Bresenham
7.  Se ingresa centro x: 300, centro y: 200, radio: 80
8.  Se muestra el área y perímetro calculados
9.  Se abre una ventana con la imagen y el círculo dibujado
10. Se presiona cualquier tecla y se vuelve al submenú
11. Se elige [0] para volver al menú principal
12. Se elige [0] para salir del programa
```

---

## Tecnologías usadas

| Tecnología | Para qué se usa |
|---|---|
| **C++ 17** | Lenguaje principal del proyecto |
| **CMake** | Sistema de compilación multiplataforma |
| **OpenCV** | Cargar imágenes desde disco y mostrarlas en ventana |

> OpenCV **solo** se usa para abrir la imagen y mostrarla. Todos los algoritmos de dibujo, relleno, transformación y análisis están implementados desde cero en el código del proyecto.
