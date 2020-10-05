//
// Created by Jonathan Hirsch on 2/15/20.
//

#ifndef FLIR_FLIRCAMERA_H
#define FLIR_FLIRCAMERA_H

#include <iostream>
#include "Spinnaker.h"

#pragma once

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;

enum class TriggerType
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

    void setTrigger(TriggerType trigger);
    int getNumCameras() const;
    CameraPtr& getCamera() {return this-> cameraPtr;}
    TriggerType getTriggerType();

private:
    void disableTrigger();

    SystemPtr system;
    CameraPtr cameraPtr;
    TriggerType currentTrigger = TriggerType::SOFTWARE;
    CameraList cameraList;
    int numberOfCameras = 0;
};
#endif //FLIR_FLIRCAMERA_H
