#include "geometria.h"

// ─── Plota los 8 octantes simétricos del círculo ─────────────────────────────
static void plotOctantes(cv::Mat& img, int cx, int cy, int x, int y,
                          cv::Vec3b color) {
    setPixel(img, cx+x, cy+y, color); setPixel(img, cx-x, cy+y, color);
    setPixel(img, cx+x, cy-y, color); setPixel(img, cx-x, cy-y, color);
    setPixel(img, cx+y, cy+x, color); setPixel(img, cx-y, cy+x, color);
    setPixel(img, cx+y, cy-x, color); setPixel(img, cx-y, cy-x, color);
}

// ─── Algoritmo de Bresenham para círculos ─────────────────────────────────────
static void bresenhamCircle(cv::Mat& img, int cx, int cy, int r,
                             cv::Vec3b color) {
    int x = 0, y = r, d = 3 - 2*r;
    while (x <= y) {
        plotOctantes(img, cx, cy, x, y, color);
        if (d < 0) {
            d += 4*x + 6;
        } else {
            d += 4*(x - y) + 10;
            y--;
        }
        x++;
    }
}

// ─── Algoritmo Midpoint para elipses ─────────────────────────────────────────
static void midpointElipse(cv::Mat& img, int cx, int cy, int rx, int ry,
                            cv::Vec3b color) {
    auto plot4 = [&](int x, int y) {
        setPixel(img, cx+x, cy+y, color); setPixel(img, cx-x, cy+y, color);
        setPixel(img, cx+x, cy-y, color); setPixel(img, cx-x, cy-y, color);
    };

    double rx2 = (double)rx*rx, ry2 = (double)ry*ry;
    double x = 0, y = (double)ry;
    double dx = 2.0*ry2*x, dy = 2.0*rx2*y;

    // Región 1: pendiente < 1 (se mueve más en X)
    double p = ry2 - rx2*ry + 0.25*rx2;
    while (dx < dy) {
        plot4((int)x, (int)y);
        x++;
        dx = 2.0*ry2*x;
        if (p < 0) {
            p += ry2 + dx;
        } else {
            y--;
            dy = 2.0*rx2*y;
            p += ry2 + dx - dy;
        }
    }

    // Región 2: pendiente > 1 (se mueve más en Y)
    p = ry2*(x+0.5)*(x+0.5) + rx2*(y-1.0)*(y-1.0) - rx2*ry2;
    while (y >= 0) {
        plot4((int)x, (int)y);
        y--;
        dy = 2.0*rx2*y;
        if (p > 0) {
            p += rx2 - dy;
        } else {
            x++;
            dx = 2.0*ry2*x;
            p += rx2 + dx - dy;
        }
    }
}

// ─── Menú Paso 4 ─────────────────────────────────────────────────────────────
void paso4_circulos(cv::Mat& img) {
    int op;
    do {
        std::cout << "\n--- Paso 4: Circulos y Elipses ---\n";
        std::cout << "[1] Dibujar circulo (Bresenham)\n";
        std::cout << "[2] Dibujar elipse (Midpoint)\n";
        std::cout << "[3] Dibujar circulos concentricos\n";
        std::cout << "[0] Volver\n";
        std::cout << "Opcion: ";
        std::cin >> op;

        if (op == 1) {
            std::cout << "\n[Algoritmo] Bresenham para circulos: aprovecha la simetria de 8 octantes.\n"
                         "Solo calcula los pixeles del primer octante y los refleja a los otros 7,\n"
                         "usando una variable de decision entera para elegir entre el pixel\n"
                         "horizontal o el diagonal en cada paso.\n"
                         "El centro se marca en rojo. Se calculan Area = pi*r^2 y Perimetro = 2*pi*r.\n";
            int cx, cy, r;
            std::cout << "Centro x: "; std::cin >> cx;
            std::cout << "Centro y: "; std::cin >> cy;
            std::cout << "Radio:    "; std::cin >> r;
            cv::Mat res = img.clone();
            bresenhamCircle(res, cx, cy, r, {0, 255, 0});
            // Marcar centro
            for (int dy=-3; dy<=3; dy++)
                for (int dx=-3; dx<=3; dx++)
                    setPixel(res, cx+dx, cy+dy, {0,0,255});
            double area = std::acos(-1.0) * r * r;
            std::cout << "Radio     = " << r << " px\n";
            std::cout << "Area      = " << area << " px^2\n";
            std::cout << "Perimetro = " << 2.0*std::acos(-1.0)*r << " px\n";
            mostrarYEsperar("Paso 4 - Circulo (Bresenham)", res);
        }
        else if (op == 2) {
            std::cout << "\n[Algoritmo] Midpoint para elipses: divide el trazado en dos regiones\n"
                         "segun la pendiente de la curva:\n"
                         "  Region 1: |pendiente| < 1  ->  avanza principalmente en X.\n"
                         "  Region 2: |pendiente| > 1  ->  avanza principalmente en Y.\n"
                         "La funcion de decision se basa en la ecuacion (x/rx)^2 + (y/ry)^2 = 1.\n"
                         "Area aproximada = pi * rx * ry  (exacta si rx = ry = radio del circulo).\n"
                         "El centro se marca en rojo. rx = semieje horizontal, ry = semieje vertical.\n";
            int cx, cy, rx, ry;
            std::cout << "Centro x:  "; std::cin >> cx;
            std::cout << "Centro y:  "; std::cin >> cy;
            std::cout << "Radio eje X: "; std::cin >> rx;
            std::cout << "Radio eje Y: "; std::cin >> ry;
            cv::Mat res = img.clone();
            midpointElipse(res, cx, cy, rx, ry, {255, 100, 0});
            for (int dy=-3; dy<=3; dy++)
                for (int dx=-3; dx<=3; dx++)
                    setPixel(res, cx+dx, cy+dy, {0,0,255});
            double PI = std::acos(-1.0);
            double area = PI * rx * ry;
            std::cout << "Area aprox. = " << area << " px^2\n";
            mostrarYEsperar("Paso 4 - Elipse (Midpoint)", res);
        }
        else if (op == 3) {
            std::cout << "\n[Concepto] Circulos concentricos: mismo centro, radios crecientes.\n"
                         "Los radios se distribuyen uniformemente desde rmax/n hasta rmax.\n"
                         "Cada circulo se traza con Bresenham en un color diferente\n"
                         "(verde, azul, rojo, cian, magenta, amarillo) para distinguirlos.\n";
            int cx, cy, rmax, n;
            std::cout << "Centro x:        "; std::cin >> cx;
            std::cout << "Centro y:        "; std::cin >> cy;
            std::cout << "Radio maximo:    "; std::cin >> rmax;
            std::cout << "Cantidad de circulos: "; std::cin >> n;
            cv::Mat res = img.clone();
            cv::Vec3b colores[] = {{0,255,0},{0,0,255},{255,0,0},
                                    {0,255,255},{255,0,255},{255,255,0}};
            for (int i = 1; i <= n; i++) {
                int r = (rmax * i) / n;
                bresenhamCircle(res, cx, cy, r, colores[i % 6]);
            }
            mostrarYEsperar("Paso 4 - Circulos concentricos", res);
        }
    } while (op != 0);
}
