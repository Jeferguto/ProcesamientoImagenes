# Integrantes: Jefferson Gutierrez y Juan Joya

# Taller de Filtros - Procesamiento de Imágenes en C++

## Descripción

Este proyecto implementa un conjunto de filtros de procesamiento de imágenes en **C++** usando **OpenCV** únicamente como soporte para:

- carga de imágenes
- visualización de resultados
- captura de cámara
- manejo de matrices `cv::Mat`

La parte principal del taller está desarrollada de forma **matemática/manual**, siguiendo el enfoque trabajado en ejercicios anteriores. Esto significa que operaciones como la conversión a gris, la convolución, el suavizado gaussiano, la magnitud del gradiente y la lógica base de varios detectores de bordes fueron implementadas manualmente, sin usar las funciones de alto nivel de OpenCV para resolver directamente cada filtro.

El sistema permite trabajar de dos formas:

- usando una imagen seleccionada desde el explorador de archivos
- usando la cámara web en tiempo real

Además, el usuario puede escoger entre aplicar un único filtro o visualizar todos los filtros simultáneamente en una cuadrícula.

---

## Filtros implementados

El proyecto incluye los siguientes filtros:

### 1. Gray
Convierte la imagen de color a escala de grises mediante la fórmula:

\[
Gray = 0.114B + 0.587G + 0.299R
\]

La conversión se realiza píxel por píxel.

### 2. LoG (Laplacian of Gaussian)
Primero aplica un suavizado gaussiano manual y luego un operador laplaciano manual. Este filtro permite resaltar cambios bruscos de intensidad reduciendo parcialmente el ruido gracias al suavizado previo.

### 3. Zero Crossing
Detecta cruces por cero en la respuesta del Laplaciano después del suavizado. Este método identifica cambios de signo entre vecinos para localizar bordes.

### 4. Scharr
Implementa manualmente los kernels de Scharr para estimar derivadas en las direcciones horizontal y vertical. Luego calcula la magnitud del gradiente.

### 5. Laplaciano
Aplica un operador de segunda derivada mediante convolución manual con un kernel laplaciano.

### 6. Sobel Magnitude
Calcula manualmente los gradientes \(G_x\) y \(G_y\) usando los kernels de Sobel y luego obtiene la magnitud del gradiente:

\[
M = \sqrt{G_x^2 + G_y^2}
\]

### 7. Canny
Implementa una versión manual del detector de bordes de Canny con las siguientes etapas:

- conversión a gris
- suavizado gaussiano
- gradiente con Sobel
- cálculo del ángulo
- supresión de no máximos
- doble umbral
- histéresis

---

## Visualización

El programa ofrece dos modos de visualización:

### 1. Normal
Muestra el resultado directo del filtro.

### 2. Bordes superpuestos en color
Muestra los bordes detectados resaltados sobre la imagen original en color, para facilitar la interpretación visual del resultado.

Este segundo modo es especialmente útil para filtros de bordes como:

- LoG
- Zero Crossing
- Scharr
- Laplaciano
- Sobel Magnitude
- Canny

---

## Estructura del proyecto

```text
tallerFiltros/
├── include/
│   ├── filters.h
│   └── utils.h
├── src/
│   ├── canny.cpp
│   ├── gray.cpp
│   ├── laplacian.cpp
│   ├── log.cpp
│   ├── main.cpp
│   ├── scharr.cpp
│   ├── sobel_magnitude.cpp
│   ├── utils.cpp
│   └── zero_crossing.cpp
└── CMakeLists.txt