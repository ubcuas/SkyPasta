#include "FlirCamera.h"
#include <ctime>
#include <ratio>
#include <chrono>

using namespace std;

FlirCamera::FlirCamera()
{
    AcquisitionModeMap[AcquisitionModeEnums::AcquisitionMode_Continuous] = "Continuous";
    AcquisitionModeMap[AcquisitionModeEnums::AcquisitionMode_SingleFrame] = "SingleFrame";
    AcquisitionModeMap[AcquisitionModeEnums::AcquisitionMode_MultiFrame] = "MultiFrame";

    TriggerSourceMap[TriggerSourceEnums::TriggerSource_Software] = "TriggerSource_Software";
    TriggerSourceMap[TriggerSourceEnums::TriggerSource_Line0] = "TriggerSource_Line0";

    TriggerModeMap[TriggerModeEnums::TriggerMode_On] = "TriggerMode_On";
    TriggerModeMap[TriggerModeEnums::TriggerMode_Off] = "TriggerMode_Off";
    
    if(!initialize())
    {
        // Throwing an error to signal that the constructor has failed
        // If an error is not thrown, the destructor won't be called
        throw std::runtime_error("Camera initialization failed");
    }
}

/*
 * Initializes the first camera found.
 * Returns whether or not a camera was found.
 * Parameters:
 * - retryCountMax: total amount of retries to find a camera
 */
bool FlirCamera::initialize(int retryCountMax = 60)
{
    if(system == nullptr)
    {
        system->ReleaseInstance();
        system = nullptr;
    }
    
    system = System::GetInstance();

    for (int count = 1; count <= retryCountMax; count++)
    {
        cameraList = system->GetCameras();
        numberOfCameras = cameraList.GetSize();

        if (numberOfCameras == 0)
        {
            cout << "No Cameras Detected!" << endl;
            if (count >= retryCountMax)
            {     
                cout << "Max retry count has been reached." << endl;           
                cleanExit();
                return false;
            }
            else
            {
                cout << "Sleeping for 1 second before re-trying." << endl;
                sleepWrapper(1000);
                continue;
            }
        }

        cout << "Cameras Connected: " << numberOfCameras << endl;
        cameraPtr = cameraList.GetByIndex(0);
        cameraPtr->Init();
        status = true;
        return true;
    }
}

/*
 * Attemps clean exit by releasing all resources and clearing references to pointers.
 * Clean exit will fail if an object holds a reference to a pointer when called, exit code 11 will be thrown.
 */
void FlirCamera::cleanExit()
{
    cout << "Attempting clean exit..." << endl;
    status = false;
    try
    {
        if (cameraPtr != nullptr)
        {
            cameraPtr->DeInit();
            cameraPtr = nullptr;
        }
        cameraList.Clear();
        system->ReleaseInstance();
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Exception in cleanExit: " << e.what() << endl;
        throw e;
    }

    cout << "Successfully performed a Clean Exit!" << endl;
    cout << "exit code 11 suggests improper ptr usage. Will not be caught by clean exit" << endl;
}

/*
 * Sets the trigger source to the TriggerSourceEnum passed.
 * Trigger Sources are divded into two: software and hardware. We use Software trigger.
 * Trigger Mode to set to Off.
 * Parameters:
 * - triggerSourceToSet: Trigger source that is attempted to be set.
 */
void FlirCamera::setTriggerSource(TriggerSourceEnums triggerSourceToSet)
{
    try
    {
    setTriggerMode(TriggerModeEnums::TriggerMode_Off);

    // Select trigger source
    // The trigger source must be set to hardware or software while trigger mode is off.

    CEnumerationPtr ptrTriggerSource = nodeMap.GetNode("TriggerSource");

    ptrTriggerSource->SetIntValue(triggerSourceToSet);
    cout << "Trigger source set to " << TriggerSourceMap[triggerSourceToSet] << "." << endl;
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error while setting trigger source: " << e.what() << endl;
        throw e;
    }

}

/*
 * Sets the trigger mode to the TriggerModeEnum passed.
 * Trigger Mode is eighter On or Off.
 * Parameters:
 * - triggerModeToSet: Trigger mode that is attempted to be set.
 */
void FlirCamera::setTriggerMode(TriggerModeEnums triggerModeToSet)
{
    try
    {
        INodeMap &nodeMap = cameraPtr->GetNodeMap();
        CEnumerationPtr ptrTriggerMode = nodeMap.GetNode("TriggerMode");
        ptrTriggerMode->SetIntValue(triggerModeToSet);
        cout << "Trigger mode set to " << TriggerModeMap[triggerModeToSet] << "." << endl;
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error while setting trigger mode: " << e.what() << endl;
        throw e;
    }
}

