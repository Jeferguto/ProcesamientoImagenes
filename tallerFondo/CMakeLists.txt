cmake_minimum_required(VERSION 3.15)
project(TallerSegmentacion CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS} include)

add_executable(parte_a
    src/main_frame_diff.cpp
    src/frame_diff.cpp
)

target_link_libraries(parte_a ${OpenCV_LIBS})

add_executable(parte_b
    src/main_gmm.cpp
    src/gmm_segmenter.cpp
)

target_link_libraries(parte_b ${OpenCV_LIBS})