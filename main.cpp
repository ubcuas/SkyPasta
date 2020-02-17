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


int main() {

    try {
        FlirCamera flirCamera;
        flirCamera.setTrigger(SOFTWARE);

        cout << "Cameras Connected: " << flirCamera.getNumCameras() << endl;
        ImageRetriever ca(flirCamera.getCamera());

        auto myFuture(async(launch::async, &ImageRetriever::startAcquisition, &ca));

        sleep(5);
        ca.stopAcquisition();
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

