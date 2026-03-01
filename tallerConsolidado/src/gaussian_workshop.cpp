#include "gaussian_workshop.h"
#include <iostream>
#include <cstdio>
#include <cmath>

using namespace std;

static string trimNewlines(string s) {
    while (!s.empty() && (s.back() == '\n' || s.back() == '\r')) s.pop_back();
    return s;
}

std::string pickImageWithZenity() {
    // Nota: si zenity no está instalado, el comando fallará y retornará vacío.
    const char* cmd = "zenity --file-selection --title=\"Selecciona la imagen\" 2>/dev/null";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "";

    string result;
    char buffer[512];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);

    return trimNewlines(result); // "" si cancelaste
}

void printImageInfo(const cv::Mat& img) {
    cout << "Filas: " << img.rows << "\n";
    cout << "Columnas: " << img.cols << "\n";
    cout << "Canales: " << img.channels() << "\n";
    cout << "Profundidad (depth): " << img.depth() << "\n";
}

void printMeanStdDev(const cv::Mat& img, const std::string& label) {
    cv::Scalar mean, stddev;
    cv::meanStdDev(img, mean, stddev);
    cout << label << " -> Media: " << mean[0] << " | Desviacion: " << stddev[0] << "\n";
}

cv::Mat gaussianSegmentation(const cv::Mat& img, double k) {
    cv::Scalar mean, stddev;
    cv::meanStdDev(img, mean, stddev);

    const double mu = mean[0];
    const double sigma = stddev[0];
    const double low = mu - k * sigma;
    const double high = mu + k * sigma;

    cv::Mat mask = cv::Mat::zeros(img.size(), CV_8U);

    for (int i = 0; i < img.rows; ++i) {
        const uchar* row = img.ptr<uchar>(i);
        uchar* mrow = mask.ptr<uchar>(i);
        for (int j = 0; j < img.cols; ++j) {
            const uchar px = row[j];
            if (px > low && px < high) mrow[j] = 255;
        }
    }
    return mask;
}

cv::Mat applyGaussianBlur(const cv::Mat& img, int ksize, double sigma) {
    cv::Mat out;
    cv::GaussianBlur(img, out, cv::Size(ksize, ksize), sigma);
    return out;
}

cv::Mat laplacianHighpass(const cv::Mat& img, int ksize) {
    cv::Mat lap16s, lapAbs;
    cv::Laplacian(img, lap16s, CV_16S, ksize);
    cv::convertScaleAbs(lap16s, lapAbs);
    return lapAbs;
}

cv::Mat sharpen(const cv::Mat& base, const cv::Mat& lap) {
    cv::Mat out;
    cv::add(base, lap, out);
    return out;
}

cv::Mat gaussianPipeline(const cv::Mat& img) {
    cv::Mat gauss = applyGaussianBlur(img, 7, 1.5);
    cv::Mat lap = laplacianHighpass(gauss, 3);
    return sharpen(gauss, lap);
}

cv::Mat createGaussianKernel(int size, double sigma) {
    cv::Mat kernel(size, size, CV_64F);
    int c = size / 2;

    double sum = 0.0;
    const double twoSigma2 = 2.0 * sigma * sigma;
    const double norm = 1.0 / (2.0 * CV_PI * sigma * sigma);

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            int x = i - c;
            int y = j - c;
            double val = norm * std::exp(-(x * x + y * y) / twoSigma2);
            kernel.at<double>(i, j) = val;
            sum += val;
        }
    }

    kernel /= sum; // normaliza para que suma ~ 1
    return kernel;
}