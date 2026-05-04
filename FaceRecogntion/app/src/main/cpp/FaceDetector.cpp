#include "FaceDetector.h"
#include <android/log.h>

FaceDetector::FaceDetector(const string& cascadePath) {
    if (!face_cascade.load(cascadePath)) {
        __android_log_print(ANDROID_LOG_ERROR, "FaceDetector", "Error loading cascade: %s", cascadePath.c_str());
    }
}

bool FaceDetector::detectFace(Mat& frameGray, Mat& faceOut) {
    vector<Rect> faces;
    Mat equalized;
    equalizeHist(frameGray, equalized);
    face_cascade.detectMultiScale(equalized, faces, 1.1, 5, 0|CASCADE_SCALE_IMAGE, Size(30,30));

    if (!faces.empty()) {
        Mat faceROI = frameGray(faces[0]);
        resize(faceROI, faceOut, Size(100, 100));
        return true;
    }
    return false;
}

FaceDetector::~FaceDetector() {}
