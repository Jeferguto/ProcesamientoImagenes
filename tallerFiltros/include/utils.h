#ifndef UTILS_H
#define UTILS_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <utility>

std::string openFileDialog();

cv::Mat toGrayManual(const cv::Mat& input);
cv::Mat resizeForDisplay(const cv::Mat& img, int width = 320, int height = 240);
cv::Mat ensure3Channels(const cv::Mat& img);
cv::Mat createGrid(const std::vector<std::pair<std::string, cv::Mat>>& images, int cols = 2);

cv::Mat convolveFloat(const cv::Mat& input, const std::vector<std::vector<float>>& kernel);
cv::Mat gaussianBlurManual(const cv::Mat& gray, int ksize = 5, float sigma = 1.0f);
std::vector<std::vector<float>> createGaussianKernel(int ksize, float sigma);

cv::Mat normalizeFloatToU8(const cv::Mat& input);
cv::Mat absFloatToU8(const cv::Mat& input);

cv::Mat computeGradientMagnitude(const cv::Mat& gx, const cv::Mat& gy);
cv::Mat computeGradientAngle(const cv::Mat& gx, const cv::Mat& gy);

cv::Mat overlayEdgesOnImage(const cv::Mat& original, const cv::Mat& edges, const cv::Vec3b& color, uchar threshold = 50);

float clampFloat(float value, float minVal, float maxVal);

#endif