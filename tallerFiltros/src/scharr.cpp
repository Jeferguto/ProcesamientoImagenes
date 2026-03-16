#include "filters.h"
#include "utils.h"

cv::Mat applyScharrEdges(const cv::Mat& input) {
    cv::Mat gray = toGrayManual(input);

    std::vector<std::vector<float>> scharrX = {
        {-3.0f, 0.0f, 3.0f},
        {-10.0f, 0.0f, 10.0f},
        {-3.0f, 0.0f, 3.0f}
    };

    std::vector<std::vector<float>> scharrY = {
        {-3.0f, -10.0f, -3.0f},
        {0.0f, 0.0f, 0.0f},
        {3.0f, 10.0f, 3.0f}
    };

    cv::Mat gx = convolveFloat(gray, scharrX);
    cv::Mat gy = convolveFloat(gray, scharrY);
    cv::Mat mag = computeGradientMagnitude(gx, gy);

    return normalizeFloatToU8(mag);
}