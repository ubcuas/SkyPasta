//
// Created by Jonathan Hirsch on 2/17/20.
//
#include "FlirCamera.h"
#ifndef SKYPASTA_CONTINUOUSACQUISITION_H
#define SKYPASTA_CONTINUOUSACQUISITION_H


class ImageRetriever {
public:
    ImageRetriever(CameraPtr cameraPtr);
    void setRate(int rate){this -> rate = rate;};
    int getRate(){return rate;}
    bool isRunning(){return running;};
    void startAcquisition();
    int stopAcquisition();
    void releaseCamera(){cameraPtr = nullptr;};
private:
    int rate = 1;
    bool running = false;
    CameraPtr cameraPtr = nullptr;
    void acquireImages();
    bool stopFlag = false;
    void configureImageRetriver();
    int serialNum = 12345;
    void triggerImageRetrieval(INodeMap& nodeMap);

    FILE* file;
};


#endif //SKYPASTA_CONTINUOUSACQUISITION_H
