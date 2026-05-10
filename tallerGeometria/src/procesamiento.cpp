#include "geometria.h"

// ─── Ray Casting: punto dentro de polígono ────────────────────────────────────
static bool puntoDentroPoligono(const std::vector<Vec2D>& pts, double px, double py) {
    bool dentro = false;
    int n = (int)pts.size();
    for (int i = 0, j = n-1; i < n; j = i++) {
        double xi = pts[i].x, yi = pts[i].y;
        double xj = pts[j].x, yj = pts[j].y;
        // El rayo horizontal hacia la derecha desde (px,py) cruza la arista
        if (((yi > py) != (yj > py)) &&
            (px < (xj - xi) * (py - yi) / (yj - yi) + xi))
            dentro = !dentro;
    }
    return dentro;
}

// ─── Recortar ROI usando polígono como máscara ────────────────────────────────
static cv::Mat recortarROI(const cv::Mat& src, const std::vector<Vec2D>& pts) {
    cv::Mat dst = cv::Mat::zeros(src.size(), src.type());
    for (int y = 0; y < src.rows; y++)
        for (int x = 0; x < src.cols; x++)
            if (puntoDentroPoligono(pts, x, y))
                dst.at<cv::Vec3b>(y, x) = src.at<cv::Vec3b>(y, x);
    return dst;
}

// ─── Aplicar rotación sólo dentro de la región del polígono ──────────────────
static cv::Mat transformarRegion(const cv::Mat& src,
                                  const std::vector<Vec2D>& pts,
                                  double angulo) {
    static const double PI = std::acos(-1.0);
    double r  = angulo * PI / 180.0;
    double cs = std::cos(r), sn = std::sin(r);

    // Centroide del polígono
    double cx = 0, cy = 0;
    for (auto& p : pts) { cx += p.x; cy += p.y; }
    cx /= pts.size(); cy /= pts.size();

    cv::Mat dst = src.clone();
    for (int y = 0; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
            if (!puntoDentroPoligono(pts, x, y)) continue;
            // Mapeo inverso desde (x,y) hacia el origen
            double dx = x - cx, dy = y - cy;
            double sx =  cs*dx + sn*dy + cx;
            double sy = -sn*dx + cs*dy + cy;
            int ix = (int)std::round(sx), iy = (int)std::round(sy);
            if (ix >= 0 && ix < src.cols && iy >= 0 && iy < src.rows)
                dst.at<cv::Vec3b>(y, x) = src.at<cv::Vec3b>(iy, ix);
        }
    }
    return dst;
}

// ─── Dibujar contorno (Bresenham interno) ────────────────────────────────────
static void dibujarContorno(cv::Mat& img, const std::vector<Vec2D>& pts,
                             cv::Vec3b color) {
    int n = (int)pts.size();
    for (int i = 0; i < n; i++) {
        int j = (i+1)%n;
        int x0=(int)pts[i].x, y0=(int)pts[i].y;
        int x1=(int)pts[j].x, y1=(int)pts[j].y;
        int dx=std::abs(x1-x0), dy=std::abs(y1-y0);
        int sx=x0<x1?1:-1, sy=y0<y1?1:-1, err=dx-dy;
        while (true) {
            setPixel(img, x0, y0, color);
            if (x0==x1 && y0==y1) break;
            int e2=2*err;
            if (e2>-dy){err-=dy;x0+=sx;}
            if (e2< dx){err+=dx;y0+=sy;}
        }
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

// ─── Menú Paso 6 ─────────────────────────────────────────────────────────────
void paso6_procesamiento(cv::Mat& img) {
    int op;
    do {
        std::cout << "\n--- Paso 6: Procesamiento de Imagen ---\n";
        std::cout << "[1] Recortar ROI con poligono (mascara)\n";
        std::cout << "[2] Aplicar rotacion solo dentro de una region\n";
        std::cout << "[3] Detectar si un punto esta dentro de una figura (Ray Casting)\n";
        std::cout << "[0] Volver\n";
        std::cout << "Opcion: ";
        std::cin >> op;

        if (op == 1) {
            std::cout << "\n[Tecnica] Region Of Interest (ROI) con mascara poligonal.\n"
                         "El algoritmo Ray Casting decide si cada pixel de la imagen esta DENTRO\n"
                         "del poligono: lanza un rayo horizontal y cuenta cuantos lados cruza.\n"
                         "  Cruces impares -> pixel DENTRO  (se copia al resultado).\n"
                         "  Cruces pares   -> pixel FUERA   (queda en negro).\n"
                         "Resultado: solo la region interior del poligono es visible.\n";
            auto pts = leerPoligono();
            cv::Mat roi = recortarROI(img, pts);
            mostrarYEsperar("Paso 6 - ROI con poligono", roi);
        }
        else if (op == 2) {
            std::cout << "\n[Tecnica] Transformacion local: rotacion aplicada SOLO dentro del poligono.\n"
                         "El pivote de rotacion es el centroide del poligono (promedio de vertices).\n"
                         "Los pixels exteriores al poligono permanecen intactos en la imagen.\n"
                         "El contorno del poligono se dibuja en amarillo como referencia visual.\n"
                         "Angulo positivo -> sentido antihorario desde el centroide.\n";
            auto pts = leerPoligono();
            double ang;
            std::cout << "Angulo de rotacion (grados): "; std::cin >> ang;
            cv::Mat res = transformarRegion(img, pts, ang);
            dibujarContorno(res, pts, {0, 255, 255});
            mostrarYEsperar("Paso 6 - Region transformada", res);
        }
        else if (op == 3) {
            std::cout << "\n[Algoritmo] Ray Casting para clasificacion punto-poligono:\n"
                         "Desde cada punto de prueba se lanza un rayo horizontal hacia la derecha.\n"
                         "Se cuentan las intersecciones con los lados del poligono:\n"
                         "  Cruces IMPARES -> punto DENTRO  (marcado en verde en la imagen).\n"
                         "  Cruces PARES   -> punto FUERA   (marcado en rojo en la imagen).\n"
                         "Funciona con cualquier poligono, convexo o concavo.\n"
                         "Primero define el poligono, luego ingresa los puntos a verificar.\n";
            auto pts = leerPoligono();

            // Mostrar el polígono primero
            cv::Mat base = img.clone();
            dibujarContorno(base, pts, {0, 255, 0});

            int nPuntos;
            std::cout << "Cuantos puntos quieres verificar: "; std::cin >> nPuntos;
            for (int i = 0; i < nPuntos; i++) {
                double px, py;
                std::cout << "Punto " << i+1 << "  x: "; std::cin >> px;
                std::cout << "Punto " << i+1 << "  y: "; std::cin >> py;

                bool dentro = puntoDentroPoligono(pts, px, py);
                std::cout << "  -> Punto (" << px << ", " << py << "): "
                          << (dentro ? "DENTRO" : "FUERA") << "\n";

                // Verde = dentro, Rojo = fuera
                cv::Vec3b color = dentro ? cv::Vec3b{0,255,0} : cv::Vec3b{0,0,255};
                int r = 6;
                for (int dy=-r; dy<=r; dy++)
                    for (int dx=-r; dx<=r; dx++)
                        if (dx*dx+dy*dy <= r*r)
                            setPixel(base, (int)px+dx, (int)py+dy, color);
            }
            mostrarYEsperar("Paso 6 - Ray Casting (verde=dentro, rojo=fuera)", base);
        }
    } while (op != 0);
}
