#ifndef FACE_DETECTOR_H
#define FACE_DETECTOR_H
#include <string>
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core.hpp"
using namespace std;
using namespace cv;

class FaceDetector {
public:
    FaceDetector(const string& cascadePath);
    ~FaceDetector();
    bool detectFace(Mat& frameGray, Mat& faceOut);
private:
    CascadeClassifier face_cascade;
};
#endif