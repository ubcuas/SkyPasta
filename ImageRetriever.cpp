//
// Created by Jonathan Hirsch on 2/17/20.
//

#include "ImageRetriever.h"


ImageRetriever::ImageRetriever(CameraPtr cameraPtr){
    this -> cameraPtr = cameraPtr;
    if (cameraPtr == nullptr){
        cout << "No Camera devices attached to Image Retriever " << endl;
        return;
    }
    else{
        configureImageRetriver();
    }
}

void ImageRetriever::configureImageRetriver() {


    FILE* tempFile = fopen("test.txt", "w+");
    if (tempFile == nullptr)
    {
        cout << "Failed to create file in current folder.  Please check "
                "permissions."
             << endl;
        cout << "Press Enter to exit..." << endl;
        getchar();
        return;
    }
    fclose(tempFile);
    remove("test.txt");

    INodeMap &nodeMap = cameraPtr -> GetNodeMap();

    // Set acquisition mode to continuous
    CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
    if (!IsAvailable(ptrAcquisitionMode) || !IsWritable(ptrAcquisitionMode))
    {
        cout << "Unable to set acquisition mode to continuous (node retrieval). Aborting..." << endl << endl;
        return;
    }

    CEnumEntryPtr ptrAcquisitionModeContinuous = ptrAcquisitionMode->GetEntryByName("Continuous");
    if (!IsAvailable(ptrAcquisitionModeContinuous) || !IsReadable(ptrAcquisitionModeContinuous))
    {
        cout << "Unable to set acquisition mode to continuous (entry 'continuous' retrieval). Aborting..." << endl
             << endl;
        return;
    }

    int64_t acquisitionModeContinuous = ptrAcquisitionModeContinuous->GetValue();

    ptrAcquisitionMode->SetIntValue(acquisitionModeContinuous);

    cout << "Acquisition mode set to continuous..." << endl;

}

void ImageRetriever::startAcquisition() {
    if (!running){
        stopFlag = false;
        running = true;
        acquireImages();
    }
    else{
        cout << "Camera already acquiring images" << endl;
    }
}

int ImageRetriever::stopAcquisition() {
    stopFlag = true;
    cout << "Stopping..."  << endl;
    return 0;
}

void ImageRetriever::acquireImages() {

    cameraPtr ->BeginAcquisition();

    cout << "Begin Acquisition..." << endl;

    if (cameraPtr == nullptr){
        cout << "No Camera devices attached to Image Retriever " << endl;
        return;
    }

    INodeMap& nodeMap = cameraPtr ->GetNodeMap();

    int ctr = 0;
    while (true){
        if (stopFlag){
            cout << "exiting acquisition loop" << endl;
            running = false;
            stopFlag = false;
            break;
        }
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
            filename << "Trigger-" << ctr << ".jpg";

            convertedImage->Save(filename.str().c_str());

            cout << "Image saved at " << filename.str() << endl;
        }

        pResultImage->Release();
        cout << endl;
        sleep(rate);
    }

    cameraPtr ->EndAcquisition();
    cout << "Acquisition Stopped" << endl << endl;
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