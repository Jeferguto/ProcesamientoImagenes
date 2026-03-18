#ifndef CONVERTER_H
#define CONVERTER_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

struct Point2D {
    double x, y;
};

std::vector<Point2D> grahamScanHull(std::vector<Point2D>& points);
cv::Mat preprocessImage(const cv::Mat& src);
void drawHull(cv::Mat& frame, const std::vector<Point2D>& hull, const cv::Scalar& color);
std::string classifyShape(const std::vector<Point2D>& hull, const std::vector<cv::Point>& contour);
std::string openFileDialog();
void processFrame(cv::Mat& frame);

#endif