int FlirCamera::getNumCameras() const {
    return numberOfCameras;
}

/*
 * Sets the User Set Selector to the User Set passed.
 * User Set 0 is contains the default settings.
 * Returns operation success.
 * Parameters:
 * - userSet: User Set to select.
 */
bool FlirCamera::selectUserSet(int userSet)
{
    try
    {
        INodeMap &nodeMap = cameraPtr->GetNodeMap();

        // Get User Set 1 from the User Set Selector
        CEnumerationPtr ptrUserSetSelector = nodeMap.GetNode("UserSetSelector");
        if (!IsWritable(ptrUserSetSelector))
        {
            cout << "Unable to set User Set Selector to User Set " + to_string(userSet) + " (node retrieval). Aborting..." << endl;
            return false;
        }

        CEnumEntryPtr ptrUserSet = ptrUserSetSelector->GetEntryByName("UserSet" + to_string(userSet));
        if (!IsReadable(ptrUserSet))
        {
            cout << "Unable to set User Set Selector to User Set " + to_string(userSet) + " (enum entry retrieval). Aborting..." << endl;
            return false;
        }

        const int64_t userSetSelected = ptrUserSet->GetValue();

        // Set User Set Selector to userSet
        ptrUserSetSelector->SetIntValue(userSetSelected);

        // Set User Set Default to userSet
        // This ensures the camera will re-enumerate using userSet, instead of the default user set.
        CEnumerationPtr ptrUserSetDefault = nodeMap.GetNode("UserSetDefault");
        if (!IsWritable(ptrUserSetDefault))
        {
            cout << "Unable to set User Set Default to User Set " + std::to_string(userSet) + " (node retrieval). Aborting..." << endl;
            return false;
        }

        ptrUserSetDefault->SetIntValue(userSet);
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error while loading user set: " << e.what() << endl;
        throw e;
    }
    return true;
}

/*
 * Saves the camera settings to the User Set passed.
 * User Set 1, 2 are writable.
 * Also sets User Set Selector to the desired User Set
 * Returns operation success.
 * Parameters:
 * - userSet: User Set to save the settings to.
 */
bool FlirCamera::saveUserSet(int userSet)
{
    //Select the correct User Set
    if(!selectUserSet(userSet))
    {
        return false;
    }

    try
    {
        // Execute User Set Save
        INodeMap &nodeMap = cameraPtr->GetNodeMap();
        CCommandPtr ptrUserSetSave = nodeMap.GetNode("UserSetSave");
        if (!ptrUserSetSave.IsValid())
        {
            cout << "UserSetSave is not valid. Aborting..." << endl;
            return false;
        }
        ptrUserSetSave->Execute();
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error while saving user set: " << e.what() << endl;
        throw e;
    }

    return true;
}

/*
 * Loads the camera settings from the User Set passed.
 * User Set 0, 1, 2 are readable.
 * Also sets User Set Selector to the desired User Set
 * Returns operation success.
 * Parameters:
 * - userSet: User Set to load the settings from.
 */
bool FlirCamera::loadUserSet(int userSet)
{
    //Select the correct User Set
    if(!selectUserSet(userSet))
    {
        return false;
    }
    try
    {
        // Execute User Set Load
        INodeMap &nodeMap = cameraPtr->GetNodeMap();
        CCommandPtr ptrUserSetLoad = nodeMap.GetNode("UserSetLoad");
        if (!ptrUserSetLoad.IsValid())
        {
            cout << "UserSetLoad is not valid. Aborting..." << endl;
            return false;
        }
        ptrUserSetLoad->Execute();
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error while loading user set: " << e.what() << endl;
        throw e;
    }

    return true;
}

/*
 * Calls Device Reset. Device Reset resets the camera, meaning, the camera is dropped from the system
 * and the settings are reset to the settings saved in user set specified UserSetDefault. User Sets are not reset.
 * Parameters:
 * - resetSettings: If set to true, User Set 1 is overwritten by User Set 0.
 */
void FlirCamera::resetCamera(bool resetSettings = false)
{
    try
    {
        if(resetSettings)
        {
            loadUserSet(0);
        }
        saveUserSet(1);

        INodeMap &nodeMap = cameraPtr->GetNodeMap();
        CCommandPtr ptrDeviceReset = nodeMap.GetNode("DeviceReset");
        ptrDeviceReset->execute();
        cleanExit();
        if(!initialize())
        {
            throw std::runtime_error("Couldn't find the camera after calling Device Reset...");;
        }
        loadUserSet(1);
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error while reseting the camera: " << e.what() << endl;
        throw e;
    }
}

// Sleep helper function (in milliseconds)
void FlirCamera::sleepWrapper(int milliseconds)
{
#if defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64
    Sleep(milliseconds);
#else
    usleep(1000 * milliseconds);
#endif
}

