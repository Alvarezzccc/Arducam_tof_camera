
/**
 * Programa: capture_frames_for_test.cpp
 * 
 * Descripción:
 * Este programa se utiliza para capturar imágenes de un sensor de cámara Time-of-Flight (ToF) 
 * utilizando la API de Arducam y OpenCV. Permite al usuario capturar y guardar imágenes en tiempo 
 * real de las transmisiones de amplitud y vista previa de la cámara cada vez que se presiona la 
 * tecla ESPACIO. Las imágenes capturadas se guardan en una carpeta llamada "pictures" dentro del 
 * directorio donde se ejecuta el programa.
 * 
 * Funcionalidades clave:
 * - Inicialización y configuración de la cámara ToF Arducam.
 * - Visualización en tiempo real de las imágenes de amplitud y vista previa.
 * - Captura de imágenes de ambas transmisiones al presionar la tecla ESPACIO.
 * - Almacenamiento de imágenes capturadas en el directorio "pictures".
 * - Cierre seguro de la cámara y salida del programa mediante la tecla ESC.
 * 
 * Uso:
 * Ejecute el programa y abra la ventana de vista previa de OpenCV. Presione la tecla CAPTURE_KEY 
 * para capturar imágenes. Las imágenes se guardarán automáticamente en la carpeta "pictures".
 * Presione ESC para salir del programa.
 * 
 * Autor: Daniel Álvarez Conde
 * Fecha: 19/12/2023
 * Versión: 0.1
 * 
 */


#include "ArducamTOFCamera.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <chrono>
#include <string>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <iomanip>
#include <pigpio.h>
#include <sstream>
//#include <filesystem>


/*
* ASCII representation for the key pressed 
* 32 -> SPACE
*
*/

#define CAPTURE_KEY 32
#define BUTTON_PIN 17 // GPIO17

// Namespace for filesystem
//namespace fs = std::filesystem;

// MAX_DISTANCE value modifiable  is 2 or 4
#define MAX_DISTANCE 4
using namespace Arducam;
using namespace std;

bool captureRequested = false;

std::string getCurrentDateTime() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%d-%m-%Y_%H:%M:%S");
    return ss.str();
}

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

bool checkGUIAvailable() {
    // Function to check if a GUI is available
    // This is a simplistic check; you might need a more robust check depending on your environment
    return getenv("DISPLAY") != nullptr;
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

void buttonCallback(int gpio, int level, uint32_t tick) {
    captureRequested = true;
}

int main()
{
    ArducamTOFCamera tof;
    ArducamFrameBuffer *frame;
    if (tof.open(Connection::CSI))
    {
        std::cerr << "initialization failed" << std::endl;
        exit(-1);
    }

    if (tof.start(FrameType::DEPTH_FRAME))
    {
        std::cerr << "Failed to start camera" << std::endl;
        exit(-1);
    }
    //  Modify the range also to modify the MAX_DISTANCE
    tof.setControl(CameraCtrl::RANGE, MAX_DISTANCE);
    CameraInfo tofFormat = tof.getCameraInfo();

    // Initialize pigpio
    if (gpioInitialise() < 0) {
        cerr << "Failed to initialize pigpio." << endl;
        return -1;
    }

    printf("Initialiazing pins\n");
    gpioSetMode(BUTTON_PIN, PI_INPUT);
    gpioSetPullUpDown(BUTTON_PIN, PI_PUD_UP);
    gpioSetISRFunc(BUTTON_PIN, FALLING_EDGE, 0, buttonCallback);
    printf("Pins initialized");

    // Image pointers
    float *depth_ptr;
    float *amplitude_ptr;
    uint8_t *preview_ptr = new uint8_t[tofFormat.height * tofFormat.width];

    
    // Check for GUI availability
    bool guiAvailable = checkGUIAvailable();
    if (guiAvailable) {
        // Opencv Window
        cv::namedWindow("preview", cv::WINDOW_AUTOSIZE);
    }

    // Check and create pictures directory in /home/gafas/Desktop
    const char* picturesDir = "/home/gafas/Desktop/pictures";
    if (mkdir(picturesDir, 0777) && errno != EEXIST) {
        cerr << "Failed to create directory: " << picturesDir << endl;
        return -1;
    }

    // Main loop; Until process is stopped
    for (;;)
    {
        frame = tof.requestFrame(200);
        if (frame != nullptr)
        {
            depth_ptr = (float *)frame->getData(FrameType::DEPTH_FRAME);
            amplitude_ptr = (float *)frame->getData(FrameType::AMPLITUDE_FRAME);
            getPreview(preview_ptr, depth_ptr, amplitude_ptr);

            cv::Mat result_frame(tofFormat.height, tofFormat.width, CV_8U, preview_ptr);
            cv::Mat amplitude_frame(tofFormat.height, tofFormat.width, CV_32F, amplitude_ptr);
            
            result_frame = matRotateClockWise180(result_frame);
            amplitude_frame = matRotateClockWise180(amplitude_frame);

            cv::applyColorMap(result_frame, result_frame, cv::COLORMAP_JET);
            
            if (guiAvailable) {
                amplitude_frame.convertTo(amplitude_frame, CV_8U, 255.0 / 1024, 0);
                cv::imshow("amplitude", amplitude_frame);
                cv::imshow("preview", result_frame);
            }

            int key = guiAvailable ? cv::waitKey(1) : std::cin.get();

            // Capture and save images on CAPTURE_KEY key press
            if (key == CAPTURE_KEY || captureRequested) { 
                std::string timestamp = getCurrentDateTime();
                std::string fullPathPreview = std::string(picturesDir) + "/preview_" + timestamp + ".jpg";
                std::string fullPathAmplitude = std::string(picturesDir) + "/amplitude_" + timestamp + ".jpg";

                cv::imwrite(fullPathPreview, result_frame);
                cv::imwrite(fullPathAmplitude, amplitude_frame);
                std::cout << "Captured and saved images." << std::endl;

                captureRequested = false;
            }
            else if (key == 27) { // ASCII value for ESC
                break;
            }
        }
        tof.releaseFrame(frame);
    }

    if (tof.stop())
        exit(-1);
    if (tof.close())
        exit(-1);

    return 0;
}