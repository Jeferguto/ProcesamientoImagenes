#include "MyPCA.h"

MyPCA::MyPCA(vector<Mat>& _faces) {
    init(_faces);
}

void MyPCA::init(vector<Mat>& _faces) {
    imgSize = _faces[0].rows * _faces[0].cols;
    imgRows = _faces[0].rows;
    mergeMatrix(_faces);
    getAverageVector();
    subtractMatrix();
    Mat _covarMatrix = (subFacesMatrix.t()) * subFacesMatrix;
    getBestEigenVectors(_covarMatrix);
}

void MyPCA::mergeMatrix(vector<Mat>& _faces) {
    int col = (int)_faces.size();
    allFacesMatrix.create(imgSize, col, CV_32FC1);
    for (int i = 0; i < col; i++) {
        Mat tmpMatrix = allFacesMatrix.col(i);
        Mat tmpImg;
        _faces[i].convertTo(tmpImg, CV_32FC1);
        tmpImg.reshape(1, imgSize).copyTo(tmpMatrix);
    }
}

void MyPCA::getAverageVector() {
    reduce(allFacesMatrix, avgVector, 1, REDUCE_AVG);
}

void MyPCA::subtractMatrix() {
    allFacesMatrix.copyTo(subFacesMatrix);
    for (int i = 0; i < subFacesMatrix.cols; i++) {
        subtract(subFacesMatrix.col(i), avgVector, subFacesMatrix.col(i));
    }
}

void MyPCA::getBestEigenVectors(Mat _covarMatrix) {
    Mat allEigenValues, allEigenVectors;
    eigen(_covarMatrix, allEigenValues, allEigenVectors);
    eigenVector = allEigenVectors * (subFacesMatrix.t());
    for (int i = 0; i < eigenVector.rows; i++) {
        Mat tempVec = eigenVector.row(i);
        normalize(tempVec, tempVec);
    }
}

Mat MyPCA::getAverage() { return avgVector; }
Mat MyPCA::getEigenvectors() { return eigenVector; }
int MyPCA::getImgSize() { return imgSize; }
MyPCA::~MyPCA() {}