/*
 * Sets the acquisition mode to the AcquisitionModeEnum passed.
 * Acquistion Mode is Continuous or SingleFrame or MultiFrame.
 * In Continuous mode, the camera takes pictures automatically as fast as possible like a video camera.
 * In SingleFrame mode, the camera takes a picture whenever it receives a trigger.
 * In MultiFrame mode, the camera takes set amount of pictures whenever it receives a trigger.
 * Parameters:
 * - selectedMode: Acquistion mode that is attempted to be set.
 */
void FlirCamera::setAcquisitionMode(AcquisitionModeEnums selectedMode)
{
    try
    {   
        // Get AcquisitionMode node
        CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
        ptrAcquisitionMode->SetIntValue(selectedMode);
        cout << "Acquisition mode set to " << AcquisitionModeMap[selectedMode] << "..." << endl;
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error while setting acquisition mode: " << e.what() << endl;
        throw e;
    }

    // If we would like to change the buffer direction (look at node StreamBufferHandlingMode [Newest Only by deafult]),
    // this would be a good place to do it.
}

/*
 * Begins acquisitions on the camera.
 * Also calcualtes the image timestamp timeoffset.
 */
void FlirCamera::startCapture()
{
    try
    {
        if (!cameraPtr->IsStreaming())
        {
            cameraPtr->BeginAcquisition();
        }
        else
        {
            cout << "FlirCamera::startCapture() is called but the camera is already streaming. Ignoring call..." << endl;
            return;
        }
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error while starting acquisition: " << e.what() << endl;
        throw e;
    }

    try
    {
        // This set of executations give the most accurate time offset we have between the when we think the
        // camera took an image and when the camera actually took an image.
        // For the most accurate timing we would use clock synchronization but we do not need that much precision.
        using namespace std::chrono;
        cameraPtr->TimestampLatch.Execute();
        milliseconds triggerTime_ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());
        
        //TimestampLatchValue return in ticks and TimestampIncrement in nanosecond per tick
        int timestampLatch_ms = cameraPtr->TimestampLatchValue.GetValue() * cameraPtr->TimestampIncrement.GetValue() / 1000; 
        cameraTimestampEpochOffset_ms = triggerTime_ms - timestampLatch_ms;
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error while getting a refernce time for images: " << e.what() << endl;
        throw e;
    }
}

/*
 * Ends acquisitions on the camera.
 */
void FlirCamera::stopCapture()
{
    try
    {
        if (cameraPtr->IsStreaming())
        {
            cameraPtr->EndAcquisition();
        }
        else
        {
            cout << "Camera already is already not streaming. Ignoring command." << endl;
        }
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error while stopping acquisition: " << e.what() << endl;
        throw e;
    }
}

/*
 * Grabs an image and calculates its timestamp.
 * Returns operation success.
 * Parameters:
 * - imagePtr: Image pointer to be filled by the function
 * - timestamp: Image timestamp in epoch to be filled by the function
 */
bool FlirCamera::getImage(ImagePtr *imagePtr, int *timestamp)
{
    try
    {
    //Before getting the image, we have to check if we need to trigger the camera.
    CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
    CEnumEntryPtr acquisitionMode = ptrAcquisitionMode->GetCurrentEntry();

    if(acquisitionMode == AcquisitionModeEnums::AcquisitionMode_SingleFrame)
    {
        cameraPtr->TriggerSoftware.Execute();
    }

    ImagePtr imagePointer = cameraPtr->GetNextImage();

    if (imagePointer->IsIncomplete())
    {
        cout << "Image incomplete with image status: " << imagePointer->GetImageStatus() << endl;
        imagePointer->Release();
        return false;
    }

    // Grabbing the image timestamp and converting it to epoch
    int imageTime_ms =  imagePointer->GetTimeStamp() * 1000 + cameraTimestampEpochOffset_ms;

    *imagePtr = imagePointer;
    *timestamp = imageTime_ms;
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error while trying to grab an image: " << e.what() << endl;
        throw e;
    }

    return true;
}

/*
 * Grabs an image and calculates its timestamp.
 * Returns operation success.
 * Parameters:
 * - imagePtr: Image pointer to be filled by the function
 * - timestamp: Image timestamp in epoch to be filled by the function
 */
void FlirCamera::setDefaultSettings(AcquisitionModeEnums acqMode = AcquisitionMode_SingleFrame,
    TriggerSourceEnums trigSrc = TriggerSource_Software,
    TriggerModeEnums trigMode = TransferTriggerMode_On)
{
    setAcquisitionMode(acqMode);
    setTriggerSource(trigSrc);
    setTriggerMode(trigMode);

    saveUserSet(1); 
}