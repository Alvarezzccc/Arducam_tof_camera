// Including standard libraries for input/output, standard types, string operations, etc.
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>
#include <math.h>

// Define constants for the port, waveform characteristics, and buffer size
#define PORT (20001)
#define WAVS (10)          // Duration in tenths of seconds for sending data
#define UDP_FRAME (1600)   // Size of each UDP packet, representing two frames of 800 data points each
#define SERVERADDRESS "127.0.0.1"
#define BUFFER_SIZE (WAVS * UDP_FRAME) // Total buffer size
#define SR 8000 // Sampling rate

// Declare a buffer to hold the data to be sent
char buffer[BUFFER_SIZE];

/**
 * Function to populate the buffer with audio data.
 * @param buffer Pointer to the buffer
 * @param length Length of the buffer
 */
void build(uint8_t* buffer, size_t length)
{
    // Loop through the buffer and fill it with sine wave data
    for (size_t i = 0; i < length; i++)
    {
        if(i%2==0)
            buffer[i] = 127*sin((double)(200*M_PI*i)/SR)+127; // Right channel data
        else buffer[i] = 127*sin((double)(20*M_PI*i)/SR)+127; // Left channel data
    }
}

int main(int argc, char **argv)
{
    struct timespec start, end; // Variables to measure time
    int sockfd; // Socket file descriptor
    struct sockaddr_in server; // Server address structure

    printf("Build Data...\n");
    build(buffer, sizeof(buffer)); // Populate the buffer with audio data

    printf("Configure socket...\n");
    sockfd = socket(AF_INET, SOCK_DGRAM, 0); // Create a UDP socket
    if (sockfd < 0)
    {
        fprintf(stderr, "Error opening socket");
        return EXIT_FAILURE;
    }

    // Initialize server address structure
    bzero((char*)&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVERADDRESS); // Set server IP address
    server.sin_port = htons(PORT); // Set server port number

    printf("Send UDP data...\n");
    clock_gettime(CLOCK_MONOTONIC_RAW, &start); // Start timing

    // Send the data in chunks equal to the size of UDP_FRAME
    for (size_t i = 0; i < BUFFER_SIZE; i += UDP_FRAME)
    {
        if (sendto(sockfd, &buffer[i], UDP_FRAME, 0, (const struct sockaddr*)&server, sizeof(server)) < 0)
        {
            fprintf(stderr, "Error in sendto()\n");
            return EXIT_FAILURE;
        }
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end); // End timing

    // Calculate and print the time taken to send the data
    uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    printf("Time to send %d wavs: %f[s]\n", WAVS, delta_us / 1e6f);
    printf("Finished...\n");

    return EXIT_SUCCESS;
}
