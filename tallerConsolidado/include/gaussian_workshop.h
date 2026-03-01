#pragma once
#include <opencv2/opencv.hpp>
#include <string>

// Selector de archivo con Zenity (Linux). Devuelve "" si cancelas o falla.
std::string pickImageWithZenity();

// Info y estadísticas
void printImageInfo(const cv::Mat& img);
void printMeanStdDev(const cv::Mat& img, const std::string& label);

// Segmentación Gaussiana: máscara 255 si pixel en [mu - k*sigma, mu + k*sigma]
cv::Mat gaussianSegmentation(const cv::Mat& img, double k);

// Filtros
cv::Mat applyGaussianBlur(const cv::Mat& img, int ksize, double sigma);
cv::Mat laplacianHighpass(const cv::Mat& img, int ksize);
cv::Mat sharpen(const cv::Mat& base, const cv::Mat& lap);
cv::Mat gaussianPipeline(const cv::Mat& img);

// Kernel Gaussiano 2D manual (CV_64F) normalizado (suma ~ 1)
cv::Mat createGaussianKernel(int size, double sigma);