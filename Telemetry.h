//
// Created by Jonathan Hirsch on 2/17/20.
//


#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>


#ifndef SKYPASTA_TELEMETRY_H
#define SKYPASTA_TELEMETRY_H



using namespace std;
class Telemetry {
public:
    Telemetry(const string address, const int port);
    void connectServer();
    void readData();
    //void waitForData();

private:
    string address = "";
    const char* addressChar;
    int port = 0;
    int sock = 0;
    int valread = 0;
    char buffer[1024] = {0};
    struct sockaddr_in serv_addr;

};


#endif //SKYPASTA_TELEMETRY_H
