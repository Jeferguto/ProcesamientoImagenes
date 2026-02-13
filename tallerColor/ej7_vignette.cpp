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

    double k;
    cout << "Ingrese valor de k (ej: 0.5 - 0.8): ";
    cin >> k;

    int rows = img.rows;
    int cols = img.cols;

    Mat resultado(rows, cols, CV_8UC3);

    // ============================
    // Centro de la imagen
    // ============================

    double cx = cols / 2.0;
    double cy = rows / 2.0;

    // Distancia máxima (centro a esquina)
    double dmax = sqrt(cx * cx + cy * cy);

    // ============================
    // Corrección píxel por píxel
    // ============================

    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {

            // Distancia al centro
            double dx = x - cx;
            double dy = y - cy;
            double d = sqrt(dx*dx + dy*dy);

            // Normalizar distancia
            double dnorm = d / dmax;

            // Factor de corrección (según PDF)
            double factor = 1.0 / (1.0 - k * dnorm * dnorm);

            Vec3b pixel = img.at<Vec3b>(y, x);

            int nuevoB = pixel[0] * factor;
            int nuevoG = pixel[1] * factor;
            int nuevoR = pixel[2] * factor;

            // Limitar a 255
            if (nuevoB > 255) nuevoB = 255;
            if (nuevoG > 255) nuevoG = 255;
            if (nuevoR > 255) nuevoR = 255;

            resultado.at<Vec3b>(y, x) = Vec3b(nuevoB, nuevoG, nuevoR);
        }
    }

    imshow("Imagen Original", img);
    imshow("Correccion de Vigneteo (Manual)", resultado);

    waitKey(0);
    destroyAllWindows();

    return 0;
}
