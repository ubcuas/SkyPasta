//
// Created by Jonathan Hirsch on 2/17/20.
//

#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include "ImageTag.h"

#ifndef SKYPASTA_TELEMETRY_H
#define SKYPASTA_TELEMETRY_H

class Telemetry {
public:
    Telemetry(const std::string address, const int port, ImageTag *imageTag);
    int connectServer();
    int readData();
    bool isConnected()const {return this -> connected;};

private:
    bool connected = false;
    const char* addressChar;
    int port = 0;
    int sock = 0;
    int valread = 0;
    char buffer[1024] = {0};
    struct sockaddr_in serv_addr;
    ImageTag *imageTag;
};

#endif //SKYPASTA_TELEMETRY_H
