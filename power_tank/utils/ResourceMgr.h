#pragma once
#include <map>
#include "SFML/Graphics.hpp"

class ResoruceMgr
{
public:
    static ResoruceMgr& Get();

    void InitLevelUpResources();

    sf::Sprite& GetSprite(const std::string& name);
    sf::Texture& GetTexture(const std::string& name);
private:
    struct Picture
    {
        sf::Texture texture;
        sf::Sprite sprite;
    };

    std::map<std::string, Picture> pictures;
};
