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

    Mat resultado(img.rows, img.cols, CV_8UC3);

    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {

            Vec3b pixel = img.at<Vec3b>(i, j);

            // ======================
            // BGR -> HSV
            // ======================

            double b = pixel[0] / 255.0;
            double g = pixel[1] / 255.0;
            double r = pixel[2] / 255.0;

            double cmax = max(r, max(g, b));
            double cmin = min(r, min(g, b));
            double delta = cmax - cmin;

            double H = 0;

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

            double S = (cmax == 0) ? 0 : (delta / cmax);
            double V = cmax;

            // ======================
            // Aumentar Saturación
            // ======================

            S = S * 1.5;
            if (S > 1.0) S = 1.0;

            // ======================
            // HSV -> BGR
            // ======================

            double C = V * S;
            double X = C * (1 - fabs(fmod(H / 60.0, 2) - 1));
            double m = V - C;

            double r1, g1, b1;

            if (H >= 0 && H < 60) {
                r1 = C; g1 = X; b1 = 0;
            }
            else if (H < 120) {
                r1 = X; g1 = C; b1 = 0;
            }
            else if (H < 180) {
                r1 = 0; g1 = C; b1 = X;
            }
            else if (H < 240) {
                r1 = 0; g1 = X; b1 = C;
            }
            else if (H < 300) {
                r1 = X; g1 = 0; b1 = C;
            }
            else {
                r1 = C; g1 = 0; b1 = X;
            }

            r1 = (r1 + m) * 255;
            g1 = (g1 + m) * 255;
            b1 = (b1 + m) * 255;

            resultado.at<Vec3b>(i, j)[0] = static_cast<uchar>(b1);
            resultado.at<Vec3b>(i, j)[1] = static_cast<uchar>(g1);
            resultado.at<Vec3b>(i, j)[2] = static_cast<uchar>(r1);
        }
    }

    imshow("Imagen Original", img);
    imshow("Saturacion Aumentada (Manual)", resultado);

    waitKey(0);
    destroyAllWindows();

    return 0;
}
