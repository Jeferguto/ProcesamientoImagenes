## Integrantes: Jefferson Gutierrez y Juan Joya

# Taller de Laboratorio: Segmentación de Rostros con OpenCV y C++

Este proyecto implementa dos enfoques clásicos de segmentación de primer plano utilizando C++ y OpenCV:

- **Parte A:** Frame Differencing (diferenciamiento de cuadros)
- **Parte B:** Modelado Gaussiano de Píxeles (GMM) usando `BackgroundSubtractorMOG2` (MOG2)

El objetivo es separar el fondo estático de los objetos en movimiento, generar una máscara binaria y dibujar cajas delimitadoras (bounding boxes) sobre las regiones detectadas.

---

## Tecnologías Utilizadas

- Lenguaje: **C++17**
- Librería: **OpenCV 4.x**
- Sistema de construcción: **CMake 3.15 o superior**
- Compilador: **g++ (GNU)**
- Sistema operativo: **Linux**
- Herramienta adicional: **Zenity** (para selección gráfica de archivos de video)

---

## Estructura del Proyecto

```
tallerFondo/
├── CMakeLists.txt
├── include/
│   ├── frame_diff.h
│   └── gmm_segmenter.h
├── src/
│   ├── frame_diff.cpp
│   ├── main_frame_diff.cpp
│   ├── gmm_segmenter.cpp
│   └── main_gmm.cpp
└── data/
    └── (videos de prueba opcionales)
```

- `include/` contiene los archivos de cabecera.
- `src/` contiene las implementaciones y los programas principales.
- `data/` puede almacenar videos de prueba.
- `build/` se genera automáticamente al compilar y **no debe subirse al repositorio**.

---

## Requisitos Previos

En sistemas basados en Debian/Ubuntu:

```bash
sudo apt update
sudo apt install build-essential cmake libopencv-dev zenity
```

- Zenity es opcional y solo se utiliza para abrir un explorador gráfico al seleccionar video.
- Si se usa cámara, no es necesario ningún archivo de video.

---

## Compilación

Desde la carpeta raíz del proyecto (donde se encuentra `CMakeLists.txt`):

```bash
mkdir -p build
cd build
cmake ..
make -j4
```

Esto generará dos ejecutables dentro de la carpeta `build`:

- `parte_a`
- `parte_b`

---

## Ejecución

La ejecución siempre se realiza desde la carpeta `build`.

### Ejecutar Parte A (Frame Differencing)

```bash
./parte_a
```

El programa solicitará seleccionar el modo de entrada:

1 - Cámara  
2 - Seleccionar video  

Si se elige la opción 2, se abrirá un explorador gráfico mediante Zenity.

#### Pipeline implementado en Parte A

1. Captura el primer frame como fondo (background).
2. Convierte a escala de grises.
3. Calcula la diferencia absoluta: `absdiff(background, frame_actual)`.
4. Aplica umbralización (`threshold`) para generar máscara binaria.
5. Aplica operaciones morfológicas (`MORPH_OPEN` y `MORPH_CLOSE`).
6. Detecta contornos con `findContours`.
7. Filtra por área mínima.
8. Dibuja `boundingRect` sobre el frame original.
9. Finaliza con la tecla ESC.

Características:
- Método sencillo y rápido.
- Sensible a cambios de iluminación.
- No reconstruye dinámicamente el fondo.

---

### Ejecutar Parte B (GMM - MOG2)

```bash
./parte_b
```

El comportamiento inicial es el mismo:

1 - Cámara  
2 - Seleccionar video  

#### Pipeline implementado en Parte B

1. Crea un `BackgroundSubtractorMOG2`.
2. Para cada frame:
   - Aplica `apply()` para obtener máscara de primer plano.
   - Elimina sombras con `threshold` (conserva solo valores 255).
   - Aplica operaciones morfológicas.
   - Detecta contornos.
   - Filtra por área mínima.
   - Dibuja cajas delimitadoras.
   - Reconstruye el fondo estimado con `getBackgroundImage()`.
3. Muestra tres ventanas:
   - Frame original con cajas
   - Máscara limpia
   - Fondo estimado
4. Finaliza con ESC.

Características:
- Más robusto frente a cambios de iluminación.
- Modelo dinámico del fondo.
- Puede absorber objetos estáticos al fondo si permanecen inmóviles.

---

## Parámetros Ajustables

### Parte A

En `main_frame_diff.cpp`:

```cpp
FrameDifferencer fd(40, 2000, 0.0);
```

- Primer parámetro: umbral de binarización.
- Segundo parámetro: área mínima de contorno.
- Tercer parámetro: learning rate (0.0 = fondo fijo).

### Parte B

En `main_gmm.cpp` dentro de `GMMSegmenter::Config`:

- `history`: número de cuadros para construir el modelo.
- `var_threshold`: sensibilidad del modelo.
- `detect_shadows`: habilita detección de sombras.
- `min_area`: área mínima de contorno.
- `morph_open_k` y `morph_close_k`: tamaño de kernels morfológicos.

---

## Comparación General

| Característica              | Frame Differencing | GMM (MOG2) |
|----------------------------|-------------------|------------|
| Modelo de fondo dinámico   | No                | Sí         |
| Robustez a iluminación     | Baja              | Alta       |
| Sensibilidad al ruido      | Alta              | Media      |
| Reconstrucción del fondo   | No                | Sí         |
| Complejidad                | Baja              | Media      |

---

## Notas Finales

- La carpeta `build/` debe agregarse al `.gitignore`.
- Si el usuario permanece quieto durante varios segundos en GMM, el modelo puede incorporarlo al fondo.
- Para mejorar detección, pueden ajustarse los parámetros de umbral, área mínima y morfología.

Este proyecto permite comprender de manera práctica dos técnicas fundamentales de segmentación en visión por computador y comparar su comportamiento en escenarios reales.