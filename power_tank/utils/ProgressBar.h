#pragma once
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Text.hpp"

class ProgressBar
{
public:
private:
    sf::RectangleShape bar;
    sf::RectangleShape progress;
    sf::Text text;
};
