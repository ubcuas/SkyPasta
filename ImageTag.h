//
// Created by Jonathan Hirsch on 2/18/20.
//

#include "Spinnaker.h"
#include <queue>
#include "lib/json.hpp"

#ifndef SKYPASTA_IMAGETAG_H
#define SKYPASTA_IMAGETAG_H

using namespace Spinnaker;
using namespace std;
using json = nlohmann::json;

struct imageData{
    ImagePtr image;
    long timestamp;
    double timeTaken;
};

struct telemetryData{
    json data;
    long timestamp;
};

class ImageTag {
public:
    ImageTag(){};
    void addImage(const ImagePtr image, const long timestamp, const double time_taken);
    void addTelemetry(const char telemetryData []);
    void processNextImage();

private:
    queue <imageData> imageQueue;
    queue <telemetryData> telemetryQueue;
};


#endif //SKYPASTA_IMAGETAG_H
