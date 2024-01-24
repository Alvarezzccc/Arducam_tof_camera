#include "globals.h"

int main() {

    // Start a wifi AP to work without any external ISP (or external subnetwork support)
    //startWifi(); // TODO

    gst_init(nullptr, nullptr);
    const std::string address = "0.0.0.0";
    const int port = 80;

    bool useToF = false;

    if (!useToF)
        captureAndStream(address, port);
    else
        captureAndStreamToF(address, port);

    return 0;
}
