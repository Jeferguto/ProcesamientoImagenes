#include "utils.h"

#include <cstdio>
#include <array>
#include <iostream>
#include <cmath>
#include <limits>

std::string openFileDialog() {
    std::string command =
        "zenity --file-selection "
        "--title='Selecciona una imagen' "
        "--file-filter='Imagenes | *.png *.jpg *.jpeg *.bmp'";

    std::array<char, 512> buffer{};
    std::string result;

    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        std::cerr << "No se pudo abrir zenity.\n";
        return "";
    }

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }

    pclose(pipe);

    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }

    return result;
}

float clampFloat(float value, float minVal, float maxVal) {
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}

cv::Mat toGrayManual(const cv::Mat& input) {
    if (input.empty()) return cv::Mat();

    if (input.channels() == 1) {
        return input.clone();
    }

    cv::Mat gray(input.rows, input.cols, CV_8UC1);

    for (int y = 0; y < input.rows; ++y) {
        for (int x = 0; x < input.cols; ++x) {
            cv::Vec3b pixel = input.at<cv::Vec3b>(y, x);

            float b = pixel[0];
            float g = pixel[1];
            float r = pixel[2];

            float value = 0.114f * b + 0.587f * g + 0.299f * r;
            gray.at<uchar>(y, x) = static_cast<uchar>(clampFloat(std::round(value), 0.0f, 255.0f));
        }
    }

    return gray;
}

std::vector<std::vector<float>> createGaussianKernel(int ksize, float sigma) {
    std::vector<std::vector<float>> kernel(ksize, std::vector<float>(ksize, 0.0f));

    int half = ksize / 2;
    float sum = 0.0f;
    float sigma2 = sigma * sigma;
    float coeff = 1.0f / (2.0f * static_cast<float>(CV_PI) * sigma2);

    for (int y = -half; y <= half; ++y) {
        for (int x = -half; x <= half; ++x) {
            float exponent = -static_cast<float>(x * x + y * y) / (2.0f * sigma2);
            float value = coeff * std::exp(exponent);
            kernel[y + half][x + half] = value;
            sum += value;
        }
    }

    for (int y = 0; y < ksize; ++y) {
        for (int x = 0; x < ksize; ++x) {
            kernel[y][x] /= sum;
        }
    }

    return kernel;
}

cv::Mat convolveFloat(const cv::Mat& input, const std::vector<std::vector<float>>& kernel) {
    CV_Assert(input.type() == CV_8UC1 || input.type() == CV_32FC1);

    int rows = input.rows;
    int cols = input.cols;
    int kRows = static_cast<int>(kernel.size());
    int kCols = static_cast<int>(kernel[0].size());
    int padY = kRows / 2;
    int padX = kCols / 2;

    cv::Mat output(rows, cols, CV_32FC1, cv::Scalar(0));

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            float sum = 0.0f;

            for (int ky = 0; ky < kRows; ++ky) {
                for (int kx = 0; kx < kCols; ++kx) {
                    int iy = y + ky - padY;
                    int ix = x + kx - padX;

                    if (iy < 0) iy = 0;
                    if (iy >= rows) iy = rows - 1;
                    if (ix < 0) ix = 0;
                    if (ix >= cols) ix = cols - 1;

                    float pixel = 0.0f;
                    if (input.type() == CV_8UC1) {
                        pixel = static_cast<float>(input.at<uchar>(iy, ix));
                    } else {
                        pixel = input.at<float>(iy, ix);
                    }

                    sum += pixel * kernel[ky][kx];
                }
            }

            output.at<float>(y, x) = sum;
        }
    }

    return output;
}

cv::Mat gaussianBlurManual(const cv::Mat& gray, int ksize, float sigma) {
    CV_Assert(gray.type() == CV_8UC1);
    auto kernel = createGaussianKernel(ksize, sigma);
    return convolveFloat(gray, kernel);
}

cv::Mat normalizeFloatToU8(const cv::Mat& input) {
    CV_Assert(input.type() == CV_32FC1);

    double minVal, maxVal;
    cv::minMaxLoc(input, &minVal, &maxVal);

    cv::Mat output(input.rows, input.cols, CV_8UC1, cv::Scalar(0));

    if (std::abs(maxVal - minVal) < 1e-6) {
        return output;
    }

    for (int y = 0; y < input.rows; ++y) {
        for (int x = 0; x < input.cols; ++x) {
            float value = input.at<float>(y, x);
            float norm = 255.0f * (value - static_cast<float>(minVal)) /
                         (static_cast<float>(maxVal - minVal));
            output.at<uchar>(y, x) = static_cast<uchar>(clampFloat(std::round(norm), 0.0f, 255.0f));
        }
    }

    return output;
}

