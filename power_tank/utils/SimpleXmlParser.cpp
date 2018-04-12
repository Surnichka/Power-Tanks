#include "SimpleXmlParser.h"
#include <fstream>
#include <iostream>

bool SimpleXmlParser::InitXML(const std::string &xmlPath)
{
    auto file = std::ifstream(xmlPath);
    if( false == file.is_open() )
    {
        std::cout << "Xml : " << xmlPath << ", failed to open !" << std::endl;
        return false;
    }

    std::string searchFor = "\"";
    std::string line;
    while (std::getline(file, line))
    {
        if( line.find("SubTexture") == std::string::npos )
        {
            continue;
        }

        auto name_p_begin = line.find(searchFor);
        auto name_p_end = line.find(searchFor, name_p_begin + 1);
        std::string name = line.substr(name_p_begin + 1, name_p_end - name_p_begin - 1);

        auto path_p_begin = line.find(searchFor, name_p_end + 1);
        auto path_p_end = line.find(searchFor, path_p_begin + 1);
        xmlData[name] = line.substr(path_p_begin + 1, path_p_end - path_p_begin - 1);
    }

    return true;
}
