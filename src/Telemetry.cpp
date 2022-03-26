//
// Created by Jonathan Hirsch on 2/17/20.
//

#include "Telemetry.h"
#include <cpr/cpr.h>
#include <iostream>

using namespace std;

Telemetry::Telemetry(const string address, const int port, ImageTag *imageTag){
    addressChar = address.c_str();
    this -> port = port;
    this -> imageTag = imageTag;
}

int Telemetry::connectServer(){
    connected = false;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, addressChar, &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    connected = true;
}

int Telemetry::readData(){

    if (connected) {
        valread = read(sock, buffer, 1024);
        cout << buffer << endl << endl;
        imageTag->addTelemetry(buffer);

    } else {
        cout << "Telemetry Error: Not connected" << endl;
        return -1;
    }

    return 0;
}

int Telemetry::readJsonFromAcom(){
    // Make the http request to ACOM
    cpr::Response r1 = cpr::Get(cpr::Url{"http://localhost:5000/aircraft/telemetry/gps_with_timestamp"});
    
    if (r1.status_code != 200){
        cout << "Telemetry Error: Not connected" << endl;
        return -1;
    }

    // Convert string to char*
    string s = r1.text;
    char *buffer = s.data();

    // Push to addTelemtry call
    imageTag->addTelemetry(buffer);

    return 0;
}