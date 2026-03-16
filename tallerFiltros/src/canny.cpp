#include "filters.h"
#include "utils.h"

#include <cmath>
#include <queue>

static cv::Mat nonMaximumSuppression(const cv::Mat& magnitude, const cv::Mat& angle) {
    cv::Mat nms(magnitude.rows, magnitude.cols, CV_32FC1, cv::Scalar(0));

    for (int y = 1; y < magnitude.rows - 1; ++y) {
        for (int x = 1; x < magnitude.cols - 1; ++x) {
            float dir = angle.at<float>(y, x);
            float mag = magnitude.at<float>(y, x);

            float q = 0.0f;
            float r = 0.0f;

            if ((dir >= 0.0f && dir < 22.5f) || (dir >= 157.5f && dir <= 180.0f)) {
                q = magnitude.at<float>(y, x + 1);
                r = magnitude.at<float>(y, x - 1);
            } else if (dir >= 22.5f && dir < 67.5f) {
                q = magnitude.at<float>(y + 1, x - 1);
                r = magnitude.at<float>(y - 1, x + 1);
            } else if (dir >= 67.5f && dir < 112.5f) {
                q = magnitude.at<float>(y + 1, x);
                r = magnitude.at<float>(y - 1, x);
            } else {
                q = magnitude.at<float>(y - 1, x - 1);
                r = magnitude.at<float>(y + 1, x + 1);
            }

            if (mag >= q && mag >= r) {
                nms.at<float>(y, x) = mag;
            } else {
                nms.at<float>(y, x) = 0.0f;
            }
        }
    }

    return nms;
}

static cv::Mat doubleThreshold(const cv::Mat& nms, float lowRatio, float highRatio) {
    double minVal, maxVal;
    cv::minMaxLoc(nms, &minVal, &maxVal);

    float highThreshold = static_cast<float>(maxVal) * highRatio;
    float lowThreshold = highThreshold * lowRatio;

    const uchar STRONG = 255;
    const uchar WEAK = 75;

    cv::Mat result(nms.rows, nms.cols, CV_8UC1, cv::Scalar(0));

    for (int y = 0; y < nms.rows; ++y) {
        for (int x = 0; x < nms.cols; ++x) {
            float value = nms.at<float>(y, x);

            if (value >= highThreshold) {
                result.at<uchar>(y, x) = STRONG;
            } else if (value >= lowThreshold) {
                result.at<uchar>(y, x) = WEAK;
            } else {
                result.at<uchar>(y, x) = 0;
            }
        }
    }

    return result;
}

static cv::Mat hysteresis(const cv::Mat& thresholded) {
    const uchar STRONG = 255;
    const uchar WEAK = 75;

    cv::Mat result = thresholded.clone();
    std::queue<cv::Point> q;

    for (int y = 0; y < result.rows; ++y) {
        for (int x = 0; x < result.cols; ++x) {
            if (result.at<uchar>(y, x) == STRONG) {
                q.push(cv::Point(x, y));
            }
        }
    }

    while (!q.empty()) {
        cv::Point p = q.front();
        q.pop();

        for (int j = -1; j <= 1; ++j) {
            for (int i = -1; i <= 1; ++i) {
                if (i == 0 && j == 0) continue;

                int nx = p.x + i;
                int ny = p.y + j;

                if (nx < 0 || ny < 0 || nx >= result.cols || ny >= result.rows) continue;

                if (result.at<uchar>(ny, nx) == WEAK) {
                    result.at<uchar>(ny, nx) = STRONG;
                    q.push(cv::Point(nx, ny));
                }
            }
        }
    }

    for (int y = 0; y < result.rows; ++y) {
        for (int x = 0; x < result.cols; ++x) {
            if (result.at<uchar>(y, x) != STRONG) {
                result.at<uchar>(y, x) = 0;
            }
        }
    }

    return result;
}

cv::Mat applyCannyEdges(const cv::Mat& input) {
    cv::Mat gray = toGrayManual(input);
    cv::Mat blurred = gaussianBlurManual(gray, 5, 1.0f);

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

    cv::Mat gx = convolveFloat(blurred, sobelX);
    cv::Mat gy = convolveFloat(blurred, sobelY);

    cv::Mat magnitude = computeGradientMagnitude(gx, gy);
    cv::Mat angle = computeGradientAngle(gx, gy);

    cv::Mat nms = nonMaximumSuppression(magnitude, angle);
    cv::Mat thresholded = doubleThreshold(nms, 0.5f, 0.2f);
    cv::Mat edges = hysteresis(thresholded);

    return edges;
}