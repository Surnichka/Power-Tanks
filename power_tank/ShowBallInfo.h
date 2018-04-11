#pragma once
#include "SFML/Graphics/RenderWindow.hpp"
#include "Ball.h"

class ShowBallInfo
{
public:
    static void Draw(sf::RenderWindow& window, const Ball& ball);
};
