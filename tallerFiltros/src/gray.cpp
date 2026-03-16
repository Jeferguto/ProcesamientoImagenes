#include "filters.h"
#include "utils.h"

cv::Mat applyGray(const cv::Mat& input) {
    return toGrayManual(input);
}