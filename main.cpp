#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <boost/thread.hpp>
#include <future>

#include "FlirCamera.h"
#include "ImageRetriever.h"

#include "SpinGenApi/SpinnakerGenApi.h"

#include "SpinnakerPlatform.h"

#define PORT 5000


using namespace std;

static const int RATE = 3; // delay between each image acquisition trigger

bool stopFlag = false;

void acquireImagesFixedRate(int rate, ImageRetriever ca){
    ca.startAcquisition();

    while (!stopFlag){
        cout << "aquiring........." << endl;
        //ca.triggerCameraOnce();
        auto myFuture(async(launch::async, &ImageRetriever::triggerCameraOnce, &ca));
        if (stopFlag){
            break;
        }
        sleep(rate);

        cout << endl;
    }

    ca.stopAcquisition();
}



int main() {

    try {
        FlirCamera flirCamera;
        flirCamera.setTrigger(SOFTWARE);

        cout << "Cameras Connected: " << flirCamera.getNumCameras() << endl;
        ImageRetriever ca(flirCamera.getCamera());
        ca.setTriggerMode(SINGLE_FRAME);


        auto myFuture(async(launch::async, acquireImagesFixedRate, RATE, ca));

        sleep(10);
        stopFlag = true;

        myFuture.get();


        ca.releaseCamera();

        flirCamera.cleanExit();
    }
    catch (Exception e){
        cout << "Error in Main:  ";
        cout << e.what() << endl;
    }




//    int sock = 0, valread;
//    struct sockaddr_in serv_addr;
//    char *hello = "Hello from client";
//    char buffer[1024] = {0};
//    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
//    {
//        printf("\n Socket creation error \n");
//        return -1;
//    }
//
//    serv_addr.sin_family = AF_INET;
//    serv_addr.sin_port = htons(PORT);
//
//    // Convert IPv4 and IPv6 addresses from text to binary form
//    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
//    {
//        printf("\nInvalid address/ Address not supported \n");
//        return -1;
//    }
//
//    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
//    {
//        printf("\nConnection Failed \n");
//        return -1;
//    }
//
//
//    valread = read( sock , buffer, 1024);
//    cout << buffer << endl << endl;
//    cout << "choose trigger: s or h" << endl;


}

