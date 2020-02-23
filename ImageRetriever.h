//
// Created by Jonathan Hirsch on 2/17/20.
//
#include "FlirCamera.h"
#include "ImageTag.h"

#include <map>
#ifndef SKYPASTA_CONTINUOUSACQUISITION_H
#define SKYPASTA_CONTINUOUSACQUISITION_H

enum class TriggerMode{

    CONTINUOUS,
    SINGLE_FRAME
    //MULTI_FRAME
};

class ImageRetriever {
public:

    ImageRetriever(const CameraPtr cameraPtr, ImageTag *imageTag);

    void startAcquisition();
    int stopAcquisition();
    void releaseCamera(){cameraPtr = nullptr;};
    void triggerCameraOnce();

    void setTriggerMode(const TriggerMode triggerMode);

    void setContinuousRate(const int continousRate){ this -> continuousRate = continousRate;};
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
    map<TriggerMode, string> triggerModeMap;

    int continuousRate = 1;
    bool running = false;
    bool stopFlag = false;
    double totalTime = 0;
    int ctr = 0;
    bool singleFrameModeEnabled = true;

    struct timeval startTime, endTime;
};


#endif