cv::Mat absFloatToU8(const cv::Mat& input) {
    CV_Assert(input.type() == CV_32FC1);

    float maxAbs = 0.0f;
    for (int y = 0; y < input.rows; ++y) {
        for (int x = 0; x < input.cols; ++x) {
            float v = std::abs(input.at<float>(y, x));
            if (v > maxAbs) maxAbs = v;
        }
    }

    cv::Mat output(input.rows, input.cols, CV_8UC1, cv::Scalar(0));
    if (maxAbs < 1e-6f) return output;

    for (int y = 0; y < input.rows; ++y) {
        for (int x = 0; x < input.cols; ++x) {
            float v = std::abs(input.at<float>(y, x));
            float scaled = 255.0f * v / maxAbs;
            output.at<uchar>(y, x) = static_cast<uchar>(clampFloat(std::round(scaled), 0.0f, 255.0f));
        }
    }

    return output;
}

cv::Mat computeGradientMagnitude(const cv::Mat& gx, const cv::Mat& gy) {
    CV_Assert(gx.type() == CV_32FC1 && gy.type() == CV_32FC1);
    CV_Assert(gx.size() == gy.size());

    cv::Mat mag(gx.rows, gx.cols, CV_32FC1, cv::Scalar(0));

    for (int y = 0; y < gx.rows; ++y) {
        for (int x = 0; x < gx.cols; ++x) {
            float dx = gx.at<float>(y, x);
            float dy = gy.at<float>(y, x);
            mag.at<float>(y, x) = std::sqrt(dx * dx + dy * dy);
        }
    }

    return mag;
}

cv::Mat computeGradientAngle(const cv::Mat& gx, const cv::Mat& gy) {
    CV_Assert(gx.type() == CV_32FC1 && gy.type() == CV_32FC1);
    CV_Assert(gx.size() == gy.size());

    cv::Mat angle(gx.rows, gx.cols, CV_32FC1, cv::Scalar(0));

    for (int y = 0; y < gx.rows; ++y) {
        for (int x = 0; x < gx.cols; ++x) {
            float dx = gx.at<float>(y, x);
            float dy = gy.at<float>(y, x);
            float theta = std::atan2(dy, dx) * 180.0f / static_cast<float>(CV_PI);
            if (theta < 0.0f) theta += 180.0f;
            angle.at<float>(y, x) = theta;
        }
    }

    return angle;
}

cv::Mat resizeForDisplay(const cv::Mat& img, int width, int height) {
    cv::Mat resized;
    cv::resize(img, resized, cv::Size(width, height));
    return resized;
}

cv::Mat ensure3Channels(const cv::Mat& img) {
    if (img.channels() == 3) {
        return img.clone();
    }

    cv::Mat color(img.rows, img.cols, CV_8UC3);
    for (int y = 0; y < img.rows; ++y) {
        for (int x = 0; x < img.cols; ++x) {
            uchar v = img.at<uchar>(y, x);
            color.at<cv::Vec3b>(y, x) = cv::Vec3b(v, v, v);
        }
    }
    return color;
}

cv::Mat overlayEdgesOnImage(const cv::Mat& original, const cv::Mat& edges, const cv::Vec3b& color, uchar threshold) {
    cv::Mat base;

    if (original.channels() == 1) {
        base = ensure3Channels(original);
    } else {
        base = original.clone();
    }

    CV_Assert(edges.type() == CV_8UC1);
    CV_Assert(base.rows == edges.rows && base.cols == edges.cols);

    for (int y = 0; y < edges.rows; ++y) {
        for (int x = 0; x < edges.cols; ++x) {
            if (edges.at<uchar>(y, x) > threshold) {
                base.at<cv::Vec3b>(y, x) = color;
            }
        }
    }

    return base;
}

cv::Mat createGrid(const std::vector<std::pair<std::string, cv::Mat>>& images, int cols) {
    if (images.empty()) return cv::Mat();

    const int tileW = 320;
    const int tileH = 240;
    const int rows = (static_cast<int>(images.size()) + cols - 1) / cols;

    std::vector<cv::Mat> tiles;

    for (const auto& item : images) {
        cv::Mat tile = resizeForDisplay(ensure3Channels(item.second), tileW, tileH);
        cv::putText(tile, item.first, cv::Point(10, 25),
                    cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
        tiles.push_back(tile);
    }

    while (static_cast<int>(tiles.size()) < rows * cols) {
        tiles.push_back(cv::Mat::zeros(tileH, tileW, CV_8UC3));
    }

    std::vector<cv::Mat> rowMats;
    for (int r = 0; r < rows; ++r) {
        std::vector<cv::Mat> currentRow;
        for (int c = 0; c < cols; ++c) {
            currentRow.push_back(tiles[r * cols + c]);
        }
        cv::Mat row;
        cv::hconcat(currentRow, row);
        rowMats.push_back(row);
    }

    cv::Mat grid;
    cv::vconcat(rowMats, grid);
    return grid;
}