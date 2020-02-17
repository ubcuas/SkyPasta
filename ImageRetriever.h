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
    ImageRetriever(CameraPtr cameraPtr);
    void setRate(int rate){this -> rate = rate;};
    int getRate(){return rate;}
    bool isRunning(){return running;};
    void startAcquisition();
    int stopAcquisition();
    void releaseCamera(){cameraPtr = nullptr;};
    void setTriggerMode(TriggerMode triggerMode){this -> currentTriggerMode = triggerMode; configureImageRetriver();};
    void triggerCamera();

private:
    int rate = 1;
    bool running = false;
    CameraPtr cameraPtr = nullptr;
    void acquireImagesContinuous(INodeMap& nodeMap);
    void acquireImagesSingleFrame(INodeMap& nodeMap);
    bool stopFlag = false;
    void configureImageRetriver();
    int serialNum = 12345;
    void triggerImageRetrieval(INodeMap& nodeMap);
    TriggerMode currentTriggerMode = CONTINUOUS;
    void acquireImage(INodeMap& nodeMap);

    FILE* file;


    map<TriggerMode, string> triggerModeMap;


};


#endif //SKYPASTA_CONTINUOUSACQUISITION_H
