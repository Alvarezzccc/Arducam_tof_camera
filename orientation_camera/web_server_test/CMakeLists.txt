cmake_minimum_required(VERSION 3.4)
project(orientation_camera)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pg")

find_package(OpenCV REQUIRED)
find_package(Boost COMPONENTS system thread REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(httplib REQUIRED)
find_package(OpenSSL REQUIRED)

# Find GStreamer and GStreamer App libraries
find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
pkg_check_modules(GSTAPP REQUIRED gstreamer-app-1.0)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MHD REQUIRED libmicrohttpd)


include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${GSTREAMER_INCLUDE_DIRS}
    ${GSTAPP_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${MHD_INCLUDE_DIRS}
    /usr/include/cpp    
)

add_executable(video_stream video_stream.cpp functions_web.cpp functions_network.cpp functions_camera.cpp)


target_link_libraries(video_stream
    ${OpenCV_LIBS}
    ${Boost_LIBRARIES}
    ${GSTREAMER_LIBRARIES}
    ${GSTAPP_LIBRARIES}
    ${MHD_LIBRARIES}
    ArducamDepthCamera
)