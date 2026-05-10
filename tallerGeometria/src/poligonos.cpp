#include "geometria.h"

// ─── Fórmula de Shoelace para área ────────────────────────────────────────────
static double shoelaceArea(const std::vector<Vec2D>& pts) {
    double area = 0.0;
    int n = (int)pts.size();
    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;
        area += pts[i].x * pts[j].y;
        area -= pts[j].x * pts[i].y;
    }
    return std::abs(area) / 2.0;
}

// ─── Perímetro: suma de longitudes de lados ───────────────────────────────────
static double perimetro(const std::vector<Vec2D>& pts) {
    double perim = 0.0;
    int n = (int)pts.size();
    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;
        double dx = pts[j].x - pts[i].x;
        double dy = pts[j].y - pts[i].y;
        perim += std::sqrt(dx*dx + dy*dy);
    }
    return perim;
}

// ─── Dibujar contorno con Bresenham ──────────────────────────────────────────
static void dibujarContorno(cv::Mat& img, const std::vector<Vec2D>& pts,
                             cv::Vec3b color) {
    int n = (int)pts.size();
    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;
        int x0 = (int)pts[i].x, y0 = (int)pts[i].y;
        int x1 = (int)pts[j].x, y1 = (int)pts[j].y;
        int dx = std::abs(x1-x0), dy = std::abs(y1-y0);
        int sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1;
        int err = dx - dy;
        while (true) {
            setPixel(img, x0, y0, color);
            if (x0 == x1 && y0 == y1) break;
            int e2 = 2*err;
            if (e2 > -dy) { err -= dy; x0 += sx; }
            if (e2 <  dx) { err += dx; y0 += sy; }
        }
    }
    // Marcar vértices
    for (auto& p : pts) {
        for (int dy = -3; dy <= 3; dy++)
            for (int dx = -3; dx <= 3; dx++)
                setPixel(img, (int)p.x+dx, (int)p.y+dy, {0, 0, 255});
    }
}

// ─── Scanline fill (regla par-impar) ─────────────────────────────────────────
static void scanlineFill(cv::Mat& img, const std::vector<Vec2D>& pts,
                          cv::Vec3b color) {
    if (pts.size() < 3) return;
    int ymin = (int)pts[0].y, ymax = (int)pts[0].y;
    for (auto& p : pts) {
        if ((int)p.y < ymin) ymin = (int)p.y;
        if ((int)p.y > ymax) ymax = (int)p.y;
    }
    int n = (int)pts.size();
    for (int y = ymin; y <= ymax; y++) {
        std::vector<int> xs;
        for (int i = 0; i < n; i++) {
            int j = (i + 1) % n;
            double yi = pts[i].y, yj = pts[j].y;
            // Solo aristas que cruzan el scanline actual
            if ((yi <= y && yj > y) || (yj <= y && yi > y)) {
                int x = (int)(pts[i].x + (y - yi) / (yj - yi)
                              * (pts[j].x - pts[i].x));
                xs.push_back(x);
            }
        }
        std::sort(xs.begin(), xs.end());
        // Rellenar entre pares de intersecciones
        for (int k = 0; k + 1 < (int)xs.size(); k += 2)
            for (int x = xs[k]; x <= xs[k+1]; x++)
                setPixel(img, x, y, color);
    }
}

// ─── Leer vértices por terminal ───────────────────────────────────────────────
static std::vector<Vec2D> leerPoligono() {
    int n;
    std::cout << "Numero de vertices: "; std::cin >> n;
    std::vector<Vec2D> pts;
    for (int i = 0; i < n; i++) {
        double x, y;
        std::cout << "  Vertice " << i+1 << "  x: "; std::cin >> x;
        std::cout << "  Vertice " << i+1 << "  y: "; std::cin >> y;
        pts.push_back({x, y});
    }
    return pts;
}

// ─── Menú Paso 3 ─────────────────────────────────────────────────────────────
void paso3_poligonos(cv::Mat& img) {
    int op;
    do {
        std::cout << "\n--- Paso 3: Poligonos ---\n";
        std::cout << "[1] Dibujar contorno de poligono\n";
        std::cout << "[2] Rellenar poligono (scanline)\n";
        std::cout << "[3] Calcular area y perimetro (Shoelace)\n";
        std::cout << "[0] Volver\n";
        std::cout << "Opcion: ";
        std::cin >> op;

        if (op == 1) {
            std::cout << "\n[Algoritmo] Se conectan los vertices del poligono con segmentos Bresenham.\n"
                         "El contorno es la frontera geometrica de la figura (sin relleno interior).\n"
                         "Los vertices se marcan con cuadrados rojos para identificar su posicion.\n"
                         "El contorno se dibuja en color amarillo (cian en BGR).\n"
                         "Ingresa los vertices en orden (horario o antihorario).\n";
            auto pts = leerPoligono();
            cv::Mat res = img.clone();
            dibujarContorno(res, pts, {0, 255, 255});
            mostrarYEsperar("Paso 3 - Contorno", res);
        }
        else if (op == 2) {
            std::cout << "\n[Algoritmo] Scanline Fill (relleno por barrido):\n"
                         "  Para cada fila horizontal (scanline) entre ymin y ymax del poligono:\n"
                         "  1. Se calculan las intersecciones con cada lado del poligono.\n"
                         "  2. Se ordenan las X de interseccion.\n"
                         "  3. Se rellenan los segmentos entre pares de intersecciones (regla par-impar).\n"
                         "Funciona con poligonos convexos Y concavos. Se muestran tambien area y perimetro.\n"
                         "Resultado: poligono relleno en naranja-azul con contorno amarillo.\n";
            auto pts = leerPoligono();
            cv::Mat res = img.clone();
            scanlineFill(res, pts, {255, 100, 0});
            dibujarContorno(res, pts, {0, 255, 255});
            std::cout << "Area     = " << shoelaceArea(pts) << " px^2\n";
            std::cout << "Perimetro = " << perimetro(pts) << " px\n";
            mostrarYEsperar("Paso 3 - Relleno Scanline", res);
        }
        else if (op == 3) {
            std::cout << "\n[Formulas] Metricas geometricas del poligono:\n"
                         "  Area (Shoelace/Gauss): A = 0.5 * |sum(xi*yj - xj*yi)|  (vertices en orden).\n"
                         "  Perimetro: suma de las longitudes euclidianas de cada lado.\n"
                         "  Circularidad: C = (4 * pi * A) / P^2  ->  rango [0, 1].\n"
                         "    C = 1.0 -> circulo perfecto.  C cerca de 0 -> forma muy irregular.\n"
                         "No se modifica la imagen; solo se calculan y muestran los valores.\n";
            auto pts = leerPoligono();
            double area  = shoelaceArea(pts);
            double perim = perimetro(pts);
            std::cout << "Area (Shoelace) = " << area  << " px^2\n";
            std::cout << "Perimetro       = " << perim  << " px\n";
            if (perim > 0) {
                double circ = (4.0 * std::acos(-1.0) * area) / (perim * perim);
                std::cout << "Circularidad    = " << circ
                          << "  (1.0 = circulo perfecto)\n";
            }
        }
    } while (op != 0);
}
