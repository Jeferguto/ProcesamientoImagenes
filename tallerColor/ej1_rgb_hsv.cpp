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

    // Quitar salto de línea final
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

    // Imagen de salida HSV
    Mat hsv(img.rows, img.cols, CV_8UC3);

    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {

            Vec3b pixel = img.at<Vec3b>(i, j);

            // OpenCV carga en BGR
            double b = pixel[0] / 255.0;
            double g = pixel[1] / 255.0;
            double r = pixel[2] / 255.0;

            double cmax = max(r, max(g, b));
            double cmin = min(r, min(g, b));
            double delta = cmax - cmin;

            double H = 0;

            // Cálculo del Hue
            if (delta != 0) {
                if (cmax == r)
                    H = 60 * fmod(((g - b) / delta), 6);
                else if (cmax == g)
                    H = 60 * (((b - r) / delta) + 2);
                else
                    H = 60 * (((r - g) / delta) + 4);
            }

            if (H < 0)
                H += 360;

            // Saturation
            double S = (cmax == 0) ? 0 : (delta / cmax);

            // Value
            double V = cmax;

            // Formato OpenCV:
            // H -> [0,180]
            // S,V -> [0,255]
            hsv.at<Vec3b>(i, j)[0] = static_cast<uchar>(H / 2);
            hsv.at<Vec3b>(i, j)[1] = static_cast<uchar>(S * 255);
            hsv.at<Vec3b>(i, j)[2] = static_cast<uchar>(V * 255);
        }
    }

    imshow("Imagen Original (BGR)", img);
    imshow("HSV Manual", hsv);

    waitKey(0);
    destroyAllWindows();

    return 0;
}
