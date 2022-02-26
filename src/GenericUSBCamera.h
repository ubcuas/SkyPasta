#ifndef GENERIC_USB_CAMERA_H
#define GENERIC_USB_CAMERA_H

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#pragma once

/*
 * Uses OpenCV to control any generic USB cameras. Tries to connect to the first camera found by the system. 
 */
class GenericUSBCamera
{
public:
    GenericUSBCamera();
    bool captureImages();
    bool getImage(cv::Mat *imagePtr);
    bool openCamera();
    void closeCamera();

private:
    // The camera object
    cv::VideoCapture videoCapture;
};


#endif //GENERIC_USB_CAMERA_H