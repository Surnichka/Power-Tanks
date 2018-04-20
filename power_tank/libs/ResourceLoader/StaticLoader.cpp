#include "StaticLoader.h"
#include <iostream>

StaticLoader::StaticLoader()
{
    static const int treeTypes = 14;
    static const int typeVariants = 7;

    for(int type = 1; type <= treeTypes; type++)
    {
        for(int variant = 0; variant <= typeVariants; variant++)
        {
            std::string treeKey = "tree_";
            treeKey += std::to_string(type);
            treeKey += "_";
            treeKey += std::to_string(variant);

            std::string treePath = "../TopiDay/assets/resources/trees/_tree_0";
            if(type >= 10)
            {
                treePath.pop_back();
            }
            treePath += std::to_string(type);
            treePath += "_";
            treePath += std::to_string(variant);
            treePath += "0000.png";

            m_resources[treeKey] = treePath;
        }
    }

    for(int i = 0; i <= 15; i++)
    {
        std::string cruisaderPath = "../TopiDay/assets/resources/crusader/";

        std::string zeros = "000";
        if(i >= 10)
        {
            zeros.pop_back();
        }

        std::string right_idle = "idle/crusader_idle_" + std::to_string(2) + zeros + std::to_string(i) + ".png";
        std::string left_idle = "idle/crusader_idle_" + std::to_string(6) + zeros  + std::to_string(i) + ".png";
        m_resources["right_idle" + std::to_string(i)] = cruisaderPath + right_idle;
        m_resources["left_idle" + std::to_string(i)] = cruisaderPath + left_idle;

        if( i <= 14 )
        {
            std::string right_walk = "walk/crusader_walk_" + std::to_string(2) + zeros + std::to_string(i) + ".png";
            std::string left_walk = "walk/crusader_walk_" + std::to_string(6) + zeros + std::to_string(i) + ".png";
            m_resources["right_walk" + std::to_string(i)] = cruisaderPath + right_walk;
            m_resources["left_walk" + std::to_string(i)] = cruisaderPath + left_walk;
        }
    }
}

bool StaticLoader::Load(const std::string &key, const std::string &path)
{
    if( false == m_textures[key].loadFromFile(path) )
    {
        std::cout << "Error while loading: " << path << " resource !" << std::endl;
        return false;
    }
    return true;
}

sf::Texture &StaticLoader::Get(const std::string &key)
{
    static sf::Texture empty;
    auto iter = m_textures.find(key);
    if( iter == m_textures.end() )
    {
        auto iter = m_resources.find(key);
        if( iter == m_resources.end() )
        {
            return empty;
        }
        if( false == Load(iter->first, iter->second) )
        {
            return empty;
        }
        return m_textures.at(key);
    }
    return iter->second;
}
