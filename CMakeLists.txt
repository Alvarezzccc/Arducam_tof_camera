cmake_minimum_required(VERSION 3.4)

SET(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -std=c++17 -g2 -ggdb -Wno-psabi")  

project(TOFCameraExample)

add_subdirectory(example)
add_subdirectory(pcl_preview)
add_subdirectory(jetson)
add_subdirectory(orientation_camera)
