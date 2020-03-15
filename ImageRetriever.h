//
// Created by Jonathan Hirsch on 2/17/20.
//
#include "FlirCamera.h"
#include "ImageTag.h"
#include <map>
#include <chrono>
#include <iomanip>
#include <sys/stat.h>

#ifndef SKYPASTA_CONTINUOUSACQUISITION_H
#define SKYPASTA_CONTINUOUSACQUISITION_H

enum class TriggerMode{
    CONTINUOUS,
    SINGLE_FRAME
};

class ImageRetriever {
public:
    ImageRetriever(const CameraPtr& cameraPtr, ImageTag *imageTag);

    void startAcquisition();
    void stopAcquisition();
    void releaseCamera(){cameraPtr = nullptr;};
    void triggerCameraOnce();
    void setTriggerMode(const TriggerMode triggerMode);

    void setContinuousRate(const int continuousRate){ this -> continuousRate = continuousRate;};
    int getContinuousRate()const {return continuousRate;}
    bool isRunning()const {return running;};

private:
    void configureImageRetriever();
    void acquireImagesContinuous(INodeMap& nodeMap);
    void triggerImageRetrieval(INodeMap& nodeMap);
    void acquireImage(INodeMap& nodeMap);

    CameraPtr cameraPtr = nullptr;
    ImageTag *imageTag;
    TriggerMode currentTriggerMode = TriggerMode::CONTINUOUS;
    std::map<TriggerMode, std::string> triggerModeMap;

    double totalTime = 0;
    int imageNumber = 0;
    int continuousRate = 1;
    bool running = false;
    bool stopFlag = false;
    bool singleFrameModeEnabled = true;
};



#endif

