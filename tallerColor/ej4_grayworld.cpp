#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstdio>
#include <array>

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

    Mat resultado(img.rows, img.cols, CV_8UC3);

    double sumaB = 0, sumaG = 0, sumaR = 0;
    int totalPixels = img.rows * img.cols;

    // ============================
    // Paso 1: Calcular promedios
    // ============================

    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {

            Vec3b pixel = img.at<Vec3b>(i, j);

            sumaB += pixel[0];
            sumaG += pixel[1];
            sumaR += pixel[2];
        }
    }

    double promB = sumaB / totalPixels;
    double promG = sumaG / totalPixels;
    double promR = sumaR / totalPixels;

    // ============================
    // Paso 2: Promedio global gris
    // ============================

    double gray = (promB + promG + promR) / 3.0;

    // ============================
    // Paso 3: Factores de escala
    // ============================

    double factorB = gray / promB;
    double factorG = gray / promG;
    double factorR = gray / promR;

    cout << "Factor B: " << factorB << endl;
    cout << "Factor G: " << factorG << endl;
    cout << "Factor R: " << factorR << endl;

    // ============================
    // Paso 4: Aplicar corrección
    // ============================

    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {

            Vec3b pixel = img.at<Vec3b>(i, j);

            int nuevoB = pixel[0] * factorB;
            int nuevoG = pixel[1] * factorG;
            int nuevoR = pixel[2] * factorR;

            // Limitar a 255
            if (nuevoB > 255) nuevoB = 255;
            if (nuevoG > 255) nuevoG = 255;
            if (nuevoR > 255) nuevoR = 255;

            resultado.at<Vec3b>(i, j) = Vec3b(nuevoB, nuevoG, nuevoR);
        }
    }

    imshow("Imagen Original", img);
    imshow("Gray World (Balance de Blancos Manual)", resultado);

    waitKey(0);
    destroyAllWindows();

    return 0;
}
