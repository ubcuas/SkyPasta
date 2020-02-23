//
// Created by Jonathan Hirsch on 2/17/20.
//

#include "ImageRetriever.h"
#include <sys/time.h>
#include <iomanip>
#include <sys/stat.h>


namespace {
    double calculateTimeTaken (timeval& startTime, timeval& endTime){

        double timeTaken;

        timeTaken = (startTime.tv_sec - startTime.tv_sec) * 1e6;
        timeTaken = (timeTaken + (endTime.tv_usec -
                                  startTime.tv_usec)) * 1e-6;

        cout << "Time taken by program is : " << fixed
             << timeTaken << setprecision(6);
        cout << " sec" << endl;

        return timeTaken;
    }
}

ImageRetriever::ImageRetriever(const CameraPtr cameraPtr){
    this -> cameraPtr = cameraPtr;

    triggerModeMap[TriggerMode::CONTINUOUS] = "Continuous";
    triggerModeMap[TriggerMode::SINGLE_FRAME] = "SingleFrame";
    //triggerModeMap[MULTI_FRAME] = "MultiFrame";

    if (cameraPtr == nullptr){
        cout << "No Camera devices attached to Image Retriever " << endl;
        return;
    }
    if (cameraPtr-> IsInitialized() == false){
        cout << "camera not initialized" << endl;
        return;
    }
    else{
        configureImageRetriever();
    }
}

void ImageRetriever::setTriggerMode(TriggerMode triggerMode){
    this -> currentTriggerMode = triggerMode;
    configureImageRetriever();
}


void ImageRetriever::triggerCameraOnce() {
    cameraPtr ->BeginAcquisition();
    if (singleFrameModeEnabled == true){
        acquireImage(cameraPtr -> GetNodeMap());
    }
    cameraPtr ->EndAcquisition();
}

void ImageRetriever::configureImageRetriever() {

    mkdir("../Images", 0);
    FILE* tempFile = fopen("../Images/test.txt", "w+");
    if (tempFile == nullptr)
    {
        cout << "Failed to create file in current folder.  Please check "
                "permissions."
             << endl;
    }
    fclose(tempFile);
    remove("../Images/test.txt");

    INodeMap &nodeMap = cameraPtr -> GetNodeMap();

    gcstring selectedMode;
    const char *selectedModeArr = triggerModeMap.at(currentTriggerMode).c_str();
    selectedMode.assign(selectedModeArr);


    // Set acquisition mode to continuous
    CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
    if (!IsAvailable(ptrAcquisitionMode) || !IsWritable(ptrAcquisitionMode))
    {
        cout << "Unable to set acquisition mode to continuous (node retrieval). Aborting..." << endl << endl;
        return;
    }

    CEnumEntryPtr ptrAcquisitionModeSet = ptrAcquisitionMode->GetEntryByName(selectedMode);
    if (!IsAvailable(ptrAcquisitionModeSet) || !IsReadable(ptrAcquisitionModeSet))
    {
        cout << "Unable to set acquisition mode to continuous (entry '" << selectedMode << "' retrieval). Aborting..." << endl
             << endl;
        return;
    }

    int64_t acquisitionModeSet = ptrAcquisitionModeSet->GetValue();

    ptrAcquisitionMode->SetIntValue(acquisitionModeSet);

    cout << "Acquisition mode set to " <<  selectedMode << "..." << endl;

}

void ImageRetriever::startAcquisition() {

    INodeMap& nodeMap = cameraPtr ->GetNodeMap();

    if (!running){
        stopFlag = false;
        running = true;
        ctr = 0;
        totalTime = 0;

        if (cameraPtr == nullptr){
            cout << "No Camera devices attached to Image Retriever " << endl;
            return;
        }


        cout << "Begin Acquisition..." << endl;
        switch (currentTriggerMode){
            case TriggerMode::SINGLE_FRAME: singleFrameModeEnabled = true; break;
            case TriggerMode::CONTINUOUS: acquireImagesContinuous(nodeMap); break;
            default: stopAcquisition(); break;
        }

    }
    else{
        cout << "Camera already acquiring images" << endl;
    }
}

int ImageRetriever::stopAcquisition() {
    stopFlag = true;
    cout << "Stopping..."  << endl;
    singleFrameModeEnabled = false;

    if (cameraPtr->IsStreaming()) {
        cameraPtr->EndAcquisition();
    }

    cout << "Acquisition Stopped" << endl << endl;

    cout << "Number of images Acquired: " << ctr << endl;
    cout << "Total Time Taken: " << totalTime << setprecision(6) << "sec"<<  endl;
    double averageTime = totalTime / ctr;
    cout << "Average time per image: " << averageTime << setprecision(6) << "sec"<< endl;

    cout << endl;

    return 0;
}

void ImageRetriever::acquireImage(INodeMap &nodeMap) {

    gettimeofday(&startTime, NULL);
    ctr ++;
    triggerImageRetrieval(nodeMap);

    ImagePtr pResultImage = cameraPtr ->GetNextImage();
    cout << "_____________________________" << endl;
    cout << ":: Acquisition# " << ctr << endl;
    if (pResultImage->IsIncomplete())
    {
        cout << "Image incomplete with image status " << pResultImage->GetImageStatus() << "..." << endl
             << endl;
    }
    else{
        cout << "Grabbed image: W*H = " << pResultImage ->GetWidth() << "*" << pResultImage->GetHeight() << endl;


        ImagePtr convertedImage = pResultImage->Convert(PixelFormat_Mono8, HQ_LINEAR);
        ostringstream filename;
        filename << "../Images/Trigger-" << ctr << ".jpg";

        convertedImage->Save(filename.str().c_str());

        cout << "Image saved at " << filename.str() << endl;
    }

    pResultImage->Release();

    gettimeofday(&endTime, NULL);

    totalTime += calculateTimeTaken(startTime, endTime);
}

void ImageRetriever::acquireImagesContinuous(INodeMap& nodeMap) {
    cameraPtr ->BeginAcquisition();
    while (true){
        if (stopFlag){
            cout << "exiting acquisition loop" << endl;
            running = false;
            stopFlag = false;
            break;
        }
        acquireImage(nodeMap);

        cout << endl;
        sleep(continuousRate);
    }
    cameraPtr ->EndAcquisition();
}

void ImageRetriever::triggerImageRetrieval(INodeMap& nodeMap) {
    CCommandPtr ptrSoftwareTriggerCommand = nodeMap.GetNode("TriggerSoftware");
    if (!IsAvailable(ptrSoftwareTriggerCommand) || !IsWritable(ptrSoftwareTriggerCommand))
    {
        cout << "Unable to execute trigger. Aborting..." << endl;
        return;
    }

    ptrSoftwareTriggerCommand->Execute();
}