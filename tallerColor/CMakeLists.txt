cmake_minimum_required(VERSION 3.10)
project(TallerOpenCV)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(ej1 ej1_rgb_hsv.cpp)
target_link_libraries(ej1 ${OpenCV_LIBS})

add_executable(ej2 ej2_saturacion.cpp)
target_link_libraries(ej2 ${OpenCV_LIBS})

add_executable(ej3 ej3_kmeans.cpp)
target_link_libraries(ej3 ${OpenCV_LIBS})

add_executable(ej4 ej4_grayworld.cpp)
target_link_libraries(ej4 ${OpenCV_LIBS})

add_executable(ej6 ej6_gamma.cpp)
target_link_libraries(ej6 ${OpenCV_LIBS})

add_executable(ej7 ej7_vignette.cpp)
target_link_libraries(ej7 ${OpenCV_LIBS})



message(STATUS "OpenCV version: ${OpenCV_VERSION}")
