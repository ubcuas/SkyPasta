//
// Created by Jonathan Hirsch on 2/18/20.
//

#include "ImageTag.h"

constexpr int errorMargin = 500;
constexpr int errorMarginLarge = errorMargin*1.5;

using namespace Spinnaker;
using namespace std;

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

    string lat = convertLatToDMS(telemetryJSON.at("latitude"));
    string lon = convertLonToDMS(telemetryJSON.at("longitude"));
    int altitude_agl_m = telemetryJSON.at("altitude_agl_meters");
    int altitude_msl_m = telemetryJSON.at("altitude_msl_meters");
    int heading = telemetryJSON.at("heading_degrees");
    long timestamp_msg = telemetryJSON.at("timestamp_msg");
    long timestamp_telem = telemetryJSON.at("timestamp_telem");

    telemetryList.push({telemetryDataString, timestamp_msg, timestamp_telem, lat, lon, heading, altitude_agl_m, altitude_msl_m});
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
        telemetryList.pop();
    }
}



// Cycles through old telemetry data, and compares it to given timestamp_telem
// Removes telemetry data which is too old to be relevant to the given image
bool ImageTag::removeOldTelemData(long timestamp) {
    long difference;
    auto currentNode = telemetryList.front();
    while (true) {
        difference = findDifference(timestamp, currentNode->data);
        cout << "Difference in remove: " << difference << endl;

        if (difference < errorMarginLarge) {
            telemetryList.setHead(currentNode);
            return true;
        }
        if (!currentNode->next) {
            break;
        }
        currentNode = currentNode->next;
    }
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
    auto currentDataNode = telemetryList.front();
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
    while (true) {
        currentDataDifference = findDifference(timestamp, currentDataNode->data);

        if (currentDataDifference < errorMargin) {
            telemetryData = currentDataNode->data;
            return true;

        } else if (currentDataNode->next) {
            nextDataDifference = findDifference(timestamp, currentDataNode->next->data);
            if (nextDataDifference < errorMargin) {
                telemetryData = currentDataNode->next->data;
                return true;

            } else if (nextDataDifference > currentDataDifference) {
                telemetryData = currentDataNode->data;
                sleepForNS(90000000L);
                return true;
            }
            else {
                currentDataNode = currentDataNode->next;
            }
        } else {
            sleepForNS(130000000L);
            return false;
        }
    }
}

// sleep for specified duration in nanoseconds
void ImageTag::sleepForNS(long sleepTime) const {
    nanosleep((const struct timespec[]){{0, sleepTime}}, nullptr);
}

