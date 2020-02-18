//
// Created by Jonathan Hirsch on 2/18/20.
//

#include "ImageTag.h"
//#include <json/reader.h>

//using namespace json;
void ImageTag::addImage(const ImagePtr image, long timestamp) {

    imageQueue.push({image, timestamp});

    cout << "Adding image:" << timestamp << endl;
}

void ImageTag::addTelemetry(const char *telemetryData) {
    cout << " in Img tag" <<telemetryData << endl;


}