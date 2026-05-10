#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "geometria.h"

int main() {
    std::cout << "============================================\n";
    std::cout << "  Taller Geometria - Procesamiento Imagenes\n";
    std::cout << "============================================\n";
    std::cout << "Ruta de la imagen: ";

    std::string path;
    std::getline(std::cin >> std::ws, path);

    cv::Mat img = cv::imread(path, cv::IMREAD_COLOR);
    if (img.empty()) {
        std::cerr << "Error: no se pudo cargar: " << path << "\n";
        return 1;
    }
    std::cout << "Imagen cargada: " << img.cols << " x " << img.rows << " px\n";

    int op;
    do {
        std::cout << "\n=== MENU PRINCIPAL ===\n";
        std::cout << "[1] Puntos y Vectores\n";
        std::cout << "[2] Lineas y Segmentos (Bresenham)\n";
        std::cout << "[3] Poligonos (Shoelace + Scanline)\n";
        std::cout << "[4] Circulos y Elipses\n";
        std::cout << "[5] Transformaciones (Matrices 3x3)\n";
        std::cout << "[6] Procesamiento de Imagen (ROI, Ray Casting)\n";
        std::cout << "[0] Salir\n";
        std::cout << "Opcion: ";
        std::cin >> op;

        switch (op) {
            case 1: paso1_puntosVectores(img);   break;
            case 2: paso2_lineas(img);           break;
            case 3: paso3_poligonos(img);        break;
            case 4: paso4_circulos(img);         break;
            case 5: paso5_transformaciones(img); break;
            case 6: paso6_procesamiento(img);    break;
            case 0: std::cout << "Saliendo...\n"; break;
            default: std::cout << "Opcion invalida.\n";
        }
    } while (op != 0);

    return 0;
}
