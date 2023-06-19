cmake_minimum_required(VERSION 3.4)
include_directories(/usr/include/cpp)

project(orientation_camera)
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

project(captureFrames)
add_executable(capture_frames_for_test capture_frames_for_test.cpp )
target_link_libraries( capture_frames_for_test ArducamDepthCamera pigpio ${OpenCV_LIBS} )

