# Procesamiento de Imágenes con OpenCV (Conversión Manual de Espacios de Color)

Este proyecto implementa un programa en **C++ usando OpenCV** que permite cargar una imagen desde el sistema y convertirla a distintos espacios de color **utilizando fórmulas matemáticas**, sin emplear las funciones automáticas de conversión de OpenCV como `cvtColor`.

El objetivo principal es comprender y aplicar los fundamentos del **procesamiento digital de imágenes**, trabajando directamente a nivel de píxel.

---

## Funcionalidad del programa

El programa realiza las siguientes acciones:

1. Abre un **explorador de archivos** para que el usuario seleccione una imagen.
2. Carga la imagen seleccionada en memoria.
3. Convierte la imagen original a:
   - **Escala de grises**
   - **HSV**
   - **YUV**
4. Muestra simultáneamente:
   - Imagen original
   - Imagen en escala de grises
   - Imagen en HSV
   - Imagen en YUV

Todas las conversiones se realizan **pixel por pixel usando ecuaciones matemáticas**, sin utilizar funciones de alto nivel de OpenCV para la conversión de color.

---

## Conversión de espacios de color

### Escala de grises
Se utiliza la fórmula de luminancia:

\[
Gray = 0.299R + 0.587G + 0.114B
\]

---

### RGB a YUV
Se emplean las siguientes ecuaciones:

\[
Y = 0.299R + 0.587G + 0.114B
\]

\[
U = -0.147R - 0.289G + 0.436B + 128
\]

\[
V = 0.615R - 0.515G - 0.100B + 128
\]

---

### RGB a HSV
El espacio HSV se calcula a partir de los valores normalizados de RGB, obteniendo:
- **H (Hue)** según el canal dominante
- **S (Saturation)** como la relación entre la diferencia de canales
- **V (Value)** como el valor máximo de los canales RGB

Los valores se ajustan al rango utilizado por OpenCV:
- H ∈ [0, 180]
- S ∈ [0, 255]
- V ∈ [0, 255]

---

## Requisitos

- Sistema operativo Linux
- **OpenCV 4**
- **g++**
- **pkg-config**
- **zenity** (para el selector gráfico de archivos)

Instalación de dependencias en Ubuntu/Debian:

```bash
sudo apt update
sudo apt install libopencv-dev pkg-config zenity
```

## Compilación

Desde la carpeta donde se encuentra el archivo fuente convertir.cpp, ejecutar el siguiente comando:

g++ convertir.cpp -o convertir `pkg-config --cflags --libs opencv4`

Este comando compila el programa enlazando las librerías necesarias de OpenCV.

## Ejecución

Una vez compilado, ejecutar:

./convertir

Al ejecutarse el programa:
- Se abrirá un explorador de archivos para seleccionar la imagen.
- Se mostrarán cuatro ventanas:
  - Imagen original
  - Imagen en escala de grises
  - Imagen en HSV
  - Imagen en YUV
- Presione cualquier tecla para cerrar el programa.

## Notas

- OpenCV utiliza el formato de color BGR por defecto.
- Las conversiones de color se realizan de forma manual, pixel por pixel, utilizando fórmulas matemáticas.
- No se utilizan funciones automáticas de conversión de color como cvtColor.
- Todo el procesamiento se realiza en memoria y la imagen original no se modifica.

## Autor

Jefferson Gutierrez Tovar, estudiante de Ciencias de la Computación e I.A
