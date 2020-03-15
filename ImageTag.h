//
// Created by Jonathan Hirsch on 2/18/20.
//

#include "Spinnaker.h"
#include <queue>
#include "lib/json.hpp"
#include <exiv2/exiv2.hpp>
#include "LinkedList.h"

#ifndef SKYPASTA_IMAGETAG_H
#define SKYPASTA_IMAGETAG_H


using json = nlohmann::json;

struct ImageData{
    std::string image;
    long timestamp;
    double timeTaken;
};

struct TelemetryData{
    std::string data;
    long timestamp;
    std::string lat;
    std::string lon;
    int heading;
    int alt_agl;
    int alt_msl;
};

class ImageTag {
public:
    ImageTag(){Exiv2::XmpProperties::registerNs("UAS/", "UAS");};
    void addImage(const std::string image, const long timestamp, const double time_taken);
    void addTelemetry(const char telemetryData []);
    void processNextImage();

private:
    std::queue<ImageData> imageQueue;
    LinkedList<TelemetryData> telemetryList;

    bool removeOldTelemData(long timestamp);
    double findDifference(long timestamp, TelemetryData telemetryData);
    bool findTelemDataAtTimestamp(long timestamp, TelemetryData& telemetryData);
};


#endif //SKYPASTA_IMAGETAG_H
