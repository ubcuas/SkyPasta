#include <iostream>
#include <unistd.h>
#include <future>
#include "FlirCamera.h"
#include "ImageRetriever.h"
#include "SpinGenApi/SpinnakerGenApi.h"
#include "Telemetry.h"
#include <exiv2/exiv2.hpp>


constexpr int PORT = 5000;
constexpr auto ADDRESS  = "127.0.0.1";
constexpr int RATE = 2; // delay between each image acquisition trigger


using namespace std;



bool stopFlag = false;

void acquireImagesFixedRate(int rate, ImageRetriever *imageRetriever){
    imageRetriever->startAcquisition();

    while (!stopFlag){
        cout << "aquiring........." << endl;
        auto triggerCameraOnceFuture(async(launch::async, &ImageRetriever::triggerCameraOnce, imageRetriever));
        if (stopFlag){
            break;
        }
        sleep(rate);

        cout << endl;
    }

    imageRetriever -> stopAcquisition();
}

void readFromSocket(Telemetry *telemetry){

    if (!telemetry->isConnected()){
        cout << "Telemetry error: Not connected" << endl;
        return;
    }
    while (!stopFlag && telemetry->isConnected()){
        if (telemetry->readData() == -1){
            return;
        }
        if (stopFlag){
            break;
        }
        cout << endl;
    }
}


int main() {

    try {
        FlirCamera flirCamera;
        flirCamera.setTrigger(TriggerType::SOFTWARE);

        cout << "Cameras Connected: " << flirCamera.getNumCameras() << endl;
        ImageRetriever imageRetriever(flirCamera.getCamera());
        imageRetriever.setTriggerMode(TriggerMode::SINGLE_FRAME);

        Telemetry telemetry(ADDRESS,PORT);
        telemetry.connectServer();


        auto acquireImagesFixedRateFuture(async(launch::async, acquireImagesFixedRate, RATE, &imageRetriever));

        auto readFromSocketFuture(async(launch::async, readFromSocket, &telemetry));

        sleep(20);
        stopFlag = true;

        acquireImagesFixedRateFuture.get();

        imageRetriever.releaseCamera();

        flirCamera.cleanExit();
    }
    catch (const Exception& e){
        cout << "Error in Main:  ";
        cout << e.what() << endl;
    }
}

