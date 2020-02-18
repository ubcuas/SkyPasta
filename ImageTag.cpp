//
// Created by Jonathan Hirsch on 2/18/20.
//

#include "ImageTag.h"
//#include <json/reader.h>

//using namespace json;
void ImageTag::addImage(const ImagePtr image, const long timestamp, const double timeTaken) {

    imageQueue.push({image, timestamp, timeTaken});
    cout << "Adding image:" << timestamp << endl;
}

void ImageTag::addTelemetry(const char *telemetryData) {
    cout << " in Img tag" <<telemetryData << endl;

    string telemetryDataString = string(telemetryData);
    json telemetryJSON = json::parse(telemetryDataString);

    long time = telemetryJSON.at("timestamp_msg");
    cout << "time JSON: "  << time << endl;

    telemetryQueue.push({telemetryJSON, time});
}

void ImageTag::processNextImage(){

    if (!imageQueue.empty()){

    }
}


void convertToDMS(double coordinate){

}

