//
// Created by Jonathan Hirsch on 2/18/20.
//

#include "ImageTag.h"
#include <iomanip>

constexpr int errorMargin = 500;
constexpr int errorMarginLarge = errorMargin*1.5;

using namespace Spinnaker;
using namespace std;

namespace {

    void erase0anddot(string& str){
        str.erase(str.find_last_not_of('0') + 1, std::string::npos);
        str.erase(str.find_last_not_of('.') + 1, std::string::npos);
    }

    string convertToDMS(double degrees) {

        double minutes = 0;
        if (degrees >= 0) {
            minutes = (degrees - floor(degrees)) * 60.0;
        } else {
            minutes = (degrees - ceil(degrees)) * -60.0;
        }

        double seconds = (minutes - floor(minutes)) * 60.0;
        double tenths = (seconds - floor(seconds)) * 10.0;

        string degreesStr = to_string(floor(degrees));
        string minutesStr = to_string(floor(minutes));
        string secondsStr = to_string(seconds);

        erase0anddot(degreesStr);
        erase0anddot(minutesStr);
        erase0anddot(secondsStr);

        string DMS = degreesStr + "º" + minutesStr + "\'" + secondsStr + "\"";

        cout << DMS << endl;

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

void ImageTag::addImage(const string image, const long timestamp, const double timeTaken) {
    imageQueue.push({image, timestamp, timeTaken});
    cout << "Adding image:" << timestamp << endl;
}

void ImageTag::addTelemetry(const char *telemetryData) {
    string telemetryDataString = string(telemetryData);
    json telemetryJSON = json::parse(telemetryDataString);

    string lat = convertLatToDMS(telemetryJSON.at("latitude"));
    string lon = convertLonToDMS(telemetryJSON.at("longitude"));
    string altitude_agl_m = "15";//telemetryJSON.at("altitude_agl_meters");
    string altitude_msl_m = "10";//telemetryJSON.at("altitude_msl_meters");
    string heading = "5";//telemetryJSON.at("heading_degrees");


    long time = telemetryJSON.at("timestamp_msg");
    cout << "time JSON: " << time << endl;

    telemetryList.push({telemetryDataString, time, lat, lon, heading, altitude_agl_m, altitude_msl_m});
}

void ImageTag::processNextImage() {
    if (!imageQueue.empty() && !telemetryList.empty()) {
        ImageData currentData = imageQueue.front();

        TelemetryData currentTelem;

        bool found = findTelemDataAtTimestamp(currentData.timestamp, currentTelem);

        if (found == false) {
            return;
        }

        string imageLocation = currentData.image;
        cout << "Tagging..." << imageLocation << endl;

        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(imageLocation);
        Exiv2::XmpData &XMPData = image->xmpData();

        XMPData["Xmp.UAS.Latitude"] = currentTelem.lat;
        XMPData["Xmp.UAS.Longitude"] = currentTelem.lon;
        XMPData["Xmp.UAS.Heading"] = currentTelem.heading;
        XMPData["Xmp.UAS.Altitude_AGL_m"] = currentTelem.alt_agl;
        XMPData["Xmp.UAS.Altitude_MSL_m"] = currentTelem.alt_msl;
        XMPData["Xmp.UAS.TimestampTelem"] = currentTelem.timestamp;
        XMPData["Xmp.UAS.TimestampImage"] = currentData.timestamp;
        XMPData["Xmp.UAS.Time_Difference"] = abs(currentTelem.timestamp - currentData.timestamp);
        XMPData["Xmp.UAS.json"] = currentTelem.data;

        image->setXmpData(XMPData);
        image->writeMetadata();
        imageQueue.pop();
        telemetryList.pop();
    }
}

/*
 * Compares telemetry data and timestamp on images
 * Strives to produce a delay of < errorMargin ms between the timestamp on image & timestamp of telemetry data.
 * Returns true if data is found (assigned to telemetryData), false otherwise
 *
 * If current data point is within errorMargin ms error margin, return true & assigns telemetryData to that object
 * Otherwise, checks size of the list. If the list has more than 1 object in it, compare next object with current
 * the one before it. if next is smaller than errorMargin, return next. Otherwise, run loop again.
 *
 */

bool ImageTag::removeOldTelemData(long timestamp) {
    double difference;
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


double ImageTag::findDifference(long timestamp, TelemetryData telemetryData) {
    return abs(telemetryData.timestamp - timestamp);
}


bool ImageTag::findTelemDataAtTimestamp(long timestamp, TelemetryData &telemetryData) {
    if (telemetryList.empty()) {
        return false;
    }

    removeOldTelemData(timestamp);
    auto currentDataNode = telemetryList.front();
    double currentDataDifference;
    double nextDataDifference = 0;

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
                nanosleep((const struct timespec[]){{0, 90000000L}}, NULL);
                return true;
            }
            else {
                currentDataNode = currentDataNode->next;
            }
        } else {
            nanosleep((const struct timespec[]){{0, 130000000L}}, NULL);
            return false;
        }
    }
}

