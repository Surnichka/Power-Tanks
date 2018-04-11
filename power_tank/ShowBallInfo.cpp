#include "ShowBallInfo.h"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/RectangleShape.hpp"

void ShowBallInfo::Draw(sf::RenderWindow &window, const Ball &ball)
{
    sf::RectangleShape rect;
    rect.setPosition(ball.m_position.x + ball.m_radius, ball.m_position.y);
    rect.setSize({100, 100});
    rect.setFillColor({110, 110, 110, 150});
    rect.setOutlineColor(sf::Color::Black);
    rect.setOutlineThickness(1);


}
