cmake_minimum_required(VERSION 3.4)
project(orientation_camera)

find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)

# Include directories for GStreamer and libmicrohttpd
include_directories(${OpenCV_INCLUDE_DIRS})
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
pkg_check_modules(LIBMHD REQUIRED libmicrohttpd)

# Project configuration
add_executable(video_stream video_stream.c)

# Link libraries
target_link_libraries(video_stream
    ${OpenCV_LIBS}
    ${GSTREAMER_LIBRARIES}
    ${LIBMHD_LIBRARIES}
)

# Include directories
target_include_directories(video_stream PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${GSTREAMER_INCLUDE_DIRS}
    ${LIBMHD_INCLUDE_DIRS}
)
