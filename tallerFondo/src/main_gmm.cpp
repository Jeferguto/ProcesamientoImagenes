#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstdio>
#include <memory>
#include <array>
#include "gmm_segmenter.h"

std::string seleccionarVideo() {

    std::array<char, 256> buffer;
    std::string result;

    std::shared_ptr<FILE> pipe(
        popen("zenity --file-selection --title='Selecciona un video'", "r"),
        pclose
    );

    if (!pipe) return "";

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    if (!result.empty() && result.back() == '\n')
        result.pop_back();

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
            std::cerr << "No se selecciono archivo.\n";
            return 1;
        }

        cap.open(ruta);
    }
    else {
        std::cerr << "Opcion invalida.\n";
        return 1;
    }

    if (!cap.isOpened()) {
        std::cerr << "No se pudo abrir la fuente.\n";
        return 1;
    }

    GMMSegmenter::Config cfg;
    GMMSegmenter gmm(cfg);

    cv::Mat frame;

    while (true) {

        cap.read(frame);
        if (frame.empty()) break;

        cv::Mat mask = gmm.apply(frame);
        auto boxes = gmm.getRegions(mask);

        cv::Mat display = frame.clone();

        for (const auto& r : boxes) {
            cv::rectangle(display, r, cv::Scalar(0,255,0), 2);
        }

        cv::Mat bg = gmm.getBackground();

        cv::imshow("GMM - Frame + Cajas", display);
        cv::imshow("GMM - Mascara limpia", mask);

        if (!bg.empty())
            cv::imshow("GMM - Fondo estimado", bg);

        int key = cv::waitKey(10);
        if (key == 27) break;
    }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}