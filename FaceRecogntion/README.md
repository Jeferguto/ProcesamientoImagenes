# Face Recognition — Android con OpenCV y PCA

Aplicación Android de reconocimiento facial en tiempo real implementada con **C++ nativo (NDK)**, **OpenCV** y el algoritmo de **Análisis de Componentes Principales (PCA)** también conocido como *Eigenfaces*. Toda la lógica de visión computacional corre en C++ expuesta a Kotlin mediante **JNI**.

---

## ¿Qué hace la aplicación?

La app usa la cámara frontal del dispositivo para:

1. **Detectar** si hay un rostro en el frame actual.
2. **Entrenarse** capturando 20 fotos de una persona para aprender cómo se ve.
3. **Reconocer** en tiempo real quién está frente a la cámara comparando el rostro detectado contra los que aprendió.

La pantalla muestra:
- El visor de la cámara en vivo.
- Un texto con el resultado: nombre de la persona reconocida, `Desconocido`, o `Sin rostro`.
- Un botón para iniciar/finalizar el entrenamiento.

---

## Cómo funciona por dentro

El pipeline de procesamiento ocurre una vez por segundo sobre cada frame capturado.

### 1. Captura de frame — Kotlin / CameraX

`MainActivity.kt` usa **CameraX** (`ImageAnalysis`) para recibir frames de la cámara frontal. Cada frame se convierte a `android.graphics.Bitmap` y luego a una matriz OpenCV (`Mat`) usando `Utils.bitmapToMat`.

### 2. Detección de rostro — `FaceDetector.cpp`

```
frame RGBA → escala de grises → ecualización de histograma → Haar Cascade
```

- Convierte el frame a escala de grises.
- Aplica **ecualización de histograma** (`equalizeHist`) para mejorar el contraste y hacer la detección más robusta a distintas condiciones de iluminación.
- Usa el clasificador **Haar Cascade** (`haarcascade_frontalface_default.xml`) con `detectMultiScale` para encontrar rostros.
- Si detecta un rostro, recorta la región de interés (ROI) y la redimensiona a **100 × 100 píxeles**.

### 3. Entrenamiento con PCA — `MyPCA.cpp`

Cuando se presiona "Entrenar", se capturan 20 frames. Con ellos se construye el modelo PCA:

| Paso | Operación |
|------|-----------|
| Aplanar | Cada imagen 100×100 se convierte en un vector de 10 000 valores |
| Matriz global | Se apilan todos los vectores en una matriz `A` de tamaño `10000 × N` |
| Vector promedio | Se calcula el rostro promedio (`μ`) promediando las columnas |
| Centrado | Se resta el promedio a cada imagen: `A' = A − μ` |
| Covarianza | Se calcula `C = A'ᵀ · A'` (truco para matrices grandes) |
| Eigenvectores | Se obtienen con `cv::eigen(C)` y se transforman al espacio original |
| Normalización | Cada eigenvector se normaliza a longitud unitaria |

Los eigenvectores resultantes son los **eigenfaces** — las "direcciones" que capturan la mayor varianza entre los rostros entrenados.

Finalmente, cada imagen de entrenamiento se **proyecta** al espacio PCA:

```
proyección = (imagen_centrada) × eigenvectores^T
```

### 4. Reconocimiento — `native-lib.cpp`

Para cada frame nuevo (ya entrenado el modelo):

1. Se detecta el rostro con `FaceDetector`.
2. Se proyecta al mismo espacio PCA.
3. Se calcula la **distancia euclidiana** entre esa proyección y todas las proyecciones de entrenamiento.
4. Se toma la más cercana.
5. Si la distancia mínima supera el umbral `5000.0` → devuelve `"Desconocido"`.
6. Si no → devuelve el nombre de la persona más cercana.

### 5. Comunicación Kotlin ↔ C++ — JNI

Las tres funciones nativas declaradas en `MainActivity.kt` se implementan en `native-lib.cpp`:

```kotlin
external fun initDetector(cascadePath: String)        // Carga el Haar Cascade
external fun trainPCA(matAddrs: LongArray, labels: Array<String>)  // Entrena el modelo
external fun recognizeFace(matAddr: Long): String     // Reconoce un frame
```

---

## Estructura del proyecto

```
FaceRecogntion/
├── app/
│   └── src/main/
│       ├── cpp/
│       │   ├── CMakeLists.txt        # Configuración de compilación NDK
│       │   ├── native-lib.cpp        # JNI: puente Kotlin ↔ C++
│       │   ├── FaceDetector.h/.cpp   # Detección con Haar Cascade
│       │   └── MyPCA.h/.cpp          # Implementación manual de PCA
│       ├── java/com/example/facerecognition/
│       │   └── MainActivity.kt       # UI, cámara, flujo principal
│       ├── assets/
│       │   └── haarcascade_frontalface_default.xml  # Modelo de detección
│       └── res/layout/
│           └── activity_main.xml     # Layout de la pantalla
├── opencv/                           # OpenCV Android SDK (no incluido en git)
└── README.md
```

