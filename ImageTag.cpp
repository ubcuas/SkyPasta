//
// Created by Jonathan Hirsch on 2/18/20.
//

#include "ImageTag.h"
#include <iomanip>


string convertToDMS(double coordinate);
string convertLatToDMS(double lat);
string convertLonToDMS(double lon);

void ImageTag::addImage(const string image, const long timestamp, const double timeTaken) {

    imageQueue.push({image, timestamp, timeTaken});
    cout << "Adding image:" << timestamp << endl;

}

void ImageTag::addTelemetry(const char *telemetryData) {
    cout << " in Img tag" <<telemetryData << endl;

    string telemetryDataString = string(telemetryData);
    json telemetryJSON = json::parse(telemetryDataString);

    string lat = convertLatToDMS(telemetryJSON.at("latitude"));
    string lon = convertLonToDMS(telemetryJSON.at("longitude"));
    string altitude_agl_m = "15";//telemetryJSON.at("altitude_agl_meters");
    string altitude_msl_m = "10";//telemetryJSON.at("altitude_msl_meters");
    string heading = //"5";
            telemetryJSON.at("heading_degrees");


    long time = telemetryJSON.at("timestamp_msg");
    cout << "time JSON: "  << time << endl;

    telemetryQueue.push({telemetryDataString, time, lat, lon, heading, altitude_agl_m, altitude_msl_m});
}

void ImageTag::processNextImage(){


    if (!imageQueue.empty() && !telemetryQueue.empty()){
        ImageData currentData = imageQueue.front();

        TelemetryData currentTelem = telemetryQueue.front();


       string imageLocation = currentData.image;
        cout << "Tagging... ";
       cout <<imageLocation << endl;

        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(imageLocation);
        Exiv2::XmpData &XMPData = image->xmpData();

        XMPData["Xmp.UAS.Latitude"] = currentTelem.lat;
        XMPData["Xmp.UAS.Longitude"] = currentTelem.lon;
        XMPData["Xmp.UAS.Heading"] = currentTelem.heading;
        XMPData["Xmp.UAS.Altitude AGL m"] = currentTelem.alt_agl;
        XMPData["Xmp.UAS.Altitude MSL m"] = currentTelem.alt_msl;
        XMPData["Xmp.UAS.json"] = currentTelem.data;

        image->setXmpData(XMPData);
        image->writeMetadata();
        imageQueue.pop();
        telemetryQueue.pop();
    }
}


string convertLatToDMS(double lat){
    while (lat > 90 || lat < -90 ){
        lat /= 10;
    }

    return convertToDMS(lat);
}

string convertLonToDMS(double lon){
    while (lon > 180 || lon < -180){
        lon /= 10;
    }

    return convertToDMS(lon);
}

string convertToDMS(double degrees){

    double minutes = 0;
    if (degrees >= 0){
        minutes = (degrees - floor(degrees)) * 60.0;
    }
    else{
        minutes = (degrees - ceil(degrees)) * -60.0;
    }


    double seconds = (minutes - floor(minutes)) * 60.0;
    double tenths = (seconds - floor(seconds)) * 10.0;

    string degreesStr = to_string(floor(degrees));
    string minutesStr = to_string(floor(minutes));
    string secondsStr = to_string(seconds);

    degreesStr.erase (degreesStr.find_last_not_of('0') + 1, std::string::npos );
    degreesStr.erase (degreesStr.find_last_not_of('.') + 1, std::string::npos );

    minutesStr.erase (minutesStr.find_last_not_of('0') + 1, std::string::npos );
    minutesStr.erase (minutesStr.find_last_not_of('.') + 1, std::string::npos );

    secondsStr.erase (secondsStr.find_last_not_of('0') + 1, std::string::npos );
    secondsStr.erase (secondsStr.find_last_not_of('.') + 1, std::string::npos );

    string DMS = degreesStr + "º" + minutesStr + "\'"  + secondsStr+"\"";

    cout << DMS << endl;

    return DMS;
}

