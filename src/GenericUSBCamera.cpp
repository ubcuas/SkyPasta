#include "GenericUSBCamera.h"
#include <iostream>
#include <stdio.h>
#include <chrono>

using namespace cv;
using namespace std;
using namespace std::chrono;

GenericUSBCamera::GenericUSBCamera()
{   
    videoCapture = VideoCapture(0, cv::CAP_ANY);
}

/*
 * Gets the camera ready to be used.
 * Doesn't need to be called when VideoCapture is initialized.
 * Returns operation success.
 */
bool GenericUSBCamera::openCamera()
{
    if (!videoCapture.isOpened())
    {
        videoCapture.open(0, cv::CAP_ANY);
        if (!videoCapture.isOpened())
        {
            cout << "ERROR! Unable to open camera\n";
            return false;
        }
    }
    return true;
}

/*
 * Releases the camera.
 * Doesn't need to be called before the object is getting destroyed.
 * Returns operation success.
 */
void GenericUSBCamera::closeCamera()
{
    if (videoCapture.isOpened())
    {
        videoCapture.release();
        if (videoCapture.isOpened())
        {
            cout << "Warning: unable to close camera\n";
        }
    }
}

/*
 * Grabs an image.
 * Returns if the image is empty (image grab failed).
 * Parameters:
 * - imagePtr: Image pointer to be filled by the function
 * - timestamp: Image timestamp in epoch to be filled by the function
 */
bool GenericUSBCamera::getImage(Mat *imagePtr, long *timestamp)
{
    Mat frame;

    // Waits until a frame is created (or times out)
    videoCapture >> frame;
    int64_t triggerTime_ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch()).count() /1000;

    if(frame.empty())
    {
        return false;
    }

    *imagePtr = frame;
    *timestamp = triggerTime_ms;
    return true;
}