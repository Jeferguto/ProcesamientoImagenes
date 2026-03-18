cmake_minimum_required(VERSION 3.10)
project(ConvexHullApp)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

include_directories(include)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(convexhull_app
    main.cpp
    src/converter.cpp
)

target_link_libraries(convexhull_app ${OpenCV_LIBS})
