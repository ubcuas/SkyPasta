//
// Created by Jonathan Hirsch on 2/18/20.
//

#include "ImageTag.h"
#include <mutex>

constexpr int errorMargin = 1;
constexpr int errorMarginLarge = errorMargin*1.5;

using namespace Spinnaker;
using namespace std;
std::mutex mtx;           // mutex for critical section


namespace {
    // erases trailing 0 and decimals from given string
    void erase0anddot(string& str){
        str.erase(str.find_last_not_of('0') + 1, std::string::npos);
        str.erase(str.find_last_not_of('.') + 1, std::string::npos);
    }

    // converts a given coordinate from degrees to DMS
    string convertToDMS(double degrees) {

        double minutes = 0;
        if (degrees >= 0) {
            minutes = (degrees - floor(degrees)) * 60.0;
        } else {
            minutes = (degrees - ceil(degrees)) * -60.0;
        }

        double seconds = (minutes - floor(minutes)) * 60.0;

        string degreesStr = to_string(floor(degrees));
        string minutesStr = to_string(floor(minutes));
        string secondsStr = to_string(seconds);

        erase0anddot(degreesStr);
        erase0anddot(minutesStr);
        erase0anddot(secondsStr);

        string DMS = degreesStr + "\u00BA" + to_string(char(65)) + minutesStr + "\'" + secondsStr + "\"";

        return DMS;
    }

    string convertLatToDMS(double lat) {
        while (lat > 90 || lat < -90) {
            lat /= 10;
        }
        return convertToDMS(lat);
    }

    string convertLonToDMS(double lon) {
        while (lon > 180 || lon < -180) {
            lon /= 10;
        }
        return convertToDMS(lon);
    }
}

void ImageTag::addImage(const string image, const long timestamp) {
    imageQueue.push({image, timestamp});
    cout << "Adding image:" << timestamp << endl;
}

// adds given JSON telemetry data point to linked list
void ImageTag::addTelemetry(const char *telemetryData) {
    string telemetryDataString = string(telemetryData);
    json telemetryJSON = json::parse(telemetryDataString);

    string lat = convertLatToDMS(telemetryJSON.at("lat"));
    string lon = convertLonToDMS(telemetryJSON.at("lng"));
    int altitude_agl_m = telemetryJSON.at("alt");
    int altitude_msl_m = 0;
    int heading = telemetryJSON.at("heading");
    long timestamp_msg = telemetryJSON.at("timestamp");
    long timestamp_telem = telemetryJSON.at("timestamp");

    // telemetryList.push({telemetryDataString, timestamp_msg, timestamp_telem, lat, lon, heading, altitude_agl_m, altitude_msl_m});
    telemetryList.push_back({telemetryDataString, timestamp_msg, timestamp_telem, lat, lon, heading, altitude_agl_m, altitude_msl_m});
    cout << "Adding Telem:" << timestamp_telem << endl;
}

// Find telemetry data for next image, and writes the data to UAS XMP Namespace on the image location
// removes used images and telemetry data from relevant lists
void ImageTag::processNextImage() {
    if (!imageQueue.empty() && !telemetryList.empty()) {
        ImageData currentData = imageQueue.front();

        TelemetryData currentTelem;

        bool found = findTelemDataAtTimestamp(currentData.timestamp, currentTelem);

        if (!found) {
            return;
        }

        string imageLocation = currentData.image;
        cout << "Tagging..." << imageLocation << endl;

        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(imageLocation);
        Exiv2::XmpData &XMPData = image->xmpData();

        image->writeXmpFromPacket(true);

        XMPData["Xmp.UAS.Latitude"] = currentTelem.lat;
        XMPData["Xmp.UAS.Longitude"] = currentTelem.lon;
        XMPData["Xmp.UAS.Heading_dgress"] = currentTelem.heading;
        XMPData["Xmp.UAS.Altitude_AGL_m"] = currentTelem.alt_agl;
        XMPData["Xmp.UAS.Altitude_MSL_m"] = currentTelem.alt_msl;
        XMPData["Xmp.UAS.Timestamp_msg_millis"] = currentTelem.timestamp_msg;
        XMPData["Xmp.UAS.Timestamp_Telem_millis"] = currentTelem.timestamp_telem;
        XMPData["Xmp.UAS.Timestamp_Image_millis"] = currentData.timestamp;
        XMPData["Xmp.UAS.Time_Difference_millis"] = abs(currentTelem.timestamp_telem - currentData.timestamp);
        XMPData["Xmp.UAS.json"] = currentTelem.data;

        image->setXmpData(XMPData);
        image->writeMetadata();
        image.release();
        cout << "Wrote XMP data to " << imageLocation << endl;
        imageQueue.pop();
        // telemetryList.pop();
        // telemetryList.pop_back();
    } else {
        // cout<< "Trying to tag" << endl;
    }
}



