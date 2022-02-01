#include "FlirCamera.h"
#include "ImageTag.h"
#include "string.h"
#include <map>
#include <chrono>
#include <iomanip>
#include <sys/stat.h>

#pragma once

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;

#ifndef SKYPASTA_CONTINUOUSACQUISITION_H
#define SKYPASTA_CONTINUOUSACQUISITION_H

enum class CameraType{
    FLIR
};

class ImageRetriever
{
public:
    ImageRetriever(ImageTag *imageTag, CameraType cameraType, FlirCamera *flirCamera, string filePath = "..");
    void releaseCamera();

    void startAcquisition();
    void stopAcquisition();
    void acquireImage();
    
    void setAcquisitionMode(std::string AcquisitionMode);
    void setTriggerType(std::string triggerSourceToSet);
    void setTriggerSource(std::string triggerSourceToSet);
    void setTriggerMode(std::string triggerModeToSet);

    bool getIsCameraBusy()const {return isCameraBusy;};

private:
    void getImage(std::string &imageName, long * timestamp);
    bool waitForCameraAvailability(const char* func);

    FlirCamera *flirCamera = nullptr;
    ImageTag *imageTag = nullptr;
    CameraType cameraType;
    string filePath;

    double totalTime = 0;
    int imageNumber = 0;
    bool isCameraBusy = false;
    int acquistionStartTime = 0;
};

#endif