---

## Requisitos

| Herramienta | Versión recomendada |
|---|---|
| Android Studio | Hedgehog o superior |
| NDK | 25 o superior |
| CMake | 3.22.1 o superior |
| SDK Android mínimo | API 24 (Android 7.0) |
| OpenCV Android SDK | 4.x |

---

## Configuración y ejecución en Android Studio

### Paso 1 — Descargar el OpenCV Android SDK

1. Descarga el **OpenCV Android SDK** desde [opencv.org/releases](https://opencv.org/releases/) (elige la versión Android `.zip`).
2. Descomprime el archivo. Obtendrás una carpeta llamada `OpenCV-android-sdk/`.
3. Copia la carpeta `sdk/` dentro del proyecto renombrándola como `opencv/`:

```
FaceRecogntion/
└── opencv/          ← pega aquí el contenido de OpenCV-android-sdk/sdk/
    ├── build.gradle
    ├── native/
    └── java/
```

### Paso 2 — Actualizar la ruta en CMakeLists.txt

Abre `app/src/main/cpp/CMakeLists.txt` y cambia la ruta de OpenCV a donde lo colocaste:

```cmake
# Cambia esta línea:
set(OpenCV_DIR /Users/jeffersongutierreztovar/Desktop/OpenCV-android-sdk/sdk/native/jni)

# Por la ruta real en tu máquina, por ejemplo:
set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/../../../opencv/native/jni)
```

> Si colocaste opencv/ dentro de la carpeta del proyecto como se indica en el Paso 1, la ruta relativa `${CMAKE_SOURCE_DIR}/../../../opencv/native/jni` debería funcionar directamente.

### Paso 3 — Abrir el proyecto en Android Studio

1. Abre Android Studio.
2. Selecciona **File → Open** y elige la carpeta `FaceRecogntion/` (la que contiene `settings.gradle.kts`).
3. Espera a que Gradle sincronice el proyecto (puede tardar unos minutos la primera vez).

### Paso 4 — Instalar NDK y CMake (si no los tienes)

1. Ve a **File → Settings → Appearance & Behavior → System Settings → Android SDK**.
2. Selecciona la pestaña **SDK Tools**.
3. Marca **NDK (Side by side)** y **CMake** y haz clic en **Apply**.

### Paso 5 — Ejecutar en un dispositivo físico

> Se recomienda un **dispositivo físico** sobre el emulador, ya que la app usa la cámara frontal para capturar rostros en tiempo real.

1. Conecta tu teléfono Android al computador via USB.
2. Activa el **Modo Desarrollador** y la **Depuración USB** en tu teléfono.
3. En Android Studio, selecciona tu dispositivo en el menú desplegable superior.
4. Haz clic en el botón **Run** (▶) o presiona `Shift + F10`.
5. La app pedirá permiso para usar la cámara — acéptalo.

---

## Cómo usar la aplicación

```
┌─────────────────────────┐
│   [Vista de cámara]     │
│                         │
│                         │
│   Presiona Entrenar     │  ← texto de resultado
│  [  Entrenar  ]         │  ← botón
└─────────────────────────┘
```

### Entrenar una persona

1. Apunta la cámara frontal hacia tu rostro.
2. Presiona **"Entrenar"**.
3. La app capturará automáticamente **20 fotos** (una por segundo) mostrando el progreso `Capturando... (X/20)`.
4. Mantén tu rostro visible y con buena iluminación durante la captura.
5. Al completar las 20 fotos, el modelo PCA se entrena automáticamente.
6. El botón cambia a **"Entrenar nueva persona"** y el texto mostrará `Entrenado! Apunta la camara`.

### Reconocer en tiempo real

- Con el modelo entrenado, simplemente apunta la cámara hacia el rostro.
- El texto mostrará el nombre reconocido, `Desconocido` si no coincide con nadie, o `Sin rostro` si no hay un rostro detectado.

### Consejos para mejores resultados

- Usa iluminación frontal uniforme durante el entrenamiento y el reconocimiento.
- Mantén una distancia de ~30–60 cm de la cámara.
- Evita fondos muy complejos o cambios drásticos de iluminación entre entrenamiento y reconocimiento.
- El umbral de reconocimiento está fijo en `5000.0` en `native-lib.cpp` — puedes ajustarlo si los resultados son muy estrictos o muy permisivos.

---

## Tecnologías utilizadas

| Tecnología | Uso |
|---|---|
| Kotlin | UI y lógica de la Activity |
| CameraX | Acceso a la cámara y análisis de frames |
| C++ (NDK) | Procesamiento de imágenes con OpenCV |
| JNI | Comunicación entre Kotlin y C++ |
| OpenCV 4.x | Detección de rostros, procesamiento matricial |
| Haar Cascade | Localización del rostro en el frame |
| PCA (Eigenfaces) | Extracción de características y reconocimiento |
