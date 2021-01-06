#include "FlirCamera.h"
#include "ImageTag.h"
#include <map>
#include <chrono>
#include <iomanip>
#include <sys/stat.h>

#ifndef SKYPASTA_CONTINUOUSACQUISITION_H
#define SKYPASTA_CONTINUOUSACQUISITION_H

enum class CameraType{
    FLIR
};

class ImageRetriever
{
public:
    ImageRetriever(ImageTag *imageTag, CameraType cameraType, FlirCamera *flirCamera);
    void releaseCamera();

    void startAcquisition();
    void stopAcquisition();
    void acquireImage();
    
    void setAcquisitionMode(AcquisitionMode AcquisitionMode);
    void setTriggerSource(TriggerSourceEnums triggerSourceToSet);
    void setTriggerMode(TriggerModeEnums triggerModeToSet);

    bool isCameraBusy()const {return isCameraBusy;};

private:
    void configureImageRetriever();
    void acquireImageContinuous();
    void getImage(const string image, const long timestamp);
    bool waitForCameraAvailability(const char* func);
    void sleepWrapper(int milliseconds);

    FlirCamera *flirCamera = nullptr;
    ImageTag *imageTag = nullptr;
    CameraType cameraType;

    double totalTime = 0;
    int imageNumber = 0;
    bool isCameraBusy = false;
};

#endif