#pragma once

#include <axml/axml_parser.h>

class ApkInfo {

public:
    int versionCode;
    std::string versionName;

    static ApkInfo fromXml(axml::AXMLParser& parser);

};