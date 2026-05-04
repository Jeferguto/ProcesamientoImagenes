#ifndef MY_PCA_H
#define MY_PCA_H
#include <vector>
#include <string>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
using namespace std;
using namespace cv;

class MyPCA {
public:
    MyPCA(vector<Mat>& _faces);
    void init(vector<Mat>& _faces);
    void mergeMatrix(vector<Mat>& _faces);
    void getAverageVector();
    void subtractMatrix();
    void getBestEigenVectors(Mat _covarMatrix);
    Mat getAverage();
    Mat getEigenvectors();
    int getImgSize();
    ~MyPCA();
private:
    int imgSize = -1;
    int imgRows = -1;
    Mat allFacesMatrix;
    Mat avgVector;
    Mat subFacesMatrix;
    Mat eigenVector;
};
#endif