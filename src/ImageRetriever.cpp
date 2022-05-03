#include "ImageRetriever.h"
#include <thread>
#include <experimental/filesystem>


using namespace std;

/* Initializes and ensures images can be saved by saving a test file and deleting it
 * Creates image directory if necessary
 * Parameters:
 * - imageTag: ImageTag object that will tag images with geotagging.
 */
ImageRetriever::ImageRetriever(ImageTag *imageTag, FlirCamera *flirCamera, string imageFilePath)
{
    this->flirCamera = flirCamera;
    this->imageTag = imageTag;
    this->cameraType = CameraType::FLIR;
    this->imageFilePath = imageFilePath;

    fileSetup();

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

// Overlords the constructor
ImageRetriever::ImageRetriever(ImageTag *imageTag, GenericUSBCamera *genericUSBCamera, string imageFilePath)
{
    this->genericUSBCamera = genericUSBCamera;
    this->imageTag = imageTag;
    this->cameraType = CameraType::GenericUSB;
    this->imageFilePath = imageFilePath;

    fileSetup();

    if (genericUSBCamera == nullptr)
    {
        cout << "No Camera devices attached to Image Retriever " << endl;
        return;
    }
}

/*
 * Creates and tests a folder on disk to save images to.
 */
void ImageRetriever::fileSetup()
{
    mkdir(imageFilePath.c_str(), 0777); //Creating a directory with all the permissions

    // Test the permissions
    FILE *tempFile = fopen((imageFilePath + "/test.txt").c_str(), "w+");
    if (tempFile == nullptr)
    {
        cout << "Failed to create file in current folder. Please check permissions." << endl;
    }
    fclose(tempFile);
    remove((imageFilePath + "/test.txt").c_str());

    // Create subdirectories
    mkdir((imageFilePath + "/Saving").c_str(), 0777); //Creating the folder where the images will be saved initally
    mkdir((imageFilePath + "/Untagged").c_str(), 0777); //Creating the folder where the images will be moved to when they are finished being saved
    mkdir((imageFilePath + "/Tagged").c_str(), 0777); //Creating the folder where the images will be moved to when they are tagged
}

// Sets AcquisitionMode. See FlirCamera::setTriggerType.
void ImageRetriever::setAcquisitionMode(string acquisitionModeToSet)
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

// Sets TriggerType. See FlirCamera::setTriggerType.
void ImageRetriever::setTriggerType(string triggerTypeToSet)
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
            flirCamera->setTriggerType(triggerTypeToSet);
        }
        catch (Spinnaker::Exception& e)
        {
            isCameraBusy = false;
            throw e;
        }
    }
    isCameraBusy = false;
}

// Sets TriggerSource. See FlirCamera::setTriggerSource.
void ImageRetriever::setTriggerSource(string triggerSourceToSet)
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

// Sets TriggerMode. See FlirCamera::setTriggerMode.
void ImageRetriever::setTriggerMode(string triggerModeToSet)
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

/*
 * Resets image and time tracking and starts a new acquisition
 */
void ImageRetriever::startAcquisition()
{
    acquisitionStartTime = std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch()).count();

    cout << "Acquisition start time is " << acquisitionStartTime << endl;

    if(!waitForCameraAvailability(__FUNCTION__))
    {
        throw std::runtime_error("Camera busy timeout");
    }

    isCameraBusy = true;
    imageNumber = 0;

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

        isCameraBusy = false;
    }
    else if (cameraType == CameraType::GenericUSB)
    {
        if(!genericUSBCamera->openCamera())
        {
            isCameraBusy = false;
            throw std::runtime_error("Can not open camera...");
        }
        isCameraBusy = false;
    }
}

/*
 * Stops acquisition of the camera
 */