// Cycles through old telemetry data, and compares it to given timestamp_telem
// Removes telemetry data which is too old to be relevant to the given image
bool ImageTag::removeOldTelemData(long timestamp) {
    if (telemetryList.empty()){
        return true;
    }
    long difference;
    // auto currentNode = telemetryList.front();
    auto currentNode = telemetryList.begin();
    difference = findDifference(timestamp, *currentNode);
    cout << "Difference in remove: " << difference << endl;

    if (difference < errorMarginLarge) {
        return true;
    }

    mtx.lock();
    while (true) {
        // difference = findDifference(timestamp, currentNode->data);

        // if (!currentNode->next) {
        //     break;
        // }
        if (currentNode == telemetryList.end()){
            break;
        }
        // currentNode = currentNode->next;
        currentNode = std::next(currentNode, 1);
        telemetryList.erase(telemetryList.begin());

        difference = findDifference(timestamp, *currentNode);
        cout << "Difference in remove: " << difference << endl;
        
        if (difference < errorMarginLarge) {
            mtx.unlock();
            return true;
        }
    }

    mtx.unlock();
    return false;
}

// Returns the absolute value difference between a given timestamp_telem and a given telemetry data point
long ImageTag::findDifference(long timestamp, const TelemetryData& telemetryData) {
    return abs(telemetryData.timestamp_telem - timestamp);
}

// Finds telemetry data closest to time at which image was taken
// Returns false if no data point is close enough
bool ImageTag::findTelemDataAtTimestamp(long timestamp, TelemetryData &telemetryData) {
    if (telemetryList.empty()) {
        return false;
    }

    // removes old telemetry data
    removeOldTelemData(timestamp);
    // auto currentDataNode = telemetryList.front();
    auto currentDataNode = telemetryList.begin();
    long currentDataDifference;
    long nextDataDifference = 0;

/*
 * Compares telemetry data and timestamp_telem on images
 * Strives to produce a delay of < errorMargin ms between the timestamp_telem on image & timestamp_telem of telemetry data.
 * Returns true if data is found (assigned to telemetryData), false otherwise
 *
 * If current data point is within errorMargin ms error margin, return true & assigns telemetryData to that object
 * Otherwise, checks size of the list. If the list has more than 1 object in it, compare next object with current
 * the one before it. if next is smaller than errorMargin, return next. Otherwise, run loop again.
 *
 */

    mtx.lock();
    while (true) {
        currentDataDifference = findDifference(timestamp, *currentDataNode);

        if (currentDataDifference < errorMargin) {
            telemetryData = *currentDataNode;
            mtx.unlock();
            return true;

        } else if (currentDataNode != telemetryList.end()) {
            auto nextDataNode = std::next(currentDataNode, 1);
            nextDataDifference = findDifference(timestamp, *nextDataNode);
            if (nextDataDifference < errorMargin) {
                telemetryData = *nextDataNode;
                mtx.unlock();
                return true;

            } else if (nextDataDifference > currentDataDifference) {
                telemetryData = *currentDataNode;
                sleepForNS(90000000L);
                mtx.unlock();
                return true;
            }
            else {
                currentDataNode = nextDataNode;
            }
        } else {
            sleepForNS(130000000L);
            mtx.unlock();
            return false;
        }
    }
    mtx.unlock();
}

// sleep for specified duration in nanoseconds
void ImageTag::sleepForNS(long sleepTime) const {
    const struct timespec sleepTimeSpec = {0, sleepTime};
    nanosleep(&sleepTimeSpec, nullptr);
}

