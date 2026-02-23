#include "frame_diff.h"

FrameDifferencer::FrameDifferencer(int threshold_val, int min_contour_area,
                                   double learning_rate)
    : threshold_val_(threshold_val),
      min_area_(min_contour_area),
      learning_rate_(learning_rate) {

    kernel_ = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5,5));
}

void FrameDifferencer::setBackground(const cv::Mat& frame) {
    cv::Mat gray;

    if (frame.channels() == 3)
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    else
        gray = frame.clone();

    background_ = gray.clone();
}

cv::Mat FrameDifferencer::process(const cv::Mat& frame) {
    cv::Mat gray;

    if (frame.channels() == 3)
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    else
        gray = frame.clone();

    cv::Mat diff;
    cv::absdiff(background_, gray, diff);

    cv::Mat mask;
    cv::threshold(diff, mask, threshold_val_, 255, cv::THRESH_BINARY);

    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel_);
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel_);

    return mask;
}

std::vector<cv::Rect> FrameDifferencer::getRegions(const cv::Mat& mask) {
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;

    cv::Mat temp = mask.clone();
    cv::findContours(temp, contours, hierarchy,
                     cv::RETR_EXTERNAL,
                     cv::CHAIN_APPROX_SIMPLE);

    std::vector<cv::Rect> boxes;

    for (const auto& c : contours) {
        if (cv::contourArea(c) >= min_area_) {
            boxes.push_back(cv::boundingRect(c));
        }
    }

    return boxes;
}

void FrameDifferencer::updateBackground(const cv::Mat& frame) {
    if (learning_rate_ <= 0.0) return;

    cv::Mat gray;

    if (frame.channels() == 3)
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    else
        gray = frame.clone();

    cv::addWeighted(background_, 1.0 - learning_rate_,
                    gray, learning_rate_,
                    0.0, background_);
}