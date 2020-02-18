//
// Created by Jonathan Hirsch on 2/17/20.
//
#include "FlirCamera.h"
#include <map>
#ifndef SKYPASTA_CONTINUOUSACQUISITION_H
#define SKYPASTA_CONTINUOUSACQUISITION_H

enum TriggerMode{
    CONTINUOUS,
    SINGLE_FRAME
    //MULTI_FRAME
};

class ImageRetriever {
public:
    ImageRetriever(const CameraPtr cameraPtr);
    void startAcquisition();
    int stopAcquisition();
    void releaseCamera(){cameraPtr = nullptr;};
    void triggerCameraOnce();

    void setTriggerMode(const TriggerMode triggerMode);
    void setContinuousRate(const int continousRate){ this -> continousRate = continousRate;};
    int getContinuousRate()const {return continousRate;}
    bool isRunning()const {return running;};

private:
    void configureImageRetriever();
    void acquireImagesContinuous(INodeMap& nodeMap);
    void triggerImageRetrieval(INodeMap& nodeMap);
    void acquireImage(INodeMap& nodeMap);


    CameraPtr cameraPtr = nullptr;
    TriggerMode currentTriggerMode = CONTINUOUS;
    map<TriggerMode, string> triggerModeMap;

    int continousRate = 1;
    bool running = false;
    bool stopFlag = false;
};


#endif