void ImageRetriever::stopAcquisition()
{
    if(!waitForCameraAvailability(__FUNCTION__))
    {
        throw std::runtime_error("Camera busy timeout");
    }
    isCameraBusy = true;

        cout << "Stopping..." << endl;
    if(cameraType == CameraType::FLIR)
    {
        flirCamera->stopCapture();
        isCameraBusy = false;

        cout << "Acquisition Stopped" << endl << endl;
        cout << "Number of images Acquired: " << imageNumber << endl;
    }
    else if (cameraType == CameraType::GenericUSB)
    {
        genericUSBCamera->closeCamera();
    }
}

/*
 * Acquires a single image and saves it to disk
 * Parameters:
 * - imagePath: Relative file path to the image, including the file name. To be modified by the function.
 * - timestamp: Timestamp of the image, in epcoh of the computer clock. To be modified by the function.
 * - getTimestamp: Whether or not the camera supports timestamps. True for CameraType::FLIR, false for CameraType::GenericUSB.
 */
void ImageRetriever::getImage(string &imagePath, long * timestamp, bool getTimestamp)
{
    if(cameraType == CameraType::FLIR)
    {
        ImagePtr imagePtr;
        try
        {
            if (!flirCamera->getImage(&imagePtr, timestamp))
            {
                imagePath = "";
                return;
            }
        }
        catch (Spinnaker::Exception& e)
        {
            cout << "Exception while getting image: " << e.what() << endl;
            throw e;
        }

        ImagePtr convertedImage;
        try
        {
            convertedImage = imagePtr->Convert(imagePtr->GetPixelFormat(), ColorProcessingAlgorithm::RIGOROUS);
        }
        catch (Spinnaker::Exception& e)
        {
            cout << "Error while converting image: " << e.what() << endl;
            imagePtr->Release();
            throw e;
        }

        imageNumber++;

        // Save image to the temporary saving location
        ostringstream filePath;
        filePath << imageFilePath + "/" << acquisitionStartTime << "-" << imageNumber << ".jpg";
        convertedImage->Save(filePath.str().c_str());
        cout << "Image saved at " << filePath.str() << endl;
        imagePtr->Release();

        // Move images when the save operation is complete
        ostringstream newFilePath;
        newFilePath << imageFilePath + "/Untagged/" << acquisitionStartTime << "-" << imageNumber << ".jpg";
        try
        {
            error_code ec;
            experimental::filesystem::rename(filePath, newFilePath, ec);
            experimental::filesystem::rename();
            
        }
        catch (experimental::filesystem::filesystem_error& e)
        {
            cout << e.what() << '\n';
            imagePath = "";
            return;
        }
        cout << "Image moved to " << newFilePath.str() << endl;

        imagePath = newFilePath.str();
    }
    else if (cameraType == CameraType::GenericUSB)
    {
        if (getTimestamp)
        {
            cout << "Asked for a timestamp but Generic USB Camera does not support it. Aboring...";
            std::runtime_error("Requested timesamp for Generic USB Camera. Functionality not yet supported.");
        }

        cv::Mat image;
        if (!genericUSBCamera->getImage(&image))
        {
            imagePath = "";
            return;
        }

        ostringstream filePath;
        filePath << imageFilePath << "/" << acquisitionStartTime << "-" << imageNumber++ << ".jpg";
        imwrite(filePath.str(), image);

        imagePath = filePath.str();
    }
}

/*
 * Gets images from the camera and tags them with geolocation.
 * Parameters:
 * - tagImages: Boolean to determine if the acquired images should have timestamps and geolocation.
 */
void ImageRetriever::acquireImage(bool tagImages)
{
    string image = "";
    long timestamp;
    getImage(image, &timestamp, tagImages);

    if (image == "")
    {
        return;
    }

    if (tagImages)
    {
        imageTag->addImage(image, timestamp);
    }
}

/*
 * Tasked with making sure that a command send to a camera is resolved before sending another one.
 * Waits for 20 * 100 milliseconds = 2,000 milliseconds = 2 seconds
 * Should be replaced with a mutex...
 * Parameters:
 * - func: The function that calls this function. used for debugging.
 */
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
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return true;
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
    else if (cameraType == CameraType::GenericUSB)
    {
        genericUSBCamera->closeCamera();
    }
    isCameraBusy = false;
}