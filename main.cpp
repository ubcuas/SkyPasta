#include <iostream>
#include <unistd.h>
#include <future>
#include "FlirCamera.h"
#include "ImageRetriever.h"
#include "SpinGenApi/SpinnakerGenApi.h"
#include "Telemetry.h"


#define PORT 5000


using namespace std;

static const int RATE = 2; // delay between each image acquisition trigger

bool stopFlag = false;

void acquireImagesFixedRate(int rate, ImageRetriever ca){
    ca.startAcquisition();

    while (!stopFlag){
        cout << "aquiring........." << endl;
        auto myFuture(async(launch::async, &ImageRetriever::triggerCameraOnce, &ca));
        if (stopFlag){
            break;
        }
        sleep(rate);

        cout << endl;
    }

    ca.stopAcquisition();
}

void readFromSocket(Telemetry telemetry){

    while (!stopFlag){
        telemetry.readData();
        if (stopFlag){
            break;
        }
        cout << endl;
    }
}


int main() {

    try {
        FlirCamera flirCamera;
        flirCamera.setTrigger(SOFTWARE);

        cout << "Cameras Connected: " << flirCamera.getNumCameras() << endl;
        ImageRetriever ca(flirCamera.getCamera());
        ca.setTriggerMode(SINGLE_FRAME);

        Telemetry telemetry("127.0.0.1",5000);
        telemetry.connectServer();


        auto myFuture(async(launch::async, acquireImagesFixedRate, RATE, ca));

        auto myFuture2(async(launch::async, readFromSocket, telemetry));

        sleep(20);
        stopFlag = true;

        myFuture.get();

        ca.releaseCamera();

        flirCamera.cleanExit();
    }
    catch (Exception e){
        cout << "Error in Main:  ";
        cout << e.what() << endl;
    }
}

