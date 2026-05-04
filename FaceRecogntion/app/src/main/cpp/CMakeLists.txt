cmake_minimum_required(VERSION 3.22.1)
project("facerecognition")

set(OpenCV_DIR /Users/jeffersongutierreztovar/Desktop/OpenCV-android-sdk/sdk/native/jni)
find_package(OpenCV REQUIRED)

add_library(${CMAKE_PROJECT_NAME} SHARED
        native-lib.cpp
        MyPCA.cpp
        FaceDetector.cpp)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        ${OpenCV_INCLUDE_DIRS})

target_link_libraries(${CMAKE_PROJECT_NAME}
        ${OpenCV_LIBS}
        android
        log)