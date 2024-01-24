#include "globals.h"

cv::Mat matRotateClockWise180(cv::Mat src)
{
    if (src.empty())
    {
        std::cerr << "RorateMat src is empty!";
    }

    flip(src, src, 0);
    flip(src, src, 1);
    return src;
}

void p(std::string errorMsg){
    std::cerr << errorMsg << std::endl;
}

void getPreview(uint8_t *preview_ptr, float *phase_image_ptr, float *amplitude_image_ptr)
{
    auto len = 240 * 180;
    for (auto i = 0; i < len; i++)
    {
        uint8_t mask = *(amplitude_image_ptr + i) > 30 ? 254 : 0;
        float depth = ((1 - (*(phase_image_ptr + i) / MAX_DISTANCE)) * 255);
        uint8_t pixel = depth > 255 ? 255 : depth;
        *(preview_ptr + i) = pixel & mask;
    }
}

void processArducamTOFImage(ArducamTOFCamera &tof, CameraInfo &tofFormat, uint8_t *preview_ptr, httplib::Response &res) {
    // Here we process the Arducam ToF image and send it to the frame buffer
    ArducamFrameBuffer *frame;
    float *depth_ptr, *amplitude_ptr;

    frame = tof.requestFrame(200);
    if (frame != nullptr) {
        depth_ptr = (float *)frame->getData(FrameType::DEPTH_FRAME);
        amplitude_ptr = (float *)frame->getData(FrameType::AMPLITUDE_FRAME);
        getPreview(preview_ptr, depth_ptr, amplitude_ptr);

        cv::Mat result_frame(tofFormat.height, tofFormat.width, CV_8U, preview_ptr);
        result_frame = matRotateClockWise180(result_frame);
        cv::applyColorMap(result_frame, result_frame, cv::COLORMAP_JET);

        std::vector<uchar> buf;
        cv::imencode(".jpg", result_frame, buf);
        res.set_content(reinterpret_cast<const char *>(buf.data()), buf.size(), "image/jpeg");
    } else {
        std::cerr << "Error: Unable to get frame." << std::endl;
    }
    tof.releaseFrame(frame);
}


// Function to capture and stream webcam frames
void captureAndStreamToF(const std::string &address, int port) {

    p("Starting captureAndStreamToF");

    // Initialize the Arducam camera
    ArducamTOFCamera tof;
    if (tof.open(Connection::CSI))
    {
        std::cerr << "initialization failed" << std::endl;
        exit(-1);
    }

    p("Camera initialized");

    if (tof.start(FrameType::DEPTH_FRAME))
    {
        std::cerr << "Failed to start camera" << std::endl;
        exit(-1);
    }

    p("Camera started");   

    //  Modify the range also to modify the MAX_DISTANCE
    tof.setControl(CameraCtrl::RANGE, MAX_DISTANCE);
    CameraInfo tofFormat = tof.getCameraInfo();

    p("Camera info retrieved");

    // Image pointers
    float *depth_ptr;
    float *amplitude_ptr;
    uint8_t *preview_ptr = new uint8_t[tofFormat.height * tofFormat.width];

    p("Image pointers initialized");

    startServer(address, port, tof, tofFormat, preview_ptr);
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
