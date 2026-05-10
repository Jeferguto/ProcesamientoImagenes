#include "geometria.h"

// ─── Desplazar imagen píxel a píxel con vector (dx, dy) ──────────────────────
static cv::Mat desplazarImagen(const cv::Mat& src, int dx, int dy) {
    cv::Mat dst = cv::Mat::zeros(src.size(), src.type());
    for (int y = 0; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
            int nx = x + dx, ny = y + dy;
            if (nx >= 0 && nx < src.cols && ny >= 0 && ny < src.rows)
                dst.at<cv::Vec3b>(ny, nx) = src.at<cv::Vec3b>(y, x);
        }
    }
    return dst;
}

// ─── Dibujar punto relleno (sin cv::circle) ───────────────────────────────────
static void dibujarPunto(cv::Mat& img, int cx, int cy, int r, cv::Vec3b color) {
    for (int dy = -r; dy <= r; dy++)
        for (int dx = -r; dx <= r; dx++)
            if (dx*dx + dy*dy <= r*r)
                setPixel(img, cx + dx, cy + dy, color);
}

// ─── Menú Paso 1 ─────────────────────────────────────────────────────────────
void paso1_puntosVectores(cv::Mat& img) {
    int op;
    do {
        std::cout << "\n--- Paso 1: Puntos y Vectores ---\n";
        std::cout << "[1] Consultar color de pixel (x, y)\n";
        std::cout << "[2] Desplazar imagen con vector (dx, dy)\n";
        std::cout << "[3] Marcar punto en la imagen\n";
        std::cout << "[4] Operaciones entre vectores (suma, resta, dot, cross)\n";
        std::cout << "[0] Volver\n";
        std::cout << "Opcion: ";
        std::cin >> op;

        if (op == 1) {
            std::cout << "\n[Concepto] Cada pixel almacena su color como tres intensidades BGR\n"
                         "(Azul, Verde, Rojo), cada una en rango 0-255. B=0,G=0,R=255 es rojo puro.\n"
                         "El origen (0,0) esta en la esquina superior izquierda de la imagen.\n";
            int x, y;
            std::cout << "x: "; std::cin >> x;
            std::cout << "y: "; std::cin >> y;
            if (x < 0 || x >= img.cols || y < 0 || y >= img.rows) {
                std::cout << "Coordenadas fuera de rango (imagen "
                          << img.cols << "x" << img.rows << ").\n";
                continue;
            }
            cv::Vec3b px = img.at<cv::Vec3b>(y, x);
            std::cout << "Pixel (" << x << ", " << y << ") -> "
                      << "B=" << (int)px[0]
                      << "  G=" << (int)px[1]
                      << "  R=" << (int)px[2] << "\n";
            Vec2D p(x, y);
            p.print("Punto como Vec2D");
        }
        else if (op == 2) {
            std::cout << "\n[Concepto] Un vector de traslacion (dx, dy) mueve cada pixel de\n"
                         "(x, y) a (x+dx, y+dy). Los pixeles que salen del borde se pierden\n"
                         "y la region vacia queda en negro. Es la operacion mas simple de\n"
                         "transformacion geometrica.\n";
            int dx, dy;
            std::cout << "dx (horizontal): "; std::cin >> dx;
            std::cout << "dy (vertical):   "; std::cin >> dy;
            Vec2D v(dx, dy);
            v.print("Vector de desplazamiento");
            img = desplazarImagen(img, dx, dy);
            mostrarYEsperar("Paso 1 - Desplazamiento", img);
        }
        else if (op == 3) {
            std::cout << "\n[Concepto] Un punto en imagen se representa como (columna, fila).\n"
                         "Se visualiza como un disco relleno usando la ecuacion del circulo:\n"
                         "  dx^2 + dy^2 <= r^2  para cada pixel en el vecindario del centro.\n"
                         "El punto se dibuja en rojo sobre una copia de la imagen original.\n";
            int x, y, r;
            std::cout << "x del punto:   "; std::cin >> x;
            std::cout << "y del punto:   "; std::cin >> y;
            std::cout << "Radio en px:   "; std::cin >> r;
            cv::Mat res = img.clone();
            dibujarPunto(res, x, y, r, {0, 0, 255});
            Vec2D p(x, y);
            p.print("Punto marcado");
            mostrarYEsperar("Paso 1 - Punto", res);
        }
        else if (op == 4) {
            std::cout << "\n[Concepto] Operaciones fundamentales entre vectores 2D:\n"
                         "  Suma/Resta    : combina desplazamientos.\n"
                         "  Escalar * 2   : duplica la magnitud, misma direccion.\n"
                         "  Dot product   : A*B = |A||B|cos(angulo) -> 0 si son perpendiculares.\n"
                         "  Cross product : A x B = Ax*By - Ay*Bx -> signo indica orientacion\n"
                         "                  (positivo=B esta a la izquierda de A, negativo=derecha).\n";
            double ax, ay, bx, by;
            std::cout << "Vector A - x: "; std::cin >> ax;
            std::cout << "Vector A - y: "; std::cin >> ay;
            std::cout << "Vector B - x: "; std::cin >> bx;
            std::cout << "Vector B - y: "; std::cin >> by;
            Vec2D a(ax, ay), b(bx, by);
            a.print("A");
            b.print("B");
            (a + b).print("A + B");
            (a - b).print("A - B");
            (a * 2.0).print("A * 2");
            std::cout << "A · B (dot product)   = " << a.dot(b)   << "\n";
            std::cout << "A × B (cross product)  = " << a.cross(b) << "\n";
        }
    } while (op != 0);
}
