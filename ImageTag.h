//
// Created by Jonathan Hirsch on 2/18/20.
//

#include "Spinnaker.h"
#include <queue>
#include "lib/json.hpp"
#include <exiv2/exiv2.hpp>

#ifndef SKYPASTA_IMAGETAG_H
#define SKYPASTA_IMAGETAG_H

using namespace Spinnaker;
using namespace std;
using json = nlohmann::json;

struct ImageData{
    string image;
    long timestamp;
    double timeTaken;
};

struct TelemetryData{
    string data;
    long timestamp;
    string lat;
    string lon;
    string heading;
    string alt_agl;
    string alt_msl;
};

class ImageTag {
public:
    ImageTag(){Exiv2::XmpProperties::registerNs("UAS/", "UAS");};
    void addImage(const string image, const long timestamp, const double time_taken);
    void addTelemetry(const char telemetryData []);
    void processNextImage();

private:
    queue <ImageData> imageQueue;
    queue <TelemetryData> telemetryQueue;
    void writeImageData(const string& file, Exiv2::ExifData& exifData);
};


#endif //SKYPASTA_IMAGETAG_H
