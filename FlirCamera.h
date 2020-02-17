//
// Created by Jonathan Hirsch on 2/15/20.
//

#ifndef FLIR_FLIRCAMERA_H
#define FLIR_FLIRCAMERA_H

#include <iostream>
#include "Spinnaker.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#pragma once

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;

using namespace::std;

enum TriggerType
{
    SOFTWARE,
    HARDWARE
};

class FlirCamera {
public:
    FlirCamera();
    ~FlirCamera(){;};
    int cleanExit();
    void switchTrigger();
    int getNumCameras();
    TriggerType getTriggerType();
    void initialize();
    void configureTrigger();
    int setTrigger(TriggerType trigger);
    void setRate(int rate);
    int getRate(){return rate;}
    void stopAcquisition();
    void acquireImages();
    void disableTrigger();

private:
    SystemPtr system;
    TriggerType currentTrigger = SOFTWARE;
    CameraList cameraList;
    int numberOfCameras = 0;
    CameraPtr cameraPtr;
    int rate = 1; // Rate of images per second
    bool acquireNextImage = false;
    INodeMap *nodeMap;
};


#endif //FLIR_FLIRCAMERA_H
