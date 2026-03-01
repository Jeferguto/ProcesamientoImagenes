#include <opencv2/opencv.hpp>
#include <iostream>
#include "gaussian_workshop.h"

using namespace cv;
using namespace std;

int main() {
    // Selección de imagen con zenity
    string path = pickImageWithZenity();
    if (path.empty()) {
        cout << "No se selecciono ninguna imagen (o zenity no esta instalado)." << endl;
        return 0;
    }

    // Cargar en escala de grises
    Mat img = imread(path, IMREAD_GRAYSCALE);
    if (img.empty()) {
        cout << "Error cargando imagen: " << path << endl;
        return -1;
    }

    // Punto: info + imagen original
    imshow("Imagen Original", img);
    printImageInfo(img);
    printMeanStdDev(img, "Original");

    // Segmentación Gaussiana (k = 1, 2, 3)
    Mat mask1 = gaussianSegmentation(img, 1.0);
    Mat mask2 = gaussianSegmentation(img, 2.0);
    Mat mask3 = gaussianSegmentation(img, 3.0);
    imshow("Segmentacion Gaussiana k=1", mask1);
    imshow("Segmentacion Gaussiana k=2", mask2);
    imshow("Segmentacion Gaussiana k=3", mask3);

    // Filtro Gaussiano (lowpass) y comparación de desviación
    Mat suav = applyGaussianBlur(img, 7, 1.5);
    imshow("Suavizada (GaussianBlur 7x7, sigma=1.5)", suav);
    printMeanStdDev(suav, "Suavizada");

    // Laplaciano (highpass) y afilado
    Mat lap = laplacianHighpass(img, 3);
    imshow("Highpass - Laplaciano", lap);

    Mat sharp = sharpen(img, lap);
    imshow("Imagen Afilada (img + lap)", sharp);

    // Pipeline completo (gauss -> lap -> add)
    Mat pipe = gaussianPipeline(img);
    imshow("Pipeline Completo", pipe);

    // Kernel gaussiano 2D manual
    Mat k1 = createGaussianKernel(5, 1.0);
    Mat k05 = createGaussianKernel(5, 0.5);
    Mat k3 = createGaussianKernel(5, 3.0);

    cout << "\nKernel Gaussiano 5x5 (sigma=1.0):\n" << k1 << endl;
    cout << "Suma (sigma=1.0): " << sum(k1)[0] << "\n";

    cout << "\nKernel Gaussiano 5x5 (sigma=0.5):\n" << k05 << endl;
    cout << "Suma (sigma=0.5): " << sum(k05)[0] << "\n";

    cout << "\nKernel Gaussiano 5x5 (sigma=3.0):\n" << k3 << endl;
    cout << "Suma (sigma=3.0): " << sum(k3)[0] << "\n";

    waitKey(0);
    return 0;
}