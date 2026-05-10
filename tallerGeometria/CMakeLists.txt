cmake_minimum_required(VERSION 3.10)
project(TallerGeometria)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

include_directories(include ${OpenCV_INCLUDE_DIRS})

add_executable(geometria
    main.cpp
    src/puntos_vectores.cpp
    src/lineas.cpp
    src/poligonos.cpp
    src/circulos.cpp
    src/transformaciones.cpp
    src/procesamiento.cpp
)

target_link_libraries(geometria ${OpenCV_LIBS})
