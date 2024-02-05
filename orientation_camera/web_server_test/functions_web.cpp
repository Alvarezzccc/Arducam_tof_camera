
#include "globals.h"

// Function to read the content of a file into a string and add the IP address
std::string read_file(const std::string &filename) {
    std::ifstream t(filename);
    std::string htmlContent((std::istreambuf_iterator<char>(t)),
                           std::istreambuf_iterator<char>());

    const std::string ipAddress = get_ip_address();
    const std::string replaced_string = "const serverIpAddress ='';";

    // Replace the placeholder with the IP address
    printf("IP address: %s\n", ipAddress.c_str());
    
    size_t pos = htmlContent.find(replaced_string);
    if (pos != std::string::npos) {
        std::string replacement = "const serverIpAddress = '" + ipAddress + "';";
        htmlContent.replace(pos, std::string(replaced_string).length(), replacement);
    }else
    {
        std::cerr << "Error: Unable to find placeholder in HTML file." << std::endl;
    }

    return htmlContent;
}

void startServer(const std::string& address, int port, ArducamTOFCamera &tof, CameraInfo &tofFormat, uint8_t *preview_ptr) {
    httplib::Server svr;
    
    // Serve the HTML file on the root URL
    svr.Get("/", [&](const httplib::Request & /*req*/, httplib::Response &res) {
        p("Serving HTML file");
        std::string html_content = read_file("../index.html"); 
        res.set_content(html_content, "text/html");
    });

    // Serve webcam frames on the /stream URL
    svr.Get("/stream", [&](const httplib::Request & /*req*/, httplib::Response &res) {
        processArducamToFFrame(tof, tofFormat, preview_ptr, res);
    });

    std::cout << "Server started on port " << port << std::endl;
    svr.listen(address.c_str(), port); // Start the server
}

bool checkGUIAvailable() {
    // Function to check if a GUI is available
    // This is a simplistic check; you might need a more robust check depending on your environment
    return getenv("DISPLAY") != nullptr;
}

// Function to send frame to the browser
void sendFrameToBrowser(const cv::Mat& frame, httplib::Response& res) {
    std::vector<uchar> buf;
    cv::imencode(".jpg", frame, buf);
    res.set_content(reinterpret_cast<const char *>(buf.data()), buf.size(), "image/jpeg");
}