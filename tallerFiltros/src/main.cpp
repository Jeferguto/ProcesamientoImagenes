#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>

#include "filters.h"
#include "utils.h"

void printMenuFuente() {
    std::cout << "\n=== FUENTE DE ENTRADA ===\n";
    std::cout << "1. Camara\n";
    std::cout << "2. Imagen desde explorador\n";
    std::cout << "Seleccione una opcion: ";
}

void printMenuFiltros() {
    std::cout << "\n=== FILTROS ===\n";
    std::cout << "1. Gray\n";
    std::cout << "2. LoG\n";
    std::cout << "3. Zero Crossing\n";
    std::cout << "4. Scharr\n";
    std::cout << "5. Laplaciano\n";
    std::cout << "6. Sobel Magnitude\n";
    std::cout << "7. Canny\n";
    std::cout << "8. Todos en cuadricula\n";
    std::cout << "Seleccione una opcion: ";
}

void printMenuVisualizacion() {
    std::cout << "\n=== VISUALIZACION ===\n";
    std::cout << "1. Normal\n";
    std::cout << "2. Bordes superpuestos en color\n";
    std::cout << "Seleccione una opcion: ";
}

cv::Mat processSingleFilter(const cv::Mat& frame, int option, std::string& title) {
    switch (option) {
        case 1: title = "Gray"; return applyGray(frame);
        case 2: title = "LoG"; return applyLoG(frame);
        case 3: title = "Zero Crossing"; return applyZeroCrossing(frame);
        case 4: title = "Scharr"; return applyScharrEdges(frame);
        case 5: title = "Laplaciano"; return applyLaplacianEdges(frame);
        case 6: title = "Sobel Magnitude"; return applySobelMagnitude(frame);
        case 7: title = "Canny"; return applyCannyEdges(frame);
        default: title = "Original"; return frame.clone();
    }
}

std::vector<std::pair<std::string, cv::Mat>> processAllFilters(const cv::Mat& frame) {
    std::vector<std::pair<std::string, cv::Mat>> results;
    results.push_back({"Original", frame});
    results.push_back({"Gray", applyGray(frame)});
    results.push_back({"LoG", applyLoG(frame)});
    results.push_back({"Zero Crossing", applyZeroCrossing(frame)});
    results.push_back({"Scharr", applyScharrEdges(frame)});
    results.push_back({"Laplaciano", applyLaplacianEdges(frame)});
    results.push_back({"Sobel Magnitude", applySobelMagnitude(frame)});
    results.push_back({"Canny", applyCannyEdges(frame)});
    return results;
}

void runWithImage(int filterOption, int displayOption) {
    std::string path = openFileDialog();
    if (path.empty()) {
        std::cerr << "No se selecciono ninguna imagen.\n";
        return;
    }

    cv::Mat image = cv::imread(path, cv::IMREAD_COLOR);
    if (image.empty()) {
        std::cerr << "No se pudo cargar la imagen: " << path << "\n";
        return;
    }

    if (filterOption == 8) {
        auto results = processAllFilters(image);

        if (displayOption == 2) {
            std::vector<std::pair<std::string, cv::Mat>> overlayResults;
            overlayResults.push_back({"Original", image});

            for (size_t i = 1; i < results.size(); ++i) {
                if (i == 1) {
                    overlayResults.push_back({results[i].first, results[i].second});
                } else {
                    cv::Mat overlay = overlayEdgesOnImage(image, results[i].second, cv::Vec3b(255, 255, 0), 50);
                    overlayResults.push_back({results[i].first, overlay});
                }
            }

            cv::Mat grid = createGrid(overlayResults, 2);
            cv::imshow("Cuadricula de filtros", grid);
        } else {
            cv::Mat grid = createGrid(results, 2);
            cv::imshow("Cuadricula de filtros", grid);
        }
    } else {
        std::string title;
        cv::Mat result = processSingleFilter(image, filterOption, title);

        if (displayOption == 2 && filterOption != 1) {
            cv::Mat overlay = overlayEdgesOnImage(image, result, cv::Vec3b(255, 255, 0), 50);
            cv::imshow(title + " Overlay", overlay);
        } else {
            cv::imshow(title, result);
        }
    }

    cv::waitKey(0);
    cv::destroyAllWindows();
}

void runWithCamera(int filterOption, int displayOption) {
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "No se pudo abrir la camara.\n";
        return;
    }

    std::cout << "\nPresione 'q' o ESC para salir.\n";

    while (true) {
        cv::Mat frame;
        cap >> frame;
        if (frame.empty()) break;

        if (filterOption == 8) {
            auto results = processAllFilters(frame);

            if (displayOption == 2) {
                std::vector<std::pair<std::string, cv::Mat>> overlayResults;
                overlayResults.push_back({"Original", frame});

                for (size_t i = 1; i < results.size(); ++i) {
                    if (i == 1) {
                        overlayResults.push_back({results[i].first, results[i].second});
                    } else {
                        cv::Mat overlay = overlayEdgesOnImage(frame, results[i].second, cv::Vec3b(255, 255, 0), 50);
                        overlayResults.push_back({results[i].first, overlay});
                    }
                }

                cv::Mat grid = createGrid(overlayResults, 2);
                cv::imshow("Cuadricula de filtros", grid);
            } else {
                cv::Mat grid = createGrid(results, 2);
                cv::imshow("Cuadricula de filtros", grid);
            }
        } else {
            std::string title;
            cv::Mat result = processSingleFilter(frame, filterOption, title);

            if (displayOption == 2 && filterOption != 1) {
                cv::Mat overlay = overlayEdgesOnImage(frame, result, cv::Vec3b(255, 255, 0), 50);
                cv::imshow(title + " Overlay", overlay);
            } else {
                cv::imshow(title, result);
            }
        }

        char key = static_cast<char>(cv::waitKey(30));
        if (key == 'q' || key == 27) {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();
}

int main() {
    int sourceOption = 0;
    int filterOption = 0;
    int displayOption = 0;

    printMenuFuente();
    std::cin >> sourceOption;

    printMenuFiltros();
    std::cin >> filterOption;

    printMenuVisualizacion();
    std::cin >> displayOption;

    if (sourceOption != 1 && sourceOption != 2) {
        std::cerr << "Opcion de fuente invalida.\n";
        return 1;
    }

    if (filterOption < 1 || filterOption > 8) {
        std::cerr << "Opcion de filtro invalida.\n";
        return 1;
    }

    if (displayOption != 1 && displayOption != 2) {
        std::cerr << "Opcion de visualizacion invalida.\n";
        return 1;
    }

    if (sourceOption == 1) {
        runWithCamera(filterOption, displayOption);
    } else {
        runWithImage(filterOption, displayOption);
    }

    return 0;
}