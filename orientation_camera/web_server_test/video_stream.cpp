#include "globals.h"

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <chrono>
#include <thread>
#include <fstream>
#include <streambuf>


// Function to read the content of a file into a string
std::string read_file(const std::string &filename) {
    std::ifstream t(filename);
    return std::string((std::istreambuf_iterator<char>(t)),
                       std::istreambuf_iterator<char>());
}

// Function to capture and stream webcam frames
void captureAndStream(const std::string &address, int port) {
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error opening webcam" << std::endl;
        return;
    }

    httplib::Server svr; // Create an instance of the httplib server

    // Serve the HTML file on the root URL
    svr.Get("/", [&](const httplib::Request & /*req*/, httplib::Response &res) {
        std::string html_content = read_file("../index.html"); // Adjust the path accordingly
        res.set_content(html_content, "text/html");
    });

    // Stream webcam frames on the /stream URL
    svr.Get("/stream", [&](const httplib::Request & /*req*/, httplib::Response &res) {
        cv::Mat frame;
        cap >> frame;

        std::vector<uchar> buf;
        cv::imencode(".jpg", frame, buf);

        res.set_content(reinterpret_cast<const char *>(buf.data()), buf.size(), "image/jpeg");
    });

    std::cout << "Server started on port " << port << std::endl;

    svr.listen(address.c_str(), port); // Start the server
}

int main() {
    gst_init(nullptr, nullptr);
    const std::string address = "0.0.0.0";
    const int port = 80;

    captureAndStream(address, port);

    return 0;
}



/*int main(){
    
    // Start a wifi AP
    // startWifi(); // TODO

    // Start a web server
    startWebServer(); // TODO

    printf("Failed, closing\n");

    return 0;
}*/