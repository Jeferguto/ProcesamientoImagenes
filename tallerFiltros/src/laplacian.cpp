#include "filters.h"
#include "utils.h"

cv::Mat applyLaplacianEdges(const cv::Mat& input) {
    cv::Mat gray = toGrayManual(input);

    std::vector<std::vector<float>> lapKernel = {
        {0.0f, -1.0f, 0.0f},
        {-1.0f, 4.0f, -1.0f},
        {0.0f, -1.0f, 0.0f}
    };

    cv::Mat lap = convolveFloat(gray, lapKernel);
    return absFloatToU8(lap);
}