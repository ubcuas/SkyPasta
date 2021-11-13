#include "FlirCamera.h"
#include <ctime>
#include <ratio>
#include <chrono>
#include <thread>

using namespace std;



FlirCamera::FlirCamera()
{   
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
bool FlirCamera::initialize(int retryCountMax)
{
    try
    {
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
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    continue;
                }
            }

            cout << "Cameras Connected: " << numberOfCameras << endl;
            cameraPtr = cameraList.GetByIndex(0);
            cameraPtr->Init();
            status = true;

            findEpochOffset();
            return true;
        }
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Exception in initialize: " << e.what() << endl;
        throw e;
    }

    return false;
}

/*
 * Calcualtes the timestamp difference between the computer and the camera
 * Returns void but modifies the cameraTimestampEpochOffset_ms variable
 */
void FlirCamera::findEpochOffset()
{
    try
    {
        // This set of executations give the most accurate time offset we have between the when we think the
        // camera took an image and when the camera actually took an image.
        // For the most accurate timing we would use clock synchronization but we do not need that much precision.

        INodeMap &nodeMap = cameraPtr->GetNodeMap();
        CIntegerPtr ptrTimestampLatchValue = nodeMap.GetNode("TimestampLatchValue");

        using namespace std::chrono;
        cameraPtr->TimestampLatch.Execute();
        milliseconds triggerTime_ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());

        int64_t timestampLatchValue = ptrTimestampLatchValue->GetValue();

        cameraTimestampEpochOffset_ms = triggerTime_ms.count() - (timestampLatchValue / 1000 / 1000);    
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error while getting a reference time for images: " << e.what() << endl;
        throw e;
    }
}

/*
 * Attemps clean exit by releasing all resources and clearing references to pointers.
 * Clean exit will fail if an object holds a reference to a pointer when called, exit code 11 will be thrown.
 */
int FlirCamera::cleanExit()
{
    cout << "Attempting clean exit..." << endl;
    status = false;
    try
    {
        if (cameraPtr != NULL_CAMERA)
        {
            stopCapture();
            cameraPtr->DeInit();
            cameraPtr = NULL_CAMERA;
        }
        
        cameraList.Clear();
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Exception in cleanExit: " << e.what() << endl;
        throw e;
    }

    // Exit code 11 suggests improper ptr usage. Will not be caught by clean exit"
    cout << "Successfully performed a Clean Exit!" << endl;
    return 0;
}

/*
 * Sets the acquisition mode to the string passed that represents AcquisitionModeEnum.
 * Acquistion Mode is Continuous, SingleFrame or MultiFrame.
 * In Continuous mode, the camera takes pictures whenever it receives a trigger.
 * In SingleFrame mode, the camera takes a single picture when it receives a trigger.
 * In MultiFrame mode, the camera takes set amount of pictures whenever it receives a trigger.
 * Parameters:
 * - selectedMode: Acquistion mode that is attempted to be set.
 */
void FlirCamera::setAcquisitionMode(string selectedMode)
{
    try
    {   
        // Get AcquisitionMode node
        INodeMap &nodeMap = cameraPtr->GetNodeMap();
        CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");

        CEnumEntryPtr ptrAcquisitionModeEnum = ptrAcquisitionMode->GetEntryByName(selectedMode.c_str());
        if (!IsReadable(ptrAcquisitionModeEnum))
        {
            cout << "Unable to set Acquisition Mode to " + selectedMode + " (enum entry retrieval). Aborting..." << endl;
            return;
        }

        int64_t acquisitionModeSelected = ptrAcquisitionModeEnum->GetValue();
        ptrAcquisitionMode->SetIntValue(acquisitionModeSelected);
        cout << "Acquisition mode set to " << selectedMode << "..." << endl;
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error while setting acquisition mode: " << e.what() << endl;
        throw e;
    }
}

