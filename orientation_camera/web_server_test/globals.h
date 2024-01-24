#include <cstdio>
#include <ostream>
#include <dirent.h>
#include <vector>
#include <random>
#include <microhttpd.h>
#include <gst/gst.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <chrono>
#include <thread>
#include <fstream>
#include <streambuf>
#include "ArducamTOFCamera.hpp"
#include <opencv2/core.hpp>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <iomanip>
#include <sstream>
#include <httplib.h>

// MAX_DISTANCE value modifiable  is 2 or 4
#define MAX_DISTANCE 4
#define HTTP_PORT 80

using namespace Arducam;
using namespace std;

std::string get_ip_address();
std::string read_file(const std::string &filename);
void captureAndStream(const std::string &address, int port);
void captureAndStreamToF(const std::string &address, int port);
cv::Mat matRotateClockWise180(cv::Mat src);
void getPreview(uint8_t *preview_ptr, float *phase_image_ptr, float *amplitude_image_ptr);
void p(std::string errorMsg);

void processArducamTOFImage(ArducamTOFCamera &tof, CameraInfo &tofFormat, uint8_t *preview_ptr, httplib::Response &res);
void startServer(const std::string& address, int port, ArducamTOFCamera &tof, CameraInfo &tofFormat, uint8_t *preview_ptr);
