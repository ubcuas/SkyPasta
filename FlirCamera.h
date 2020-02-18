//
// Created by Jonathan Hirsch on 2/15/20.
//

#ifndef SKYPASTA_FLIRCAMERA_H
#define SKYPASTA_FLIRCAMERA_H

#include <iostream>
#include "Spinnaker.h"

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
    void initialize();
    void configureTrigger();
    int cleanExit();

    int setTrigger(const TriggerType trigger);
    int& getNumCameras();
    CameraPtr& getCamera() {return this-> cameraPtr;}
    TriggerType getTriggerType();

private:
    void disableTrigger();

    SystemPtr system;
    CameraPtr cameraPtr;
    TriggerType currentTrigger = SOFTWARE;
    CameraList cameraList;
    int numberOfCameras = 0;


};
#endif //SKYPASTA_FLIRCAMERA_H
