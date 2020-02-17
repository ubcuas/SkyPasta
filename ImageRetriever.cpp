//
// Created by Jonathan Hirsch on 2/17/20.
//

#include "ImageRetriever.h"


ImageRetriever::ImageRetriever(CameraPtr cameraPtr){
    this -> cameraPtr = cameraPtr;

    if (cameraPtr == nullptr){
       this -> ~ContinuousAcquisition();
    }
}

void ImageRetriever::startAcquisition() {
    if (!running){
        stopFlag = false;
        running = true;
        acquireImages();
    }
    else{
        cout << "Camera already acquiring images" << endl;
    }
}

int ImageRetriever::stopAcquisition() {
    stopFlag = true;
    cout << "Stopping..."  << endl;
    return 0;
}

void ImageRetriever::acquireImages() {
    cout << "Begin Acquisition..." << endl;
    int ctr = 0;
    while (true){
        if (stopFlag){
            cout << "exiting acquisition loop" << endl;
            running = false;
            stopFlag = false;
            break;
        }
        ctr ++;

        cout << ":: Acquisition# " << ctr << endl;
        sleep(rate);
    }

    cout << "Acquisition Stopped" << endl << endl;
}