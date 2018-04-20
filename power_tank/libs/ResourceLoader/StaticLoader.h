#pragma once
#include "SFML/Graphics/Texture.hpp"
#include <map>

class StaticLoader
{
public:
    StaticLoader();

    bool Load(const std::string& key, const std::string& path);
    sf::Texture& Get(const std::string& key);
private:
    std::map<std::string, sf::Texture> m_textures;
    std::map<std::string, std::string> m_resources;
};
inline StaticLoader& GetLoader()
{
    static StaticLoader sl;
    return sl;
}
