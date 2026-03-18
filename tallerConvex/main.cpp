#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "converter.h"

void runCamera() {
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error: no se pudo abrir la cámara." << std::endl;
        return;
    }

    std::cout << "Cámara activa. Presiona 'q' para salir." << std::endl;
    cv::Mat frame;

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        processFrame(frame);
        cv::imshow("Convex Hull - Camara", frame);

        if (cv::waitKey(30) == 'q') break;
    }

    cap.release();
    cv::destroyAllWindows();
}

void runImage(const std::string& path) {
    cv::Mat img = cv::imread(path);
    if (img.empty()) {
        std::cerr << "Error: no se pudo cargar la imagen: " << path << std::endl;
        return;
    }

    processFrame(img);
    cv::imshow("Convex Hull - Imagen", img);
    std::cout << "Presiona cualquier tecla para cerrar." << std::endl;
    cv::waitKey(0);
    cv::destroyAllWindows();
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  Convex Hull - Graham Scan con OpenCV  " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Selecciona el modo:" << std::endl;
    std::cout << "  [1] Usar cámara en tiempo real" << std::endl;
    std::cout << "  [2] Cargar imagen desde explorador" << std::endl;
    std::cout << "Opción: ";

    int opcion;
    std::cin >> opcion;

    if (opcion == 1) {
        runCamera();
    } else if (opcion == 2) {
        std::string path = openFileDialog();
        if (path.empty()) {
            std::cerr << "No se seleccionó ningún archivo." << std::endl;
            return 1;
        }
        std::cout << "Imagen seleccionada: " << path << std::endl;
        runImage(path);
    } else {
        std::cerr << "Opción no válida." << std::endl;
        return 1;
    }

    return 0;
}
