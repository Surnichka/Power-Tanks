#include "ResourceMgr.h"
#include "SimpleXmlParser.h"
#include <iostream>

ResoruceMgr &ResoruceMgr::Get()
{
    static ResoruceMgr resMgr;
    return resMgr;
}

void ResoruceMgr::InitLevelUpResources()
{
    SimpleXmlParser simpleXmlParser;
    simpleXmlParser.InitXML("../Sprites/LevelUpSprites.xml");

    for (const auto& data : simpleXmlParser.xmlData)
    {
        auto& spriteName = data.first;
        pictures[spriteName] = Picture();

        if( false == pictures.at(spriteName).texture.loadFromFile(data.second) )
        {
            std::cout << "Error while loading: " << spriteName << " resource !" << std::endl;
            return;
        }
        auto& texture = pictures.at(spriteName).texture;
        pictures.at(spriteName).sprite.setTexture(texture);
    }
}

sf::Sprite &ResoruceMgr::GetSprite(const std::string &name)
{
    auto& sprite = pictures.at(name).sprite;
    return sprite;
}

sf::Texture &ResoruceMgr::GetTexture(const std::string &name)
{
    auto& texture = pictures.at(name).texture;
    return texture;
}
