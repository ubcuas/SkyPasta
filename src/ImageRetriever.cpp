#include "ImageRetriever.h"

using namespace std;

// Initializes and ensures images can be saved by saving a test file and deleting it
// Creates image directory if necessary
ImageRetriever::ImageRetriever(ImageTag *imageTag, CameraType cameraType, FlirCamera *flirCamera)
{
    this->flirCamera = flirCamera;
    this->imageTag = imageTag;

    mkdir("../Images", 0);
    FILE *tempFile = fopen("../Images/test.txt", "w+");
    if (tempFile == nullptr)
    {
        cout << "Failed to create file in current folder.  Please check "
                "permissions."
             << endl;
    }
    fclose(tempFile);
    remove("../Images/test.txt");

    if(cameraType == CameraType::FLIR)
    {
        if (flirCamera == nullptr)
        {
            cout << "No Camera devices attached to Image Retriever " << endl;
            return;
        }
        if (!flirCamera->getCamera()->IsInitialized())
        {
            cout << "Camera not initialized" << endl;
            return;
        }
    }
}

// Sets AcquisitionMode
void ImageRetriever::setAcquisitionMode(AcquisitionModeEnums acquisitionModeToSet)
{
    if(!waitForCameraAvailability(__FUNCTION__))
    {
        throw std::runtime_error("Camera busy timeout");
    }
    isCameraBusy = true;

    if(cameraType == CameraType::FLIR)
    {
        try
        {
            flirCamera->setAcquisitionMode(acquisitionModeToSet);
        }
        catch (Spinnaker::Exception& e)
        {
            isCameraBusy = false;
            throw e;
        }
    }
    isCameraBusy = false;
}

// Sets TriggerSource
void ImageRetriever::setTriggerSource(TriggerSourceEnums triggerSourceToSet)
{
    if(!waitForCameraAvailability(__FUNCTION__))
    {
        throw std::runtime_error("Camera busy timeout");
    }
    isCameraBusy = true;

    if(cameraType == CameraType::FLIR)
    {
        try
        {
            flirCamera->setTriggerSource(triggerSourceToSet);
        }
        catch (Spinnaker::Exception& e)
        {
            isCameraBusy = false;
            throw e;
        }
    }
    isCameraBusy = false;
}

// SetsTriggerMode
void ImageRetriever::setTriggerMode(TriggerModeEnums triggerModeToSet)
{
    if(!waitForCameraAvailability(__FUNCTION__))
    {
        throw std::runtime_error("Camera busy timeout");
    }
    isCameraBusy = true;

    if(cameraType == CameraType::FLIR)
    {
        try
        {
            flirCamera->setTriggerMode(triggerModeToSet);
        }
        catch (Spinnaker::Exception& e)
        {
            isCameraBusy = false;
            throw e;
        }
    }
    isCameraBusy = false;
}

// Reset image and time tracking and start acquisition
void ImageRetriever::startAcquisition()
{

    //INodeMap &nodeMap = cameraPtr->GetNodeMap();
    if(!waitForCameraAvailability(__FUNCTION__))
    {
        throw std::runtime_error("Camera busy timeout");
    }

    isCameraBusy = true;
    imageNumber = 0;
    totalTime = 0;

    cout << "Begin Acquisition..." << endl;

    if(cameraType == CameraType::FLIR)
    {
        try
        {
            flirCamera->startCapture();
        }
        catch (Spinnaker::Exception& e)
        {
            isCameraBusy = false;
            throw e;
        }
    }

    isCameraBusy = false;
}

// Stops acquisition of the camera
void ImageRetriever::stopAcquisition() {
    if(!waitForCameraAvailability(__FUNCTION__))
    {
        throw std::runtime_error("Camera busy timeout");
    }
    isCameraBusy = true;

    cout << "Stopping..." << endl;
    if(cameraType == CameraType::FLIR)
    {
        flirCamera->stopCapture();
    }
    isCameraBusy = false;

    cout << "Acquisition Stopped" << endl << endl;
    cout << "Number of images Acquired: " << imageNumber << endl;
}

// Acquires a single image and saves it to disk
void ImageRetriever::getImage(string &imageName, long * timestamp)
{
    if(!waitForCameraAvailability(__FUNCTION__))
    {
        throw std::runtime_error("Camera busy timeout");
    }
    isCameraBusy = true;

    if(cameraType == CameraType::FLIR)
    {
        ImagePtr imagePtr;
        int image_timestamp_epoch;
        try
        {
            flirCamera->getImage(&imagePtr, &image_timestamp_epoch);
        }
        catch (Spinnaker::Exception& e)
        {
            isCameraBusy = false;
            cout << "Exception while getting image: " << e.what() << endl;
            throw e;
        }
        ImagePtr convertedImage = imagePtr->Convert(PixelFormat_BayerRG8, HQ_LINEAR);

        // SKYPASTA ISSUE 12
        ostringstream filename;
        filename << "../Images/Trigger-" << imageNumber++ << ".jpg";
        convertedImage->Save(filename.str().c_str());
        cout << "Image saved at " << filename.str() << endl;
        imagePtr->Release();

        imageName = filename.str();
        *timestamp = image_timestamp_epoch;
    }

    isCameraBusy = false;
}

// Grabs an image, adds image location and timestamp_telem to ImageTag
void ImageRetriever::acquireImage() {
    string image = "";
    long * timestamp;
    getImage(image, timestamp);

    imageTag->addImage(image, *timestamp);
}

// Waits for 20 * 100 milliseconds = 2,000 milliseconds = 2 seconds
bool ImageRetriever::waitForCameraAvailability(const char* func)
{
    int count = 0;
    while (isCameraBusy)
    {
        if(count == 20)
        {
            cout << func << " was called but the camera was busy. Aborting..." << endl;
            return false;
        }
        cout << func << " is called but the camera is busy. Waiting..." << endl;
        count++;
        sleepWrapper(100);
    }
    return true;
}

// In milliseconds
void ImageRetriever::sleepWrapper(int milliseconds)
{
#if defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64
    Sleep(milliseconds);
#else
    usleep(1000 * milliseconds);
#endif
}

// Clean exit
void ImageRetriever::releaseCamera()
{
    if(!waitForCameraAvailability(__FUNCTION__))
    {
        throw std::runtime_error("Camera busy timeout");
    }
    isCameraBusy = true;
    
    if(cameraType == CameraType::FLIR)
    {
        flirCamera->cleanExit();   
    }
    isCameraBusy = false;
}