void FlirCamera::setTriggerType(string triggerTypeToSet)
{
    try
    {
        setTriggerMode("Off");

        // Select trigger type
        // The trigger source must be set to hardware or software while trigger mode is off.
        INodeMap &nodeMap = cameraPtr->GetNodeMap();
        CEnumerationPtr ptrTriggerType = nodeMap.GetNode("TriggerSelector");

        CEnumEntryPtr ptrTriggerTypeEnum = ptrTriggerType->GetEntryByName(triggerTypeToSet.c_str());
        if (!IsReadable(ptrTriggerTypeEnum))
        {
            cout << "Unable to set Trigger Source to " + triggerTypeToSet + " (enum entry retrieval). Aborting..." << endl;
            return;
        }
        int64_t triggerTypeSelected = ptrTriggerTypeEnum->GetValue();
        ptrTriggerType->SetIntValue(triggerTypeSelected);
        cout << "Trigger type set to " << triggerTypeToSet << "." << endl;
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error while setting trigger source: " << e.what() << endl;
        throw e;
    }
}

/*
 * Sets the trigger source to the string passed that represents TriggerSourceEnum.
 * Trigger Sources are divded into two: software and hardware. We use Software trigger.
 * Trigger Mode to set to Off.
 * Parameters:
 * - triggerSourceToSet: Trigger source that is attempted to be set.
 */
void FlirCamera::setTriggerSource(string triggerSourceToSet)
{
    try
    {
        setTriggerMode("Off");

        // Select trigger source
        // The trigger source must be set to hardware or software while trigger mode is off.
        INodeMap &nodeMap = cameraPtr->GetNodeMap();
        CEnumerationPtr ptrTriggerSource = nodeMap.GetNode("TriggerSource");

        CEnumEntryPtr ptrTriggerSourceEnum = ptrTriggerSource->GetEntryByName(triggerSourceToSet.c_str());
        if (!IsReadable(ptrTriggerSourceEnum))
        {
            cout << "Unable to set Trigger Source to " + triggerSourceToSet + " (enum entry retrieval). Aborting..." << endl;
            return;
        }

        int64_t triggerSourceSelected = ptrTriggerSourceEnum->GetValue();
        ptrTriggerSource->SetIntValue(triggerSourceSelected);
        cout << "Trigger source set to " << triggerSourceToSet << "." << endl;
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error while setting trigger source: " << e.what() << endl;
        throw e;
    }
}

/*
 * Sets the trigger mode to the string passed that represents TriggerModeEnum.
 * Trigger Mode is eighter On or Off.
 * Parameters:
 * - triggerModeToSet: Trigger mode that is attempted to be set.
 */
void FlirCamera::setTriggerMode(string triggerModeToSet)
{
    try
    {
        INodeMap &nodeMap = cameraPtr->GetNodeMap();
        CEnumerationPtr ptrTriggerMode = nodeMap.GetNode("TriggerMode");

        CEnumEntryPtr ptrTriggerModeEnum = ptrTriggerMode->GetEntryByName(triggerModeToSet.c_str());
        if (!IsReadable(ptrTriggerModeEnum))
        {
            cout << "Unable to set Trigger Source to " + triggerModeToSet + " (enum entry retrieval). Aborting..." << endl;
            return;
        }

        int64_t triggerSourceSelected = ptrTriggerModeEnum->GetValue();
        ptrTriggerMode->SetIntValue(triggerSourceSelected);
        cout << "Trigger mode set to " << triggerModeToSet << "." << endl;
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error while setting trigger mode: " << e.what() << endl;
        throw e;
    }
}

/*
 * Sets the pixel format to the string passed that represents TriggerModeEnum.
 * Pixel format determines how the image is processed and saved.
 * Parameters:
 * - pixelFormatToSet: Pixel Format that is attempted to be set.
 */
void FlirCamera::setPixelFormat(string pixelFormatToSet)
{
    try
    {
        INodeMap &nodeMap = cameraPtr->GetNodeMap();
        CEnumerationPtr ptrPixelFormat = nodeMap.GetNode("PixelFormat");

        CEnumEntryPtr ptrPixelFormatEnum = ptrPixelFormat->GetEntryByName(pixelFormatToSet.c_str());
        if (!IsReadable(ptrPixelFormatEnum))
        {
            cout << "Unable to set Pixel Format to " + pixelFormatToSet + " (enum entry retrieval). Aborting..." << endl;
            return;
        }

        int64_t pixelFormatSelected = ptrPixelFormatEnum->GetValue();
        ptrPixelFormat->SetIntValue(pixelFormatSelected);
        cout << "Pixel Format set to " << pixelFormatToSet << "." << endl;
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error while setting Pixel Format: " << e.what() << endl;
        throw e;
    }
}

