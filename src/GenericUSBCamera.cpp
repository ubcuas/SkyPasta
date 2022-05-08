#include "GenericUSBCamera.h"
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

GenericUSBCamera::GenericUSBCamera()
{   
    videoCapture = VideoCapture(8, cv::CAP_ANY);
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
        videoCapture.open(8, cv::CAP_ANY);
        if (!videoCapture.isOpened())
        {
            cout << "ERROR! Unable to open camera\n";
            return false;
        }
    }
    cout << "!!!!!!!!!!!!!!!!!!! fr" << videoCapture.CAP_PROP_FPS << endl;
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
 */
bool GenericUSBCamera::getImage(Mat *imagePtr)
{
    Mat frame;

    // Waits until a frame is created (or times out)
    videoCapture >> frame;

    if(frame.empty())
    {
        return false;
    }

    *imagePtr = frame;
    return true;
}