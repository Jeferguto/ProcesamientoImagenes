#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstdio>
#include <cmath>

using namespace cv;
using namespace std;

// entrar al buscador de archivos
string seleccionarImagen() {
    FILE* pipe = popen("zenity --file-selection --title='Selecciona una imagen'", "r");
    if (!pipe) return "";

    char buffer[512];
    string ruta = "";

    if (fgets(buffer, sizeof(buffer), pipe)) {
        ruta = buffer;
        ruta.erase(ruta.find_last_not_of(" \n\r\t") + 1);
    }

    pclose(pipe);
    return ruta;
}

int main() {
    string ruta = seleccionarImagen();
    if (ruta.empty()) {
        cout << "No se selecciono imagen" << endl;
        return -1;
    }

    Mat original = imread(ruta);
    if (original.empty()) {
        cout << "Error al cargar imagen" << endl;
        return -1;
    }

    Mat gris(original.rows, original.cols, CV_8UC1);
    Mat hsv(original.rows, original.cols, CV_8UC3);
    Mat yuv(original.rows, original.cols, CV_8UC3);

    for (int i = 0; i < original.rows; i++) {
        for (int j = 0; j < original.cols; j++) {

            // OpenCV usa BGR
            float B = original.at<Vec3b>(i, j)[0];
            float G = original.at<Vec3b>(i, j)[1];
            float R = original.at<Vec3b>(i, j)[2];

            // ---------------- GRIS ----------------
            uchar gray = static_cast<uchar>(
                0.299 * R + 0.587 * G + 0.114 * B
            );
            gris.at<uchar>(i, j) = gray;

            // ---------------- YUV ----------------
            float Y = 0.299 * R + 0.587 * G + 0.114 * B;
            float U = -0.147 * R - 0.289 * G + 0.436 * B + 128;
            float V = 0.615 * R - 0.515 * G - 0.100 * B + 128;

            yuv.at<Vec3b>(i, j)[0] = saturate_cast<uchar>(Y);
            yuv.at<Vec3b>(i, j)[1] = saturate_cast<uchar>(U);
            yuv.at<Vec3b>(i, j)[2] = saturate_cast<uchar>(V);

            // ---------------- HSV ----------------
            float r = R / 255.0;
            float g = G / 255.0;
            float b = B / 255.0;

            float cmax = max(r, max(g, b));
            float cmin = min(r, min(g, b));
            float delta = cmax - cmin;

            float H = 0, S = 0, Vv = cmax;

            if (delta != 0) {
                if (cmax == r)
                    H = 60 * fmod(((g - b) / delta), 6);
                else if (cmax == g)
                    H = 60 * (((b - r) / delta) + 2);
                else
                    H = 60 * (((r - g) / delta) + 4);

                if (H < 0) H += 360;
                S = delta / cmax;
            }

            hsv.at<Vec3b>(i, j)[0] = static_cast<uchar>(H / 2);
            hsv.at<Vec3b>(i, j)[1] = static_cast<uchar>(S * 255);
            hsv.at<Vec3b>(i, j)[2] = static_cast<uchar>(Vv * 255);
        }
    }

    imshow("Original", original);
    imshow("Grises (manual)", gris);
    imshow("HSV (manual)", hsv);
    imshow("YUV (manual)", yuv);

    cout << "Presiona cualquier tecla para salir..." << endl;
    waitKey(0);
    destroyAllWindows();

    return 0;
}
