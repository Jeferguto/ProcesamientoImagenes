#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdio>
#include <array>
#include <cstdlib>
#include <ctime>

using namespace cv;
using namespace std;

/* ================================
   Selector de archivo con Zenity
   ================================ */
string seleccionarImagen() {
    string comando = "zenity --file-selection --title='Selecciona una imagen'";
    array<char, 128> buffer;
    string resultado;

    FILE* pipe = popen(comando.c_str(), "r");
    if (!pipe) return "";

    while (fgets(buffer.data(), 128, pipe) != nullptr) {
        resultado += buffer.data();
    }

    pclose(pipe);

    if (!resultado.empty() && resultado.back() == '\n')
        resultado.pop_back();

    return resultado;
}

/* ================================
            MAIN
   ================================ */
int main() {

    srand(time(0));

    string ruta = seleccionarImagen();
    if (ruta.empty()) {
        cout << "No se seleccionó ninguna imagen." << endl;
        return -1;
    }

    Mat img = imread(ruta);
    if (img.empty()) {
        cout << "Error al cargar la imagen." << endl;
        return -1;
    }

    int K;
    cout << "Ingrese el numero de clusters (K): ";
    cin >> K;

    int rows = img.rows;
    int cols = img.cols;
    int totalPixels = rows * cols;

    // ============================
    // Convertir imagen a vector
    // ============================

    vector<Vec3f> pixels;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            Vec3b p = img.at<Vec3b>(i, j);
            pixels.push_back(Vec3f(p[0], p[1], p[2]));
        }
    }

    // ============================
    // Inicializar centroides aleatorios
    // ============================

    vector<Vec3f> centroids;

    for (int k = 0; k < K; k++) {
        int idx = rand() % totalPixels;
        centroids.push_back(pixels[idx]);
    }

    vector<int> labels(totalPixels);

    int iterations = 10;

    for (int iter = 0; iter < iterations; iter++) {

        // ============================
        // Paso 1: Asignación
        // ============================

        for (int i = 0; i < totalPixels; i++) {

            float minDist = FLT_MAX;
            int bestCluster = 0;

            for (int k = 0; k < K; k++) {

                float db = pixels[i][0] - centroids[k][0];
                float dg = pixels[i][1] - centroids[k][1];
                float dr = pixels[i][2] - centroids[k][2];

                float dist = db*db + dg*dg + dr*dr;

                if (dist < minDist) {
                    minDist = dist;
                    bestCluster = k;
                }
            }

            labels[i] = bestCluster;
        }

        // ============================
        // Paso 2: Recalcular centroides
        // ============================

        vector<Vec3f> newCentroids(K, Vec3f(0,0,0));
        vector<int> counts(K, 0);

        for (int i = 0; i < totalPixels; i++) {
            int cluster = labels[i];
            newCentroids[cluster] += pixels[i];
            counts[cluster]++;
        }

        for (int k = 0; k < K; k++) {
            if (counts[k] != 0)
                centroids[k] = newCentroids[k] / counts[k];
        }
    }

    // ============================
    // Crear imagen segmentada
    // ============================

    Mat resultado(rows, cols, CV_8UC3);

    int index = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int cluster = labels[index];
            resultado.at<Vec3b>(i, j) = Vec3b(
                static_cast<uchar>(centroids[cluster][0]),
                static_cast<uchar>(centroids[cluster][1]),
                static_cast<uchar>(centroids[cluster][2])
            );
            index++;
        }
    }

    imshow("Imagen Original", img);
    imshow("Segmentacion K-Means Manual", resultado);

    waitKey(0);
    destroyAllWindows();

    return 0;
}
