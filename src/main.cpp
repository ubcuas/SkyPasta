#include <iostream>
#include <unistd.h>
#include <future>
#include "ImageRetriever.h"
#include "SpinGenApi/SpinnakerGenApi.h"
#include "Telemetry.h"


constexpr int PORT = 5000;
constexpr auto ADDRESS  = "127.0.0.1";
constexpr int RATE = 2; // delay between each image acquisition trigger

const int NUM_THREADS = 8; // max number of threads for getting images 

constexpr int SECONDS_TO_RUN = -1; // Set to -1 to run indefinitely

using namespace std;

bool stopFlag = false;

void acquireImagesFixedRate(int rate, ImageRetriever *imageRetriever)
{
    imageRetriever->startAcquisition();

    while (!stopFlag){
        cout << "Acquiring Image" << endl;
        auto triggerCameraOnceFuture(async(launch::async, &ImageRetriever::acquireImage, imageRetriever));
        if (stopFlag)
        {
            break;
        }
        sleep(rate);

        cout << endl;
    }

    imageRetriever->stopAcquisition();
}

void acquireImages(ImageRetriever *imageRetriever)
{
    while (!stopFlag)
    {
        imageRetriever->acquireImage();
        if (stopFlag)
        {
            break;
        }
    }
}

// Reads Telemetry data from socket, exits on error from telemetry.
void readFromSocket(Telemetry *telemetry){
    int reconnectAttempts = 0;
//    if (!telemetry->isConnected()){
//        cout << "Telemetry error: Not connected" << endl;
//        return;
//    }
    cout << "Here:" << endl;
    while (!stopFlag){
        if (telemetry->readData() == -1 || !telemetry->isConnected()){
            cout << "Error with server, attempting to reconnect..." << endl;

            telemetry->connectServer();

            if (telemetry->isConnected()){
                reconnectAttempts = 0;
                continue;
            }
            //reconnectAttempts ++;
            if (reconnectAttempts > 5 || stopFlag){
                return;
            }
            sleep(3);
        }
        if (stopFlag){
            return;
        }
        cout << endl;
    }
}

void tagImages(ImageTag *imageTag){
    while(!stopFlag) {
       imageTag->processNextImage();
    }
}

int main(int argc, char *argv[])
{
    try
    {
        cout << "SkyPasta is booting up..." << endl;

        ImageTag imageTag;

        cout << "Camera setup starting" << endl;
        FlirCamera flirCamera;
        if (workingMode == Modes::triggerMode)
        {
            flirCamera.setDefaultSettings();
        }
        else if (workingMode == Modes::continuousMode)
        {
            flirCamera.setDefaultSettings("Continous", "FrameStart", "Software", "Off", "BayerRG8");
        }
        cout << "Camera setup complete" << endl;

        cout << "ImageRetriever setup starting" << endl;
        if (argc == 2) 
        {
            ImageRetriever imageRetriever(&imageTag, CameraType::FLIR, &flirCamera, argv[1]);
        }
        else if (argc == 1) 
        {
            ImageRetriever imageRetriever(&imageTag, CameraType::FLIR, &flirCamera);
        }
        else 
        {
            flirCamera.cleanExit();
            throw std::invalid_argument("Invalid arguments for ImageRetriever setup");
        }
        cout << "ImageRetriever setup complete" << endl;

        cout << "Telemetry setup starting" << endl;
        Telemetry telemetry(ADDRESS,PORT, &imageTag);
        telemetry.connectServer();
        cout << "Telemetry setup complete" << endl;

        thread threadList[NUM_THREADS]; 
        if (workingMode == Modes::triggerMode)
        {
            threadList[0] = thread(acquireImagesFixedRate,  RATE, &imageRetriever);
        }
        else if (workingMode == Modes::continuousMode) 
        {
            imageRetriever.startAcquisition();

            for (int i = 0; i < NUM_THREADS; i++)
            {
                threadList[i] = thread(acquireImages, &imageRetriever);
            }
        }


        auto readFromSocketFuture(async(launch::async, readFromSocket, &telemetry));
        auto processNextImageFuture(async(launch::async, tagImages, &imageTag));

        cout << "Boot up complete..." << endl;

        int count = 0;
        while(!stopFlag)
        {
            if(count == SECONDS_TO_RUN) {stopFlag = true;} 
            std::this_thread::sleep_for(std::chrono::milliseconds(1000)); 
            count++;
        }

        cout << "Stopping..." << endl;

        if (workingMode == Modes::triggerMode)
        {
            threadList[0].join();
        }
        else if (workingMode == Modes::continuousMode) 
        {
            for (int i = 0; i < NUM_THREADS; i++)
            {
                threadList[i].join();
            }

            imageRetriever.stopAcquisition();
        }

        imageRetriever.releaseCamera();
    }
    catch (const Exception& e)
    {
        cout << "Error in main:  ";
        cout << e.what() << endl;
    }
}

