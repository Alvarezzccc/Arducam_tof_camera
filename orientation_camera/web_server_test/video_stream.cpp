#include "globals.h"

int main() {
    startWifiAP();

    gst_init(nullptr, nullptr);
    const std::string address = "0.0.0.0";
    const int port = 80;

    captureAndStreamToF(address, port);

    return 0;
}
