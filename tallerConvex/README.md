## Integrantes: Jefferson Gutierrez - Juan Joya

# tallerConvex — Convex Hull con Graham Scan + OpenCV

Proyecto en C++ que detecta y clasifica polígonos en tiempo real (cámara) o desde una imagen, calculando el Convex Hull matemáticamente con el algoritmo **Graham Scan**, sin usar `cv::convexHull()` de OpenCV.

---

## Requisitos

| Herramienta | Versión recomendada |
|-------------|-------------------|
| Ubuntu/Debian Linux | 20.04 o superior |
| C++ | Estándar 17 |
| CMake | 3.10 o superior |
| OpenCV | 4.x |
| Zenity | cualquier versión estable |

Para instalar las dependencias ejecuta: `sudo apt update && sudo apt install cmake g++ libopencv-dev zenity`

---

## Estructura del proyecto

    tallerConvex/
    ├── build/              <- carpeta de compilación (generada por CMake)
    ├── include/
    │   └── converter.h     <- declaraciones de funciones y struct Point2D
    ├── src/
    │   └── converter.cpp   <- implementación: Graham Scan, preprocesamiento, clasificación
    ├── CMakeLists.txt      <- configuración de compilación con CMake
    └── main.cpp            <- punto de entrada, menú de modos

---

## Compilación

Desde la raíz del proyecto ejecuta los siguientes comandos en orden:

    mkdir -p build
    cd build
    cmake ..
    make
    ./convexhull_app

> Solo repite `cmake ..` si modificas `CMakeLists.txt`. Para cambios en código fuente, con `make` es suficiente.

---

## Uso

Al ejecutar el programa aparece un menú con dos opciones:

- **Opción 1:** Abre la cámara del computador y procesa cada frame en tiempo real. Presiona `q` para salir.
- **Opción 2:** Abre un explorador gráfico (Zenity) para seleccionar una imagen `.jpg`, `.png`, `.bmp` o `.jpeg`. Presiona cualquier tecla para cerrar.

---

## ¿Qué hace el programa?

Detecta contornos en la imagen o frame de video, calcula el **Convex Hull** de cada figura encontrada usando matemática pura, clasifica la figura según sus vértices y la dibuja con una etiqueta de color.

### Pipeline de procesamiento

1. **Escala de grises** — `cv::cvtColor` BGR a GRAY
2. **Filtro Gaussiano** — `cv::GaussianBlur` con kernel 7x7 para reducir ruido
3. **Detección de bordes** — `cv::Canny` con umbrales 30/100
4. **Morfología** — dilatación con kernel elíptico 5x5 (x2) + `MORPH_CLOSE` para cerrar contornos
5. **Extracción de contornos** — `cv::findContours` solo contornos externos
6. **Graham Scan** — cálculo matemático del Convex Hull (ver sección abajo)
7. **Clasificación** — identificación de la figura geométrica
8. **Visualización** — dibujo del hull y etiqueta sobre el frame

---

## Algoritmo: Graham Scan — O(n log n)

El Convex Hull se calcula **sin usar `cv::convexHull()`**, implementando Graham Scan en puro C++:

1. **Pivot** — Se encuentra el punto con coordenada Y mínima (el más bajo). En empate, se toma el de X mínima.
2. **Ordenar por ángulo polar** — El resto de puntos se ordenan por ángulo respecto al pivot usando el producto cruzado como comparador.
3. **Construir pila** — Se recorre la lista ordenada apilando cada punto candidato.
4. **Descartar giros a la derecha** — Si el producto cruzado de los últimos tres puntos es <= 0 (giro horario), el punto medio se descarta.
5. **Hull completo** — Lo que queda en la pila es el polígono convexo final.

**Criterio matemático (producto cruzado):**

    cross(O, A, B) = (Ax - Ox)(By - Oy) - (Ay - Oy)(Bx - Ox)
    - Resultado > 0  → giro antihorario ✅ se conserva
    - Resultado <= 0 → giro horario     ❌ se descarta

---

## Clasificación de figuras

La función `classifyShape()` identifica la figura usando dos criterios:

| Criterio | Descripción |
|----------|-------------|
| **Circularidad** | `C = (4π × área) / perímetro²`. Si C > 0.80 es círculo |
| **Vértices del hull** | 3=triángulo, 4=cuadrado/rectángulo, 5=pentágono, 6=hexágono, etc. |

Para distinguir **cuadrado** de **rectángulo** se calcula el ratio ancho/alto del bounding box: si está entre 0.85 y 1.15 es cuadrado.

| Figura | Color del hull |
|--------|---------------|
| Círculo | Verde |
| Triángulo | Naranja |
| Cuadrado | Azul |
| Rectángulo | Cian |
| Pentágono | Amarillo |
| Hexágono | Magenta |
| Otros polígonos | Gris |

---

## Archivos principales

| Archivo | Rol |
|---------|-----|
| `main.cpp` | Menú, modo cámara (`runCamera`) y modo imagen (`runImage`) |
| `src/converter.cpp` | Graham Scan, preprocesamiento, clasificación, Zenity |
| `include/converter.h` | Declaraciones públicas y struct `Point2D` |
| `CMakeLists.txt` | Configuración de CMake y enlace con OpenCV |

---

## Notas

- El programa filtra contornos con área menor a 800 px² para evitar ruido.
- Zenity debe estar instalado para usar el modo explorador de archivos.
- La advertencia de GStreamer al abrir la cámara es normal en algunos sistemas y no afecta el funcionamiento.
