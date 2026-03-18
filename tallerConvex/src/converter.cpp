#include "converter.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iostream>

// ─────────────────────────────────────────
// GRAHAM SCAN
// ─────────────────────────────────────────

static double crossProduct(const Point2D& O, const Point2D& A, const Point2D& B) {
    return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

static double distSq(const Point2D& a, const Point2D& b) {
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

std::vector<Point2D> grahamScanHull(std::vector<Point2D>& points) {
    int n = points.size();
    if (n < 3) return points;

    int pivot = 0;
    for (int i = 1; i < n; i++) {
        if (points[i].y < points[pivot].y ||
           (points[i].y == points[pivot].y && points[i].x < points[pivot].x))
            pivot = i;
    }
    std::swap(points[0], points[pivot]);
    Point2D p0 = points[0];

    std::sort(points.begin() + 1, points.end(), [&](const Point2D& a, const Point2D& b) {
        double cp = crossProduct(p0, a, b);
        if (cp != 0) return cp > 0;
        return distSq(p0, a) < distSq(p0, b);
    });

    std::vector<Point2D> hull;
    hull.push_back(points[0]);
    hull.push_back(points[1]);

    for (int i = 2; i < n; i++) {
        while (hull.size() > 1 &&
               crossProduct(hull[hull.size()-2], hull.back(), points[i]) <= 0)
            hull.pop_back();
        hull.push_back(points[i]);
    }

    return hull;
}

// ─────────────────────────────────────────
// CLASIFICACIÓN DE FIGURA
// ─────────────────────────────────────────

std::string classifyShape(const std::vector<Point2D>& hull, const std::vector<cv::Point>& contour) {
    int vertices = hull.size();

    // Detectar círculo/elipse comparando área del hull vs área del círculo equivalente
    double area = cv::contourArea(contour);
    double perimeter = cv::arcLength(contour, true);
    double circularity = (perimeter > 0) ? (4.0 * CV_PI * area) / (perimeter * perimeter) : 0;

    if (circularity > 0.80) return "Circulo";

    switch (vertices) {
        case 3:  return "Triangulo";
        case 4: {
            // Distinguir cuadrado de rectángulo usando bounding box
            cv::Rect bb = cv::boundingRect(contour);
            double ratio = (double)bb.width / bb.height;
            return (ratio >= 0.85 && ratio <= 1.15) ? "Cuadrado" : "Rectangulo";
        }
        case 5:  return "Pentagono";
        case 6:  return "Hexagono";
        case 7:  return "Heptagono";
        case 8:  return "Octagon";
        default:
            if (vertices > 8 && circularity > 0.65) return "Circulo aprox.";
            return "Poligono (" + std::to_string(vertices) + " lados)";
    }
}

// ─────────────────────────────────────────
// PREPROCESAMIENTO MEJORADO
// ─────────────────────────────────────────

cv::Mat preprocessImage(const cv::Mat& src) {
    cv::Mat gray, blurred, edges, binary;

    cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);

    // Gaussiano más suave para preservar bordes curvos
    cv::GaussianBlur(gray, blurred, cv::Size(7, 7), 2.0);

    // Canny con umbrales ajustados para capturar más detalles
    cv::Canny(blurred, edges, 30, 100);

    // Dilatar más para cerrar contornos abiertos (mejora círculos)
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
    cv::dilate(edges, binary, kernel, cv::Point(-1,-1), 2);

    // Cerrar huecos internos
    cv::morphologyEx(binary, binary, cv::MORPH_CLOSE, kernel);

    return binary;
}

// ─────────────────────────────────────────
// DIBUJADO
// ─────────────────────────────────────────

void drawHull(cv::Mat& frame, const std::vector<Point2D>& hull, const cv::Scalar& color) {
    if (hull.size() < 2) return;
    for (size_t i = 0; i < hull.size(); i++) {
        cv::Point p1((int)hull[i].x, (int)hull[i].y);
        cv::Point p2((int)hull[(i+1) % hull.size()].x, (int)hull[(i+1) % hull.size()].y);
        cv::line(frame, p1, p2, color, 2);
        cv::circle(frame, p1, 4, cv::Scalar(0, 0, 255), -1);
    }
}

// ─────────────────────────────────────────
// ZENITY
// ─────────────────────────────────────────

std::string openFileDialog() {
    char filename[2048] = {0};
    FILE* f = popen("zenity --file-selection --title=\"Seleccionar imagen\" "
                    "--file-filter=\"Imagenes | *.png *.jpg *.jpeg *.bmp\"", "r");
    if (!f) return "";
    if (fgets(filename, sizeof(filename), f)) {
        std::string path(filename);
        if (!path.empty() && path.back() == '\n') path.pop_back();
        pclose(f);
        return path;
    }
    pclose(f);
    return "";
}

// ─────────────────────────────────────────
// PROCESAMIENTO PRINCIPAL
// ─────────────────────────────────────────

void processFrame(cv::Mat& frame) {
    cv::Mat binary = preprocessImage(frame);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(binary, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Colores por tipo de figura
    auto getColor = [](const std::string& shape) -> cv::Scalar {
        if (shape == "Triangulo")               return cv::Scalar(0, 165, 255);  // naranja
        if (shape == "Cuadrado")                return cv::Scalar(255, 0, 0);    // azul
        if (shape == "Rectangulo")              return cv::Scalar(255, 255, 0);  // cian
        if (shape == "Pentagono")               return cv::Scalar(0, 255, 255);  // amarillo
        if (shape == "Hexagono")                return cv::Scalar(255, 0, 255);  // magenta
        if (shape.find("Circulo") != std::string::npos) return cv::Scalar(0, 255, 0); // verde
        return cv::Scalar(200, 200, 200);       // gris para el resto
    };

    for (auto& contour : contours) {
        if (cv::contourArea(contour) < 800) continue;

        std::vector<Point2D> pts;
        for (auto& pt : contour)
            pts.push_back({(double)pt.x, (double)pt.y});

        std::vector<Point2D> hull = grahamScanHull(pts);
        if (hull.size() < 3) continue;

        std::string shape = classifyShape(hull, contour);
        cv::Scalar color = getColor(shape);

        drawHull(frame, hull, color);

        // Calcular centroide del hull para poner la etiqueta
        double cx = 0, cy = 0;
        for (auto& p : hull) { cx += p.x; cy += p.y; }
        cx /= hull.size(); cy /= hull.size();

        cv::putText(frame, shape,
                    cv::Point((int)cx - 40, (int)cy),
                    cv::FONT_HERSHEY_SIMPLEX, 0.6, color, 2);
    }

    cv::putText(frame, "Graham Scan - Convex Hull",
                cv::Point(10, 25), cv::FONT_HERSHEY_SIMPLEX,
                0.7, cv::Scalar(255, 255, 0), 2);
}
