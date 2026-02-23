#include "gmm_segmenter.h"

GMMSegmenter::GMMSegmenter(const Config& cfg) : cfg_(cfg) {

    pMOG2_ = cv::createBackgroundSubtractorMOG2(
        cfg_.history,
        cfg_.var_threshold,
        cfg_.detect_shadows
    );

    kernel_open_ = cv::getStructuringElement(
        cv::MORPH_ELLIPSE,
        cv::Size(cfg_.morph_open_k, cfg_.morph_open_k)
    );

    kernel_close_ = cv::getStructuringElement(
        cv::MORPH_ELLIPSE,
        cv::Size(cfg_.morph_close_k, cfg_.morph_close_k)
    );
}

cv::Mat GMMSegmenter::apply(const cv::Mat& frame) {

    cv::Mat fg_mask;
    pMOG2_->apply(frame, fg_mask, -1);

    // eliminar sombras (127)
    cv::Mat clean_mask;
    cv::threshold(fg_mask, clean_mask, 200, 255, cv::THRESH_BINARY);

    cv::morphologyEx(clean_mask, clean_mask, cv::MORPH_OPEN, kernel_open_);
    cv::morphologyEx(clean_mask, clean_mask, cv::MORPH_CLOSE, kernel_close_);

    return clean_mask;
}

std::vector<cv::Rect> GMMSegmenter::getRegions(const cv::Mat& mask) const {

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;

    cv::Mat temp = mask.clone();

    cv::findContours(temp,
                     contours,
                     hierarchy,
                     cv::RETR_EXTERNAL,
                     cv::CHAIN_APPROX_SIMPLE);

    std::vector<cv::Rect> boxes;

    for (const auto& c : contours) {
        if (cv::contourArea(c) >= cfg_.min_area) {
            boxes.push_back(cv::boundingRect(c));
        }
    }

    return boxes;
}

cv::Mat GMMSegmenter::getBackground() const {

    cv::Mat bg;
    pMOG2_->getBackgroundImage(bg);
    return bg;
}