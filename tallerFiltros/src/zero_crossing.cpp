#include "filters.h"
#include "utils.h"

#include <cmath>

cv::Mat applyZeroCrossing(const cv::Mat& input) {
    cv::Mat gray = toGrayManual(input);
    cv::Mat blurred = gaussianBlurManual(gray, 5, 1.0f);

    std::vector<std::vector<float>> lapKernel = {
        {0.0f, -1.0f, 0.0f},
        {-1.0f, 4.0f, -1.0f},
        {0.0f, -1.0f, 0.0f}
    };

    cv::Mat lap = convolveFloat(blurred, lapKernel);
    cv::Mat output(lap.rows, lap.cols, CV_8UC1, cv::Scalar(0));

    const float threshold = 5.0f;

    for (int y = 1; y < lap.rows - 1; ++y) {
        for (int x = 1; x < lap.cols - 1; ++x) {
            float center = lap.at<float>(y, x);
            bool edge = false;

            for (int j = -1; j <= 1 && !edge; ++j) {
                for (int i = -1; i <= 1; ++i) {
                    if (i == 0 && j == 0) continue;

                    float neighbor = lap.at<float>(y + j, x + i);

                    bool signChange =
                        (center > 0.0f && neighbor < 0.0f) ||
                        (center < 0.0f && neighbor > 0.0f);

                    if (signChange && std::abs(center - neighbor) > threshold) {
                        edge = true;
                        break;
                    }
                }
            }

            output.at<uchar>(y, x) = edge ? 255 : 0;
        }
    }

    return output;
}