/*
 * Sets the exposure time to the float passed.
 * Exposure time determines the brightness of the image.
 * If -1.0 is passed, the auto exposure is used, else
 * sets Auto Exposure to Off.
 * Parameters:
 * - exposureTimeToSet: Exposure Time that is attempted to be set.
 */
void FlirCamera::setExposureTime(float exposureTimeToSet)
{
    try
    {
        string exposureTimeAutoString;
        if (exposureTimeToSet == -1.0) // If the exposureTimeToSet is -1.0, use the camera's auto exposure.
        {
            exposureTimeAutoString = "Continuous";
        }
        else
        {
            exposureTimeAutoString = "Off";
        }

        INodeMap &nodeMap = cameraPtr->GetNodeMap();
        CEnumerationPtr ptrExposureAuto = nodeMap.GetNode("ExposureAuto");

        CEnumEntryPtr ptrExposureAutoEnum = ptrExposureAuto->GetEntryByName(exposureTimeAutoString.c_str());
        if (!IsReadable(ptrExposureAutoEnum))
        {
            cout << "Unable to set Exposure Auto to " + exposureTimeAutoString + " (enum entry retrieval). Aborting..." << endl;
            return;
        }

        int64_t exposureAutoSelected = ptrExposureAutoEnum->GetValue();
        ptrExposureAuto->SetIntValue(exposureAutoSelected);
        cout << "Exposure Auto set to " + exposureTimeAutoString + "." << endl;

        if(exposureTimeAutoString != "Off")
        {
            return;
        }

        CFloatPtr ptrExposureTime = nodeMap.GetNode("ExposureTime");
        ptrExposureTime->SetValue(exposureTimeToSet);
        cout <<  "Exposure Time set to " << ptrExposureTime->GetValue() << "." << endl;
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error while setting exposure time: " << e.what() << endl;
        throw e;
    }
}

/*
 * Public facing get function for numberOfCameras.
 */
int FlirCamera::getNumCameras() const
{
    return numberOfCameras;
}

/*
 * Sets the User Set Selector to the User Set passed.
 * User Set 0 is contains the default settings.
 * Returns operation success.
 * Parameters:
 * - userSet: User Set to select.
 */
bool FlirCamera::selectUserSet(string userSet)
{
    try
    {
        INodeMap &nodeMap = cameraPtr->GetNodeMap();

        CEnumerationPtr ptrUserSetSelector = nodeMap.GetNode("UserSetSelector");
        if (!IsWritable(ptrUserSetSelector))
        {
            cout << "Unable to set User Set Selector to " + userSet + " (node retrieval). Aborting..." << endl;
            return false;
        }

        // Get User Set from the User Set Selector
        CEnumEntryPtr ptrUserSetSelectorEnum = ptrUserSetSelector->GetEntryByName(userSet.c_str());
        if (!IsReadable(ptrUserSetSelectorEnum))
        {
            cout << "Unable to set User Set Selector to " + userSet + " (enum entry retrieval). Aborting..." << endl;
            return false;
        }

        int64_t userSetSelected = ptrUserSetSelectorEnum->GetValue();
        ptrUserSetSelector->SetIntValue(userSetSelected);

        // Set User Set Default to userSet
        // This ensures the camera will re-enumerate using userSet, instead of the default user set.
        CEnumerationPtr ptrUserSetDefault = nodeMap.GetNode("UserSetDefault");
        if (!IsWritable(ptrUserSetDefault))
        {
            cout << "Unable to set User Set Default to " + userSet + " (node retrieval). Aborting..." << endl;
            return false;
        }

        CEnumEntryPtr ptrUserSetDefaultEnum = ptrUserSetDefault->GetEntryByName(userSet.c_str());
        if (!IsReadable(ptrUserSetDefaultEnum))
        {
            cout << "Unable to set User Set Default to " + userSet + " (enum entry retrieval). Aborting..." << endl;
            return false;
        }

        userSetSelected = ptrUserSetDefaultEnum->GetValue();
        ptrUserSetDefault->SetIntValue(userSetSelected);
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error while selecting user set: " << e.what() << endl;
        throw e;
    }
    return true;
}

