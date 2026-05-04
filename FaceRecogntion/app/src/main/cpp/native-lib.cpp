#include <jni.h>
#include <string>
#include <vector>
#include <android/log.h>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "MyPCA.h"
#include "FaceDetector.h"

using namespace cv;
using namespace std;

#define TAG "FaceRecognition"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

static MyPCA* pca = nullptr;
static FaceDetector* detector = nullptr;
static vector<Mat> trainProjections;
static vector<string> trainLabels;

extern "C" {

JNIEXPORT void JNICALL
Java_com_example_facerecognition_MainActivity_initDetector(
        JNIEnv* env, jobject, jstring cascadePath) {
    const char* path = env->GetStringUTFChars(cascadePath, nullptr);
    detector = new FaceDetector(string(path));
    env->ReleaseStringUTFChars(cascadePath, path);
    LOGI("Detector initialized");
}

JNIEXPORT void JNICALL
Java_com_example_facerecognition_MainActivity_trainPCA(
        JNIEnv* env, jobject,
        jlongArray matAddrs, jobjectArray labels) {
    int count = env->GetArrayLength(matAddrs);
    jlong* addrs = env->GetLongArrayElements(matAddrs, nullptr);

    vector<Mat> faces;
    trainLabels.clear();

    for (int i = 0; i < count; i++) {
        Mat* m = (Mat*)addrs[i];

        // Convertir RGBA a gris
        Mat gray;
        cvtColor(*m, gray, COLOR_RGBA2GRAY);

        // Detectar cara
        Mat faceOut;
        if (detector != nullptr && detector->detectFace(gray, faceOut)) {
            faces.push_back(faceOut.clone());
        } else {
            // Si no detecta cara, usar frame completo redimensionado
            Mat resized;
            resize(gray, resized, Size(100, 100));
            faces.push_back(resized.clone());
        }

        jstring label = (jstring)env->GetObjectArrayElement(labels, i);
        const char* l = env->GetStringUTFChars(label, nullptr);
        trainLabels.push_back(string(l));
        env->ReleaseStringUTFChars(label, l);
    }
    env->ReleaseLongArrayElements(matAddrs, addrs, 0);

    if (faces.empty()) {
        LOGI("No faces to train");
        return;
    }

    if (pca != nullptr) delete pca;
    pca = new MyPCA(faces);

    Mat avg = pca->getAverage();
    Mat eigenvecs = pca->getEigenvectors();
    trainProjections.clear();
    for (auto& face : faces) {
        Mat flat;
        face.convertTo(flat, CV_32FC1);
        flat = flat.reshape(1, 1);
        subtract(flat, avg.t(), flat);
        Mat proj = flat * eigenvecs.t();
        trainProjections.push_back(proj);
    }
    LOGI("PCA trained with %d faces", (int)faces.size());
}

JNIEXPORT jstring JNICALL
Java_com_example_facerecognition_MainActivity_recognizeFace(
        JNIEnv* env, jobject, jlong matAddr) {
    if (pca == nullptr || detector == nullptr)
        return env->NewStringUTF("No entrenado");

    Mat* frame = (Mat*)matAddr;
    Mat gray;
    cvtColor(*frame, gray, COLOR_RGBA2GRAY);

    Mat faceOut;
    if (!detector->detectFace(gray, faceOut))
        return env->NewStringUTF("Sin rostro");

    Mat flat;
    faceOut.convertTo(flat, CV_32FC1);
    flat = flat.reshape(1, 1);
    Mat avg = pca->getAverage();
    Mat eigenvecs = pca->getEigenvectors();
    subtract(flat, avg.t(), flat);
    Mat proj = flat * eigenvecs.t();

    double minDist = DBL_MAX;
    int minIdx = 0;
    for (int i = 0; i < (int)trainProjections.size(); i++) {
        double dist = norm(proj, trainProjections[i]);
        if (dist < minDist) {
            minDist = dist;
            minIdx = i;
        }
    }

    double threshold = 5000.0;
    if (minDist > threshold)
        return env->NewStringUTF("Desconocido");

    return env->NewStringUTF(trainLabels[minIdx].c_str());
}

} // extern "C"
