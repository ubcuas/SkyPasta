//
// Created by Jonathan Hirsch on 2/17/20.
//

#include "ImageRetriever.h"

using namespace std;

ImageRetriever::ImageRetriever(const CameraPtr& cameraPtr, ImageTag *imageTag) {
    this->cameraPtr = cameraPtr;
    this->imageTag = imageTag;

    triggerModeMap[TriggerMode::CONTINUOUS] = "Continuous";
    triggerModeMap[TriggerMode::SINGLE_FRAME] = "SingleFrame";

    if (cameraPtr == nullptr) {
        cout << "No Camera devices attached to Image Retriever " << endl;
        return;
    }
    if (!cameraPtr->IsInitialized()) {
        cout << "camera not initialized" << endl;
        return;
    } else {
        configureImageRetriever();
    }
}

void ImageRetriever::setTriggerMode(TriggerMode triggerMode) {
    this->currentTriggerMode = triggerMode;
    configureImageRetriever();
}

void ImageRetriever::triggerCameraOnce() {
    cameraPtr->BeginAcquisition();
    if (singleFrameModeEnabled) {
        acquireImage(cameraPtr->GetNodeMap());
    }
    cameraPtr->EndAcquisition();
}

// Configures trigger mode and insures images can be saved by saving a test file and deleting it
// Creates image directory if necessary
void ImageRetriever::configureImageRetriever() {

    mkdir("../Images", 0);
    FILE *tempFile = fopen("../Images/test.txt", "w+");
    if (tempFile == nullptr) {
        cout << "Failed to create file in current folder.  Please check "
                "permissions."
             << endl;
    }
    fclose(tempFile);
    remove("../Images/test.txt");

    INodeMap &nodeMap = cameraPtr->GetNodeMap();

    gcstring selectedMode;
    const char *selectedModeArr = triggerModeMap.at(currentTriggerMode).c_str();
    selectedMode.assign(selectedModeArr);


    // Set acquisition mode to continuous
    CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
    if (!IsAvailable(ptrAcquisitionMode) || !IsWritable(ptrAcquisitionMode)) {
        cout << "Unable to set acquisition mode to continuous (node retrieval). Aborting..." << endl << endl;
        return;
    }

    CEnumEntryPtr ptrAcquisitionModeSet = ptrAcquisitionMode->GetEntryByName(selectedMode);
    if (!IsAvailable(ptrAcquisitionModeSet) || !IsReadable(ptrAcquisitionModeSet)) {
        cout << "Unable to set acquisition mode to continuous (entry '" << selectedMode << "' retrieval). Aborting..."
             << endl
             << endl;
        return;
    }

    int64_t acquisitionModeSet = ptrAcquisitionModeSet->GetValue();
    ptrAcquisitionMode->SetIntValue(acquisitionModeSet);
    cout << "Acquisition mode set to " << selectedMode << "..." << endl;
}

// Reset all flags and start acquisition if camera is not currently running
void ImageRetriever::startAcquisition() {

    INodeMap &nodeMap = cameraPtr->GetNodeMap();

    if (!running) {
        stopFlag = false;
        running = true;
        imageNumber = 0;
        totalTime = 0;

        if (cameraPtr == nullptr) {
            cout << "No Camera devices attached to Image Retriever " << endl;
            return;
        }

        cout << "Begin Acquisition..." << endl;
        switch (currentTriggerMode) {
            case TriggerMode::SINGLE_FRAME: singleFrameModeEnabled = true; break;
            case TriggerMode::CONTINUOUS: acquireImagesContinuous(nodeMap); break;
            default: stopAcquisition();break;
        }

    } else {
        cout << "Camera already acquiring images" << endl;
    }
}

void ImageRetriever::stopAcquisition() {
    stopFlag = true;
    cout << "Stopping..." << endl;
    singleFrameModeEnabled = false;

    if (cameraPtr->IsStreaming()) {
        cameraPtr->EndAcquisition();
    }

    cout << "Acquisition Stopped" << endl << endl;
    cout << "Number of images Acquired: " << imageNumber << endl;
    cout << "Total Time Taken: " << totalTime << setprecision(6) << "sec" << endl;
    double averageTime = totalTime / imageNumber;
    cout << "Average time per image: " << averageTime << setprecision(6) << "sec" << endl << endl;
}

// acquires a single image
// Converts Image to BayerRG8,
// calculates image timestamp_telem and capture duration
// adds image location & timestamp_telem to ImageTag
void ImageRetriever::acquireImage(INodeMap &nodeMap) {
    // start clock, acquire image, and stop clock
    imageNumber++;
    std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
    triggerImageRetrieval(nodeMap);
    ImagePtr pResultImage = cameraPtr->GetNextImage();
    std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();

    cout << "_____________________________" << endl;
    cout << ":: Acquisition# " << imageNumber << endl;
    if (pResultImage->IsIncomplete()) {
        cout << "Image incomplete with image status " << pResultImage->GetImageStatus() << "..." << endl << endl;
        pResultImage->Release();

    } else { // Convert image
        cout << "Grabbed image: W*H = " << pResultImage->GetWidth() << "*" << pResultImage->GetHeight() << endl;

        ImagePtr convertedImage = pResultImage->Convert(PixelFormat_BayerRG8, HQ_LINEAR);
        ostringstream filename;
        filename << "../Images/Trigger-" << imageNumber << ".jpg";
        convertedImage->Save(filename.str().c_str());
        cout << "Image saved at " << filename.str() << endl;

        pResultImage->Release();

        // calculate capture duration, save timestamp_telem and image location to imageTag
        std::chrono::duration<double, std::milli> timeSpan = endTime - startTime;
        long timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
        cout << "time taken by program: " << timeSpan.count() << endl;
        cout << "Timestamp:" << setprecision(20) << timestamp << endl;
        totalTime += timeSpan.count();
        imageTag->addImage(filename.str(), timestamp - timeSpan.count() / 2);
    }
}

// Continuously acquire images, sleep a fixed time between each acquisition
void ImageRetriever::acquireImagesContinuous(INodeMap &nodeMap) {
    cameraPtr->BeginAcquisition();
    while (true) {
        if (stopFlag) {
            cout << "exiting acquisition loop" << endl;
            running = false;
            stopFlag = false;
            break;
        }
        acquireImage(nodeMap);

        cout << endl;
        sleep(continuousRate);
    }
    cameraPtr->EndAcquisition();
}

// Capture an image.
// Uses Software Trigger
// Camera must run BeginAcquisition() before running this method
void ImageRetriever::triggerImageRetrieval(INodeMap &nodeMap) {
    CCommandPtr ptrSoftwareTriggerCommand = nodeMap.GetNode("TriggerSoftware");
    if (!IsAvailable(ptrSoftwareTriggerCommand) || !IsWritable(ptrSoftwareTriggerCommand)) {
        cout << "Unable to execute trigger. Aborting..." << endl;
        return;
    }

    ptrSoftwareTriggerCommand->Execute();
}