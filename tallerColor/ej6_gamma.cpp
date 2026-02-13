#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
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

    double gamma;
    cout << "Ingrese valor de gamma (ej: 0.5, 1.0, 2.0): ";
    cin >> gamma;

    if (gamma <= 0) {
        cout << "Gamma debe ser mayor que 0." << endl;
        return -1;
    }

    // ============================
    // Crear tabla de 256 valores
    // ============================

    unsigned char tabla[256];

    for (int i = 0; i < 256; i++) {
        double normalizado = i / 255.0;
        double corregido = pow(normalizado, gamma);
        int valor = static_cast<int>(255 * corregido);

        if (valor > 255) valor = 255;
        if (valor < 0) valor = 0;

        tabla[i] = static_cast<unsigned char>(valor);
    }

    // ============================
    // Aplicar tabla manualmente
    // ============================

    Mat resultado(img.rows, img.cols, CV_8UC3);

    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {

            Vec3b pixel = img.at<Vec3b>(i, j);

            resultado.at<Vec3b>(i, j)[0] = tabla[pixel[0]];
            resultado.at<Vec3b>(i, j)[1] = tabla[pixel[1]];
            resultado.at<Vec3b>(i, j)[2] = tabla[pixel[2]];
        }
    }

    imshow("Imagen Original", img);
    imshow("Correccion Gamma (Manual)", resultado);

    waitKey(0);
    destroyAllWindows();

    return 0;
}
