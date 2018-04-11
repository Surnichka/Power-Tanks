#pragma once
#include <map>
#include "SFML/Graphics.hpp"

class LevelCounter
{
public:
    LevelCounter();

    void Enable();
    void GainExp();
    int GetCurrentLevel() const;
    int GetCurrentExp() const;
    void Draw(sf::RenderWindow& window);
private:
    bool enabled = true;
    sf::RectangleShape box;

    int currentLevel = 1;
    static const int expirienceRate = 2;
    static const int maxLevel = 15;
    std::map<int, int> expirienceRequirements;
    std::map<int, int> expirienceGained;
};
