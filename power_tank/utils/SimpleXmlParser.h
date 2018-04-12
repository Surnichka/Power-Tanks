#pragma once
#include <string>
#include <vector>
#include <map>
#include "SFML/Graphics/Rect.hpp"

class SimpleXmlParser
{
public:
    bool InitXML(const std::string& xmlPath);
    std::map<std::string, std::string> xmlData;
};
