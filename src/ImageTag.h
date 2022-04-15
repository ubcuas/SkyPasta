//
// Created by Jonathan Hirsch on 2/18/20.
//

#include "Spinnaker.h"
#include <queue>
#include "json.hpp"
#include <exiv2/exiv2.hpp>
#include "LinkedList.h"
#include <iomanip>
#include <iostream>
#include <iterator>
#include <vector> // vector


#ifndef SKYPASTA_IMAGETAG_H
#define SKYPASTA_IMAGETAG_H


using json = nlohmann::json;

struct ImageData{
    std::string image;
    long timestamp;
};

struct TelemetryData{
    std::string data;
    long timestamp_telem;
    long timestamp_msg;
    std::string lat;
    std::string lon;
    int heading;
    int alt_agl;
    int alt_msl;
};

class ImageTag {
public:
    ImageTag(){Exiv2::XmpProperties::registerNs("UAS/", "UAS");};
    void addImage(const std::string image, const long timestamp);
    void addTelemetry(const char telemetryData []);
    void processNextImage();

private:
    std::queue<ImageData> imageQueue;
    // LinkedList<TelemetryData> telemetryList;
    std::vector<TelemetryData> telemetryList;

    void sleepForNS(long sleepTime) const;

    long findDifference(long timestamp, const TelemetryData& telemetryData);
    bool removeOldTelemData(long timestamp);
    bool findTelemDataAtTimestamp(long timestamp, TelemetryData& telemetryData);
};


#endif //SKYPASTA_IMAGETAG_H
