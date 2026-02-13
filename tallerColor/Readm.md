# Taller Color – Procesamiento Digital de Imágenes con OpenCV (C++)

Este proyecto implementa diferentes técnicas fundamentales de **procesamiento digital de imágenes** utilizando **C++ y OpenCV**, trabajando siempre **píxel por píxel** y evitando el uso de funciones automáticas como `cvtColor`, `kmeans`, `LUT`, etc., tal como lo exige el taller.

---

## 📂 Estructura del Proyecto

```
tallerColor/
│── CMakeLists.txt
│── ej1_rgb_hsv.cpp
│── ej2_saturacion.cpp
│── ej3_kmeans.cpp
│── ej4_grayworld.cpp
│── ej6_gamma.cpp
│── ej7_vignette.cpp
│── build/
```

Cada archivo `.cpp` corresponde a un ejercicio independiente y genera su propio ejecutable.

---

## ⚙️ Requisitos

- Ubuntu / Linux
- g++
- CMake
- OpenCV 4.x
- Zenity (para selector de archivos)

Instalar dependencias si es necesario:

```bash
sudo apt update
sudo apt install cmake g++ libopencv-dev zenity
```

---

## 🔨 Compilación

Desde la carpeta `tallerColor`:

```bash
mkdir build
cd build
cmake ..
make
```

Esto generará los ejecutables:

```
ej1
ej2
ej3
ej4
ej6
ej7
```

---

## ▶️ Ejecución

Ejemplo:

```bash
./ej1
```

Todos los ejercicios:

- Abren un explorador de archivos para seleccionar la imagen.
- Procesan la imagen manualmente.
- Muestran la imagen original y la imagen resultante.

---

# 📌 Descripción de los Ejercicios

---

## 1️⃣ Ejercicio 1 – Conversión BGR → HSV Manual

**Objetivo:**  
Convertir una imagen de BGR a HSV utilizando las fórmulas matemáticas.

**Implementación:**

- Normalización de valores a [0,1].
- Cálculo de:
  - \( C_{max} \)
  - \( C_{min} \)
  - \( \Delta \)
- Cálculo manual de:
  - Hue (H)
  - Saturation (S)
  - Value (V)
- Ajuste al formato OpenCV:
  - H → [0,180]
  - S, V → [0,255]

No se usa `cvtColor`.

---

## 2️⃣ Ejercicio 2 – Aumento de Saturación (HSV → BGR Manual)

**Objetivo:**  
Modificar la saturación de la imagen y convertir nuevamente a BGR manualmente.

**Implementación:**

- Conversión manual BGR → HSV.
- Aumento de saturación con factor (ej. ×1.5).
- Limitación al rango válido.
- Conversión manual HSV → BGR usando:
  - C
  - X
  - m
- Aplicación píxel por píxel.

No se usa `cvtColor`.

---

## 3️⃣ Ejercicio 3 – K-Means Manual

**Objetivo:**  
Segmentar la imagen en K colores dominantes sin usar `cv::kmeans`.

**Implementación:**

1. Convertir imagen en vector de píxeles.
2. Inicializar K centroides aleatorios.
3. Asignar cada píxel al centroide más cercano (distancia euclidiana RGB).
4. Recalcular centroides como promedio.
5. Repetir 10 iteraciones.
6. Reconstruir imagen con colores cuantizados.

Este ejercicio implementa el algoritmo completo desde cero.

---

## 4️⃣ Ejercicio 4 – Gray World (Balance de Blancos)

**Objetivo:**  
Corregir dominantes de color basándose en la hipótesis de que el promedio del mundo es gris.

**Implementación:**

1. Calcular promedio de cada canal (B, G, R).
2. Calcular promedio global.
3. Obtener factores de corrección:
   - factorB = gray / promB
   - factorG = gray / promG
   - factorR = gray / promR
4. Multiplicar cada píxel por su factor.
5. Limitar valores a 255.

No se usan funciones automáticas de balance.

---

## 6️⃣ Ejercicio 6 – Corrección Gamma

**Objetivo:**  
Aplicar corrección radiométrica para ajustar brillo no lineal.

**Fórmula:**

\[
I_{out} = 255 \cdot \left(\frac{I_{in}}{255}\right)^\gamma
\]

**Implementación:**

- Creación de tabla de 256 valores (Lookup Table manual).
- Aplicación de la tabla a cada canal.
- No se usa `cv::LUT`.

Efectos:

- γ < 1 → Aclara la imagen.
- γ > 1 → Oscurece la imagen.

---

## 7️⃣ Ejercicio 7 – Corrección de Viñeteo

**Objetivo:**  
Corregir oscurecimiento en los bordes de la imagen.

**Implementación:**

1. Calcular centro de la imagen.
2. Calcular distancia del píxel al centro.
3. Normalizar distancia.
4. Aplicar factor:

\[
f = \frac{1}{1 - k \cdot d_{norm}^2}
\]

5. Multiplicar cada canal por el factor.
6. Limitar a 255.

El parámetro `k` controla la intensidad de la corrección.

---

# 🧠 Conceptos Aplicados

- Espacios de color
- Segmentación por clustering
- Balance de blancos
- Corrección radiométrica
- Normalización
- Procesamiento píxel por píxel
- Distancia euclidiana
- Tablas de búsqueda

---

# 🚫 Restricciones Respetadas

No se utilizaron:

- `cvtColor`
- `kmeans`
- `LUT`
- Funciones automáticas de corrección

Todas las transformaciones fueron implementadas manualmente.

---

# 🎓 Conclusión

Este taller permite comprender a profundidad:

- Cómo funcionan los espacios de color.
- Cómo opera el algoritmo K-Means internamente.
- Cómo se corrige iluminación y radiometría.
- Cómo implementar procesamiento digital de imágenes a bajo nivel.

El enfoque manual fortalece la comprensión matemática detrás de OpenCV.
