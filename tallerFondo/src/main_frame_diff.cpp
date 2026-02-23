#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include "frame_diff.h"

// Funcion para abrir zenity y capturar la ruta seleccionada
std::string seleccionarVideo() {
    std::array<char, 256> buffer;
    std::string result;

    std::shared_ptr<FILE> pipe(
        popen("zenity --file-selection --title='Selecciona un video'", "r"),
        pclose
    );

    if (!pipe) {
        std::cerr << "No se pudo ejecutar zenity.\n";
        return "";
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    // Quitar salto de línea final
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }

    return result;
}

int main() {

    std::cout << "Seleccione modo:\n";
    std::cout << "1 - Camara\n";
    std::cout << "2 - Seleccionar video\n";
    std::cout << "Opcion: ";

    int opcion;
    std::cin >> opcion;

    cv::VideoCapture cap;

    if (opcion == 1) {
        cap.open(0);
    }
    else if (opcion == 2) {
        std::string ruta = seleccionarVideo();

        if (ruta.empty()) {
            std::cerr << "No se selecciono ningun archivo.\n";
            return 1;
        }

        cap.open(ruta);
    }
    else {
        std::cerr << "Opcion invalida.\n";
        return 1;
    }

    if (!cap.isOpened()) {
        std::cerr << "No se pudo abrir la fuente de video.\n";
        return 1;
    }

    cv::Mat frame;
    cap.read(frame);

    if (frame.empty()) {
        std::cerr << "No se pudo capturar el primer frame.\n";
        return 1;
    }

    FrameDifferencer fd(40, 2000, 0.0);
    fd.setBackground(frame);

    while (true) {

        cap.read(frame);
        if (frame.empty()) break;

        cv::Mat mask = fd.process(frame);
        auto boxes = fd.getRegions(mask);

        cv::Mat display = frame.clone();

        for (const auto& r : boxes) {
            cv::rectangle(display, r, cv::Scalar(0,255,0), 2);
        }

        cv::imshow("Frame Original + Cajas", display);
        cv::imshow("Mascara Binaria", mask);

        int key = cv::waitKey(10);
        if (key == 27) break; // ESC
    }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}