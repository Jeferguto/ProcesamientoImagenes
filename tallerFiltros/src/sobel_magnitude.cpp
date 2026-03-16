#include "filters.h"
#include "utils.h"

cv::Mat applySobelMagnitude(const cv::Mat& input) {
    cv::Mat gray = toGrayManual(input);

    std::vector<std::vector<float>> sobelX = {
        {-1.0f, 0.0f, 1.0f},
        {-2.0f, 0.0f, 2.0f},
        {-1.0f, 0.0f, 1.0f}
    };

    std::vector<std::vector<float>> sobelY = {
        {-1.0f, -2.0f, -1.0f},
        {0.0f, 0.0f, 0.0f},
        {1.0f, 2.0f, 1.0f}
    };

    cv::Mat gx = convolveFloat(gray, sobelX);
    cv::Mat gy = convolveFloat(gray, sobelY);
    cv::Mat mag = computeGradientMagnitude(gx, gy);

    return normalizeFloatToU8(mag);
}