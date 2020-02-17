//
// Created by Jonathan Hirsch on 2/15/20.
//

#include "FlirCamera.h"


FlirCamera::FlirCamera(){
    system = System::GetInstance();
    initialize();

}

void FlirCamera::initialize(){
    cameraList = system -> GetCameras();
    numberOfCameras = cameraList.GetSize();

    if (numberOfCameras <= 0){
        cout << "No Cameras Detected" << endl;
    }

    cameraPtr = cameraList.GetByIndex(0);
    cameraPtr -> Init();

    nodeMap = &cameraPtr ->GetNodeMap();



    configureTrigger();
}

int FlirCamera::cleanExit() {
    try {
        disableTrigger();
        if (cameraPtr != nullptr){
            cameraPtr->DeInit();
            cameraPtr = nullptr;
        }
        cameraList.Clear();
        system -> ReleaseInstance();
    }
    catch (Exception& e){
        cout << "Exception in cleanExit: ";
        cout << e.what() << endl;
        return -1;
    }
        cout << "Exiting Cleanly..." << endl;
    return 0;
}

TriggerType FlirCamera::getTriggerType() {
    return currentTrigger;
}

int FlirCamera::getNumCameras() {
    return numberOfCameras;
}

int FlirCamera::setTrigger(TriggerType trigger) {
    if (trigger != this -> currentTrigger){
        configureTrigger();
    }
    return 0;
}

void FlirCamera::configureTrigger(){


    CEnumerationPtr ptrTriggerMode = nodeMap -> GetNode("TriggerMode");
    disableTrigger();

    //
    // Select trigger source
    //
    // *** NOTES ***
    // The trigger source must be set to hardware or software while trigger
    // mode is off.
    //
    CEnumerationPtr ptrTriggerSource = nodeMap -> GetNode("TriggerSource");
    if (!IsAvailable(ptrTriggerSource) || !IsWritable(ptrTriggerSource))
    {
        cout << "Unable to set trigger mode (node retrieval). Aborting..." << endl;
        return;
    }

    if (currentTrigger == SOFTWARE)
    {
        // Set trigger mode to software
        CEnumEntryPtr ptrTriggerSourceSoftware = ptrTriggerSource->GetEntryByName("Software");
        if (!IsAvailable(ptrTriggerSourceSoftware) || !IsReadable(ptrTriggerSourceSoftware))
        {
            cout << "Unable to set trigger mode (enum entry retrieval). Aborting..." << endl;
            return;
        }

        ptrTriggerSource->SetIntValue(ptrTriggerSourceSoftware->GetValue());

        cout << "Trigger source set to software..." << endl;
    }
    else if (currentTrigger == HARDWARE)
    {
        // Set trigger mode to hardware ('Line0')
        CEnumEntryPtr ptrTriggerSourceHardware = ptrTriggerSource->GetEntryByName("Line0");
        if (!IsAvailable(ptrTriggerSourceHardware) || !IsReadable(ptrTriggerSourceHardware))
        {
            cout << "Unable to set trigger mode (enum entry retrieval). Aborting..." << endl;
            return;
        }

        ptrTriggerSource->SetIntValue(ptrTriggerSourceHardware->GetValue());

        cout << "Trigger source set to hardware..." << endl;
    }

    //
    // Turn trigger mode on
    //
    // *** LATER ***
    // Once the appropriate trigger source has been set, turn trigger mode
    // on in order to retrieve images using the trigger.
    //

    CEnumEntryPtr ptrTriggerModeOn = ptrTriggerMode->GetEntryByName("On");
    if (!IsAvailable(ptrTriggerModeOn) || !IsReadable(ptrTriggerModeOn))
    {
        cout << "Unable to enable trigger mode (enum entry retrieval). Aborting..." << endl;
        return;
    }

    ptrTriggerMode->SetIntValue(ptrTriggerModeOn->GetValue());

    cout << "Trigger mode turned back on..." << endl << endl;
}

void FlirCamera::disableTrigger() {
    // Ensure trigger mode off
    //
    // *** NOTES ***
    // The trigger must be disabled in order to configure whether the source
    // is software or hardware.
    //
    CEnumerationPtr ptrTriggerMode = nodeMap -> GetNode("TriggerMode");
    if (!IsAvailable(ptrTriggerMode) || !IsReadable(ptrTriggerMode))
    {
        cout << "Unable to disable trigger mode (node retrieval). Aborting..." << endl;
        return;
    }

    CEnumEntryPtr ptrTriggerModeOff = ptrTriggerMode->GetEntryByName("Off");
    if (!IsAvailable(ptrTriggerModeOff) || !IsReadable(ptrTriggerModeOff))
    {
        cout << "Unable to disable trigger mode (enum entry retrieval). Aborting..." << endl;
        return;
    }

    ptrTriggerMode->SetIntValue(ptrTriggerModeOff->GetValue());

    cout << "Trigger mode disabled..." << endl;
}

void FlirCamera::setRate(int rate){
    if (acquireNextImage){
        stopAcquisition();
    }

    this -> rate = rate;
}

void FlirCamera::stopAcquisition() {
    acquireNextImage = false;
}

void FlirCamera::acquireImages() {
    acquireNextImage = true;
    cameraPtr -> BeginAcquisition();
    while (acquireNextImage){


        sleep(rate * 1000);
    }

    cameraPtr -> EndAcquisition();
}

