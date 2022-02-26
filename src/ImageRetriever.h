#include "FlirCamera.h"
#include "GenericUSBCamera.h"
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

#ifndef SKYPASTA_IMAGE_RETRIEVER_H
#define SKYPASTA_IMAGE_RETRIEVER_H

enum class CameraType
{
    FLIR,
    GenericUSB
};

const std::string defaultImageFilePath = "../Images";

/*
 * A class that hides away library specific functions
 */
class ImageRetriever
{
public:
    ImageRetriever(ImageTag *imageTag, FlirCamera *flirCamera, std::string imageFilePath = defaultImageFilePath);
    ImageRetriever(ImageTag *imageTag, GenericUSBCamera *genericUSBCamera, std::string imageFilePath = defaultImageFilePath);
    void releaseCamera();

    void startAcquisition();
    void stopAcquisition();
    void acquireImage(bool tagImages);
    
    void setAcquisitionMode(std::string AcquisitionMode);
    void setTriggerType(std::string triggerSourceToSet);
    void setTriggerSource(std::string triggerSourceToSet);
    void setTriggerMode(std::string triggerModeToSet);

    bool getIsCameraBusy()const {return isCameraBusy;};

private:
    void fileSetup();
    void getImage(std::string &imagePath, long * timestamp, bool getTimestamp);
    bool waitForCameraAvailability(const char* func);

    // Camera objects
    FlirCamera *flirCamera = nullptr;
    GenericUSBCamera *genericUSBCamera = nullptr;

    // The imagetag object that will put GPS information on the images
    ImageTag *imageTag = nullptr;
    CameraType cameraType;
    
    // Relative path to the Images folder
    std::string imageFilePath;

    // Keeps track of number of images taken during a single acquisition.
    int imageNumber = 0;

    // Used for making every single acquisition unique. Units: milliseconds till epoch.
    int64_t acquisitionStartTime = 0;

    // Used for making sure multiple commands are not send at the same time. See waitForCameraAvailability
    bool isCameraBusy = false;
};

#endif // SKYPASTA_IMAGE_RETRIEVER_H