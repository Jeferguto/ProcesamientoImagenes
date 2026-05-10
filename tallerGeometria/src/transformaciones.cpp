#include "geometria.h"

// ─── Multiplicación de matrices 3×3 ──────────────────────────────────────────
static void matMul(const Mat3 A, const Mat3 B, Mat3 C) {
    double tmp[3][3] = {};
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            for (int k = 0; k < 3; k++)
                tmp[i][j] += A[i][k] * B[k][j];
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            C[i][j] = tmp[i][j];
}

// ─── Inversa 3×3 por método de cofactores ────────────────────────────────────
static bool matInv(const Mat3 M, Mat3 inv) {
    double det = M[0][0]*(M[1][1]*M[2][2] - M[1][2]*M[2][1])
               - M[0][1]*(M[1][0]*M[2][2] - M[1][2]*M[2][0])
               + M[0][2]*(M[1][0]*M[2][1] - M[1][1]*M[2][0]);
    if (std::abs(det) < 1e-10) return false;

    inv[0][0] =  (M[1][1]*M[2][2] - M[1][2]*M[2][1]) / det;
    inv[0][1] = -(M[0][1]*M[2][2] - M[0][2]*M[2][1]) / det;
    inv[0][2] =  (M[0][1]*M[1][2] - M[0][2]*M[1][1]) / det;
    inv[1][0] = -(M[1][0]*M[2][2] - M[1][2]*M[2][0]) / det;
    inv[1][1] =  (M[0][0]*M[2][2] - M[0][2]*M[2][0]) / det;
    inv[1][2] = -(M[0][0]*M[1][2] - M[0][2]*M[1][0]) / det;
    inv[2][0] =  (M[1][0]*M[2][1] - M[1][1]*M[2][0]) / det;
    inv[2][1] = -(M[0][0]*M[2][1] - M[0][1]*M[2][0]) / det;
    inv[2][2] =  (M[0][0]*M[1][1] - M[0][1]*M[1][0]) / det;
    return true;
}

// ─── Aplicar transformación: mapeo inverso (nearest-neighbor) ─────────────────
static cv::Mat aplicar(const cv::Mat& src, const Mat3 M) {
    Mat3 inv;
    if (!matInv(M, inv)) {
        std::cerr << "Matriz singular, transformacion imposible.\n";
        return src.clone();
    }
    cv::Mat dst = cv::Mat::zeros(src.size(), src.type());
    for (int y = 0; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
            // Transformación inversa: (x',y') → (x,y) en el origen
            double w  = inv[2][0]*x + inv[2][1]*y + inv[2][2];
            double sx = (inv[0][0]*x + inv[0][1]*y + inv[0][2]) / w;
            double sy = (inv[1][0]*x + inv[1][1]*y + inv[1][2]) / w;
            int ix = (int)std::round(sx), iy = (int)std::round(sy);
            if (ix >= 0 && ix < src.cols && iy >= 0 && iy < src.rows)
                dst.at<cv::Vec3b>(y, x) = src.at<cv::Vec3b>(iy, ix);
        }
    }
    return dst;
}

// ─── Imprimir matriz 3×3 ─────────────────────────────────────────────────────
static void printMat(const Mat3 M, const std::string& nombre) {
    std::cout << nombre << ":\n";
    for (int i = 0; i < 3; i++) {
        std::cout << "  [";
        for (int j = 0; j < 3; j++)
            std::cout << "\t" << M[i][j];
        std::cout << "\t]\n";
    }
}

