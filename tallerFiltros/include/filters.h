#ifndef FILTERS_H
#define FILTERS_H

#include <opencv2/opencv.hpp>

cv::Mat applyGray(const cv::Mat& input);
cv::Mat applyLoG(const cv::Mat& input);
cv::Mat applyZeroCrossing(const cv::Mat& input);
cv::Mat applyScharrEdges(const cv::Mat& input);
cv::Mat applyLaplacianEdges(const cv::Mat& input);
cv::Mat applySobelMagnitude(const cv::Mat& input);
cv::Mat applyCannyEdges(const cv::Mat& input);

#endif