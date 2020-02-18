//
// Created by Jonathan Hirsch on 2/18/20.
//

#include "Spinnaker.h"
#include <queue>

#ifndef SKYPASTA_IMAGETAG_H
#define SKYPASTA_IMAGETAG_H

using namespace Spinnaker;
using namespace std;

struct imageData{
    ImagePtr image;
    long timestamp;
};

struct telemetryData{
    double lat;
    double lon;
    long timestamp;
};

class ImageTag {
public:
    ImageTag(){};
    void addImage(const ImagePtr image, long timestamp);
    void addTelemetry(const char telemetryData []);
    void processNextImage();

private:
    queue <imageData> imageQueue;
    queue <telemetryData> telemetryQueue;
};


#endif //SKYPASTA_IMAGETAG_H