// ─── Menú Paso 5 ─────────────────────────────────────────────────────────────
void paso5_transformaciones(cv::Mat& img) {
    static const double PI = std::acos(-1.0);
    int op;
    do {
        std::cout << "\n--- Paso 5: Transformaciones (Matrices) ---\n";
        std::cout << "[1] Traslacion\n";
        std::cout << "[2] Rotacion (alrededor del centro)\n";
        std::cout << "[3] Escalado\n";
        std::cout << "[4] Composicion (traslacion + rotacion + escalado)\n";
        std::cout << "[0] Volver\n";
        std::cout << "Opcion: ";
        std::cin >> op;

        if (op == 1) {
            std::cout << "\n[Transformacion] Traslacion con coordenadas homogeneas.\n"
                         "Matriz 3x3 aplicada a cada pixel:\n"
                         "  | 1  0  tx |   | x |   | x + tx |\n"
                         "  | 0  1  ty | * | y | = | y + ty |\n"
                         "  | 0  0   1 |   | 1 |   |   1    |\n"
                         "Se usa mapeo INVERSO: por cada pixel destino se busca su origen.\n"
                         "La imagen resultante REEMPLAZA la actual (los cambios se acumulan).\n";
            double tx, ty;
            std::cout << "tx (desplazamiento x): "; std::cin >> tx;
            std::cout << "ty (desplazamiento y): "; std::cin >> ty;
            Mat3 T = {{1,0,tx},
                      {0,1,ty},
                      {0,0, 1}};
            printMat(T, "Matriz Traslacion T");
            img = aplicar(img, T);
            mostrarYEsperar("Paso 5 - Traslacion", img);
        }
        else if (op == 2) {
            std::cout << "\n[Transformacion] Rotacion alrededor del CENTRO de la imagen.\n"
                         "Matriz compuesta M = T(centro) * R(theta) * T(-centro):\n"
                         "  R(theta) = | cos  -sin  0 |\n"
                         "             | sin   cos  0 |\n"
                         "             |  0     0   1 |\n"
                         "Angulo positivo -> sentido antihorario.\n"
                         "Angulo negativo -> sentido horario.\n"
                         "La imagen resultante REEMPLAZA la actual.\n";
            double ang;
            std::cout << "Angulo en grados: "; std::cin >> ang;
            double r  = ang * PI / 180.0;
            double cs = std::cos(r), sn = std::sin(r);
            double cx = img.cols / 2.0, cy = img.rows / 2.0;

            // M = T(cx,cy) · R(θ) · T(-cx,-cy)
            Mat3 T1  = {{1,0,-cx},{0,1,-cy},{0,0,1}};
            Mat3 R   = {{cs,-sn,0},{sn,cs,0},{0,0,1}};
            Mat3 T2  = {{1,0, cx},{0,1, cy},{0,0,1}};
            Mat3 tmp, M;
            matMul(R, T1, tmp);
            matMul(T2, tmp, M);
            printMat(M, "Matriz Rotacion M = T2 · R · T1");
            img = aplicar(img, M);
            mostrarYEsperar("Paso 5 - Rotacion", img);
        }
        else if (op == 3) {
            std::cout << "\n[Transformacion] Escalado desde el origen (0,0).\n"
                         "  | sx  0  0 |   sx = sy -> isotropico (proporcional).\n"
                         "  |  0 sy  0 |   sx != sy -> anisotropico (distorsiona).\n"
                         "  |  0  0  1 |   Valores < 1 reducen; valores > 1 amplian.\n"
                         "AVISO: el escalado se aplica respecto al origen, no al centro;\n"
                         "la imagen puede desplazarse visualmente hacia una esquina.\n";
            double sx, sy;
            std::cout << "Factor escala x (ej: 0.5 = mitad, 2.0 = doble): "; std::cin >> sx;
            std::cout << "Factor escala y: "; std::cin >> sy;
            Mat3 S = {{sx, 0, 0},
                      { 0,sy, 0},
                      { 0, 0, 1}};
            printMat(S, "Matriz Escalado S");
            img = aplicar(img, S);
            mostrarYEsperar("Paso 5 - Escalado", img);
        }
        else if (op == 4) {
            std::cout << "\n[Transformacion] Composicion de transformaciones: M = T * R * S\n"
                         "El orden de aplicacion es: primero Escalado, luego Rotacion, luego Traslacion.\n"
                         "Multiplicar las matrices permite combinar las tres operaciones en una sola\n"
                         "pasada sobre los pixeles, lo que es mucho mas eficiente que aplicarlas\n"
                         "una por una. La matriz compuesta se imprime antes de aplicarla.\n";
            double tx, ty, ang, sx, sy;
            std::cout << "tx: "; std::cin >> tx;
            std::cout << "ty: "; std::cin >> ty;
            std::cout << "Angulo (grados): "; std::cin >> ang;
            std::cout << "sx: "; std::cin >> sx;
            std::cout << "sy: "; std::cin >> sy;

            double r  = ang * PI / 180.0;
            double cs = std::cos(r), sn = std::sin(r);
            Mat3 T = {{1,0,tx},  {0,1,ty},  {0,0,1}};
            Mat3 R = {{cs,-sn,0},{sn,cs,0}, {0,0,1}};
            Mat3 S = {{sx,0,0},  {0,sy,0},  {0,0,1}};
            Mat3 tmp, M;
            matMul(R, S, tmp);   // R·S
            matMul(T, tmp, M);   // T·R·S
            printMat(M, "Matriz compuesta M = T · R · S");
            img = aplicar(img, M);
            mostrarYEsperar("Paso 5 - Composicion", img);
        }
    } while (op != 0);
}
