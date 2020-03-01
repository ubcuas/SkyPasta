//
// Created by Jonathan Hirsch on 2/17/20.
//

#include "Telemetry.h"

using namespace std;

Telemetry::Telemetry(const string address, const int port, ImageTag *imageTag){
    addressChar = address.c_str();
    this -> port = port;
    this -> imageTag = imageTag;
}

void Telemetry::connectServer(){
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, addressChar, &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return;
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