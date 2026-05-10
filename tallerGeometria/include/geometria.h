#ifndef GEOMETRIA_H
#define GEOMETRIA_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <cmath>
#include <iostream>
#include <algorithm>

// ─── Estructuras ──────────────────────────────────────────────────────────────

struct Vec2D {
    double x, y;
    Vec2D(double x = 0.0, double y = 0.0) : x(x), y(y) {}
    Vec2D  operator+(const Vec2D& o) const { return {x + o.x, y + o.y}; }
    Vec2D  operator-(const Vec2D& o) const { return {x - o.x, y - o.y}; }
    Vec2D  operator*(double s)        const { return {x * s,   y * s};   }
    double dot(const Vec2D& o)        const { return x*o.x + y*o.y;       }
    double cross(const Vec2D& o)      const { return x*o.y - y*o.x;       }
    double magnitude()                const { return std::sqrt(x*x + y*y); }
    void   print(const std::string& nombre = "") const {
        if (!nombre.empty()) std::cout << nombre << " = ";
        std::cout << "Vec2D(" << x << ", " << y
                  << ")  |magnitud=" << magnitude() << "\n";
    }
};

// Matriz homogénea 3×3 (row-major)
typedef double Mat3[3][3];

// ─── Utilidades inline ────────────────────────────────────────────────────────

inline void setPixel(cv::Mat& img, int x, int y, const cv::Vec3b& c) {
    if (x >= 0 && x < img.cols && y >= 0 && y < img.rows)
        img.at<cv::Vec3b>(y, x) = c;
}

inline void mostrarYEsperar(const std::string& titulo, const cv::Mat& img) {
    cv::imshow(titulo, img);
    std::cout << "Presiona cualquier tecla para cerrar...\n";
    cv::waitKey(0);
    cv::destroyAllWindows();
}

// ─── Declaraciones de pasos ───────────────────────────────────────────────────

void paso1_puntosVectores(cv::Mat& img);
void paso2_lineas(cv::Mat& img);
void paso3_poligonos(cv::Mat& img);
void paso4_circulos(cv::Mat& img);
void paso5_transformaciones(cv::Mat& img);
void paso6_procesamiento(cv::Mat& img);

#endif
