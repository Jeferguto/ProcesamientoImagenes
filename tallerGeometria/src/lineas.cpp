#include "geometria.h"

// ─── Algoritmo de Bresenham para líneas ───────────────────────────────────────
static void bresenhamLine(cv::Mat& img, int x0, int y0, int x1, int y1,
                          cv::Vec3b color, int grosor = 1) {
    int dx = std::abs(x1 - x0), dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        for (int gy = -grosor/2; gy <= grosor/2; gy++)
            for (int gx = -grosor/2; gx <= grosor/2; gx++)
                setPixel(img, x0 + gx, y0 + gy, color);

        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 <  dx) { err += dx; y0 += sy; }
    }
}

// ─── Detección de bordes: gradiente Sobel implementado manualmente ────────────
static cv::Mat detectarBordes(const cv::Mat& src, int umbral) {
    // Convertir a escala de grises (manual, sin cv::cvtColor para el cálculo)
    cv::Mat gray(src.rows, src.cols, CV_32FC1);
    for (int y = 0; y < src.rows; y++)
        for (int x = 0; x < src.cols; x++) {
            cv::Vec3b px = src.at<cv::Vec3b>(y, x);
            // Luminancia: 0.114*B + 0.587*G + 0.299*R
            gray.at<float>(y, x) = 0.114f*(float)px[0]
                                 + 0.587f*(float)px[1]
                                 + 0.299f*(float)px[2];
        }

    // Kernels Sobel 3×3 (implementados como arrays, no importados)
    const int Kx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    const int Ky[3][3] = {{-1,-2,-1}, { 0, 0, 0}, { 1, 2, 1}};

    cv::Mat bordes = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);
    for (int y = 1; y < src.rows - 1; y++) {
        for (int x = 1; x < src.cols - 1; x++) {
            double gx = 0, gy = 0;
            for (int ky = -1; ky <= 1; ky++)
                for (int kx = -1; kx <= 1; kx++) {
                    float v = gray.at<float>(y + ky, x + kx);
                    gx += Kx[ky+1][kx+1] * v;
                    gy += Ky[ky+1][kx+1] * v;
                }
            double mag = std::sqrt(gx*gx + gy*gy);
            bordes.at<uchar>(y, x) = (mag > umbral) ? 255 : 0;
        }
    }
    return bordes;
}

// ─── Información del segmento ────────────────────────────────────────────────
static void infoSegmento(int x0, int y0, int x1, int y1) {
    Vec2D a(x0, y0), b(x1, y1);
    Vec2D seg = b - a;
    std::cout << "Segmento AB:\n";
    a.print("  A");
    b.print("  B");
    seg.print("  AB (vector)");
    std::cout << "  Longitud = " << seg.magnitude() << " px\n";
    std::cout << "  Pendiente = " << (x1 != x0 ? (double)(y1-y0)/(x1-x0) : 1e9) << "\n";
}

// ─── Menú Paso 2 ─────────────────────────────────────────────────────────────
void paso2_lineas(cv::Mat& img) {
    int op;
    do {
        std::cout << "\n--- Paso 2: Lineas y Segmentos ---\n";
        std::cout << "[1] Dibujar linea entre dos puntos (Bresenham)\n";
        std::cout << "[2] Detectar bordes (gradiente Sobel manual)\n";
        std::cout << "[3] Dibujar multiple lineas\n";
        std::cout << "[0] Volver\n";
        std::cout << "Opcion: ";
        std::cin >> op;

        if (op == 1) {
            std::cout << "\n[Algoritmo] Bresenham (1965): traza una linea entre dos puntos usando\n"
                         "solo aritmetica entera. En cada paso decide si el siguiente pixel va\n"
                         "en la direccion principal o en la diagonal, acumulando el error residual.\n"
                         "Resultado: linea verde dibujada pixel a pixel. Se muestra info del segmento\n"
                         "(longitud euclidiana, vector AB y pendiente) antes de mostrar la imagen.\n";
            int x0, y0, x1, y1, grosor;
            std::cout << "Inicio  x0: "; std::cin >> x0;
            std::cout << "Inicio  y0: "; std::cin >> y0;
            std::cout << "Fin     x1: "; std::cin >> x1;
            std::cout << "Fin     y1: "; std::cin >> y1;
            std::cout << "Grosor (1=fino): "; std::cin >> grosor;
            infoSegmento(x0, y0, x1, y1);
            cv::Mat res = img.clone();
            bresenhamLine(res, x0, y0, x1, y1, {0, 255, 0}, grosor);
            mostrarYEsperar("Paso 2 - Bresenham", res);
        }
        else if (op == 2) {
            std::cout << "\n[Algoritmo] Deteccion de bordes con gradiente Sobel (implementado manualmente):\n"
                         "  1. Se convierte la imagen a escala de grises (luminancia: 0.299R+0.587G+0.114B).\n"
                         "  2. Se aplican dos kernels 3x3 (Kx y Ky) para calcular el gradiente horizontal y vertical.\n"
                         "  3. Magnitud = sqrt(Gx^2 + Gy^2). Si supera el umbral, el pixel se marca blanco.\n"
                         "Umbral bajo (20-50)  : detecta muchos bordes, incluye ruido.\n"
                         "Umbral alto (100-200): solo bordes fuertes y bien definidos.\n"
                         "Resultado: imagen binaria (blanco=borde, negro=zona uniforme).\n";
            int umbral;
            std::cout << "Umbral de magnitud (sugerido 50-150): "; std::cin >> umbral;
            cv::Mat bordes = detectarBordes(img, umbral);
            mostrarYEsperar("Paso 2 - Bordes Sobel (manual)", bordes);
        }
        else if (op == 3) {
            std::cout << "\n[Concepto] Se trazaran multiples lineas independientes con Bresenham,\n"
                         "cada una en un color distinto (verde, rojo, azul, cian, magenta) para\n"
                         "distinguirlas visualmente. Todas se dibujan sobre la misma imagen base.\n";
            int n;
            std::cout << "Cuantas lineas quieres dibujar: "; std::cin >> n;
            cv::Mat res = img.clone();
            for (int i = 0; i < n; i++) {
                int x0, y0, x1, y1;
                std::cout << "Linea " << i+1 << " - x0: "; std::cin >> x0;
                std::cout << "Linea " << i+1 << " - y0: "; std::cin >> y0;
                std::cout << "Linea " << i+1 << " - x1: "; std::cin >> x1;
                std::cout << "Linea " << i+1 << " - y1: "; std::cin >> y1;
                // Colores distintos por índice
                cv::Vec3b colores[] = {{0,255,0},{0,0,255},{255,0,0},{0,255,255},{255,0,255}};
                bresenhamLine(res, x0, y0, x1, y1, colores[i % 5], 2);
            }
            mostrarYEsperar("Paso 2 - Multiples lineas", res);
        }
    } while (op != 0);
}
