
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


std::string get_ip_address() {
    struct ifaddrs *addrs, *tmp;
    
    if (getifaddrs(&addrs) != 0) {
        // Handle error
        return "Error";
    }
    
    tmp = addrs;
    while (tmp) {
        if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *pAddr = (struct sockaddr_in *)tmp->ifa_addr;
            
            // Check if it's not the loopback interface
            if (strcmp(tmp->ifa_name, "lo") != 0) {
                char buffer[INET_ADDRSTRLEN];
                const char *ip = inet_ntop(AF_INET, &(pAddr->sin_addr), buffer, INET_ADDRSTRLEN);
                
                freeifaddrs(addrs);
                
                if (ip != nullptr) {
                    return std::string(ip);
                }
            }
        }
        tmp = tmp->ifa_next;
    }
    
    freeifaddrs(addrs);
    return "Error";
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
        processArducamTOFImage(tof, tofFormat, preview_ptr, res);
    });

    std::cout << "Server started on port " << port << std::endl;
    svr.listen(address.c_str(), port); // Start the server
}
