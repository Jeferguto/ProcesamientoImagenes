cmake_minimum_required(VERSION 3.10)
project(tallerConsolidado)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

add_executable(tallerConsolidado
    main.cpp
    src/gaussian_workshop.cpp
)

target_include_directories(tallerConsolidado PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(tallerConsolidado PRIVATE ${OpenCV_LIBS})

message(STATUS "OpenCV version: ${OpenCV_VERSION}")