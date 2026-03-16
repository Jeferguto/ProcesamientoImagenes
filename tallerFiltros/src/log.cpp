#include "filters.h"
#include "utils.h"

cv::Mat applyLoG(const cv::Mat& input) {
    cv::Mat gray = toGrayManual(input);
    cv::Mat blurred = gaussianBlurManual(gray, 5, 1.0f);

    std::vector<std::vector<float>> lapKernel = {
        {0.0f, -1.0f, 0.0f},
        {-1.0f, 4.0f, -1.0f},
        {0.0f, -1.0f, 0.0f}
    };

    cv::Mat logResponse = convolveFloat(blurred, lapKernel);
    return absFloatToU8(logResponse);
}