/*
 * Saves the camera settings to the User Set passed.
 * User Set 0, 1 are writable.
 * Also sets User Set Selector to the desired User Set
 * Returns operation success.
 * Parameters:
 * - userSet: User Set to save the settings to.
 */
bool FlirCamera::saveUserSet(string userSet)
{
    //Select the correct User Set
    if(!selectUserSet(userSet))
    {
        return false;
    }

    // We have to sleep here while the camera gets the save and load nodes ready
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    try
    {
        // Execute User Set Save
        INodeMap &nodeMap = cameraPtr->GetNodeMap();
        CCommandPtr ptrUserSetSave = nodeMap.GetNode("UserSetSave");
        if (!IsWritable(ptrUserSetSave))
        {
            cout << "UserSetSave is not writable. Aborting..." << endl;
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
 * User Set Default, 0, 1 are readable.
 * Also sets User Set Selector to the desired User Set
 * Returns operation success.
 * Parameters:
 * - userSet: User Set to load the settings from.
 */
bool FlirCamera::loadUserSet(string userSet)
{
    //Select the correct User Set
    if(!selectUserSet(userSet))
    {
        return false;
    }

    // We have to sleep here while the camera gets the save and load nodes ready
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

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
 * - resetSettings: If set to true, User Set 0 is overwritten by User Set Default.
 */
void FlirCamera::resetCamera(bool resetSettings)
{
    try
    {
        if(resetSettings)
        {
            loadUserSet("Default");
        }
        saveUserSet("UserSet0");

        INodeMap &nodeMap = cameraPtr->GetNodeMap();
        CCommandPtr ptrDeviceReset = nodeMap.GetNode("DeviceReset");
        ptrDeviceReset->Execute();
        cleanExit();
        if(!initialize())
        {
            throw std::runtime_error("Couldn't find the camera after calling Device Reset...");;
        }
        loadUserSet("UserSet0");
    }
    catch (Spinnaker::Exception& e)
    {
        cout << "Error while reseting the camera: " << e.what() << endl;
        throw e;
    }
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
bool FlirCamera::getImage(ImagePtr *imagePtr, long *timestamp)
{
    try
    {
        //Before getting the image, we have to check if we need to trigger the camera.
        INodeMap &nodeMap = cameraPtr->GetNodeMap();
        CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
        CEnumEntryPtr acquisitionMode = ptrAcquisitionMode->GetCurrentEntry();

        if(workingMode == Modes::triggerMode)
        {
            cameraPtr->TriggerSoftware.Execute();
        }

        ImagePtr imagePointer = cameraPtr->GetNextImage(100);

        if (imagePointer->IsIncomplete())
        {
            cout << "Image incomplete with image status: " << imagePointer->GetImageStatus() << endl;
            imagePointer->Release();
            return false;
        }

        int64_t imageTime_ms = (double)imagePointer->GetTimeStamp() / 1000 / 1000  + cameraTimestampEpochOffset_ms; 

        *imagePtr = imagePointer;
        *timestamp = imageTime_ms;
    }
    catch (Spinnaker::Exception& e)
    {
        if (workingMode == Modes::continuousMode && strcmp(e.GetErrorMessage(), "Failed waiting for EventData on NEW_BUFFER_DATA event.") == 0) 
        {
            return false;
        }
        else
        {
            cout << "Error while trying to grab an image: " << e.what() << endl;
            throw e;  
        }
        
    }

    return true;
}

/*
 * Sets a lot of settings on the camera. If a parameter is not given, it sets the settings to predetermined values.
 * Parameters:
 * - acqMode: Acquistion Mode to be set
 * - trigType: Trigger Type to be set
 * - trigSrc: Trigger Source to be set
 * - trigMode: Trigger Mode to be set
 * - pixFormat: Pixel Format to be set
 * - expoTime: Exposure time to be set
 */
void FlirCamera::setDefaultSettings(string acqMode,
                                    string trigType,
                                    string trigSrc,
                                    string trigMode,
                                    string pixFormat,
                                    float expoTime)
{
    setAcquisitionMode(acqMode);
    setTriggerType(trigType);
    setTriggerSource(trigSrc);
    setTriggerMode(trigMode);
    setPixelFormat(pixFormat);  
    setExposureTime(expoTime);

    saveUserSet("UserSet0");
}