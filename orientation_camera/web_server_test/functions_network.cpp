#include "globals.h"


/*
 * This function creates a new process in charge of the wifi access point
 * The script creates a new virtual interface and sets up the AP with hostapd and dnsmasq
*/
void startWifiAP() {
    pid_t pid = fork();

    if (pid == -1) {
        // Handle error in fork
        p("Error: Unable to start wifi access point");
    } else if (pid > 0) {
        // Parent process
        
    } else {
        // Child process
        std::string command = "sudo sh ../start_wifi.sh";
        execl("/bin/sh", "sh", "-c", command.c_str(), (char *) NULL);

        // execl only returns on error
        exit(EXIT_FAILURE);
    }
}


/* 
*  This function serves the local IP address of the device. It is used to serve it to the cleint
*  so that the client can ask for the video stream. 
*
*  It first checkks the IP address of the vapInterf and then the wlan0 interface, since the camera 
*  is meant to work with the vapInterf virtual interface.
*/
std::string get_ip_address() {
    struct ifaddrs *addrs, *tmp;

    if (getifaddrs(&addrs) != 0) {
        // Handle error
        return "Error";
    }

    tmp = addrs;
    std::string vapInterf_ip = "";
    std::string wlan0_ip = "";

    while (tmp) {
        if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *pAddr = (struct sockaddr_in *)tmp->ifa_addr;
            char buffer[INET_ADDRSTRLEN];
            const char *ip = inet_ntop(AF_INET, &(pAddr->sin_addr), buffer, INET_ADDRSTRLEN);

            if (strcmp(tmp->ifa_name, "vapInterf") == 0 && ip != nullptr) {
                vapInterf_ip = std::string(ip);
            } else if (strcmp(tmp->ifa_name, "wlan0") == 0 && ip != nullptr) {
                wlan0_ip = std::string(ip);
            }
        }
        tmp = tmp->ifa_next;
    }

    freeifaddrs(addrs);

    if (!vapInterf_ip.empty()) {
        return vapInterf_ip;
    } else if (!wlan0_ip.empty()) {
        return wlan0_ip;
    }

    return "Error";
}