#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    // Abrir la cámara (0 = cámara por defecto)
    VideoCapture cam(0);

    // Verificar si se abrió correctamente
    if (!cam.isOpened()) {
        cout << "Error: no se pudo abrir la camara" << endl;
        return -1;
    }

    Mat frame;

    cout << "Presiona ESC para salir" << endl;

    while (true) {
        cam >> frame;  // Capturar un frame

        if (frame.empty()) {
            cout << "Frame vacio" << endl;
            break;
        }

        imshow("Camara OpenCV", frame);

        // Salir con ESC (27)
        if (waitKey(30) == 27) {
            break;
        }
    }

    cam.release();
    destroyAllWindows();

    return 0;
}
