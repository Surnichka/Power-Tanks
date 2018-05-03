#pragma once
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"

class ProgressBar
{
public:
    enum class Orientation{ Horizontal, Vertical };
    enum class FillDirection{ Front, End };
    void Init(sf::FloatRect rect, int maxElement, Orientation orientation, FillDirection fillDir);
    void Add();
    void Pop();
    void Draw(sf::RenderWindow& window);
    int GetCurrent() const;
    bool isFull() const;
private:
    int m_maxElement = 0;
    int m_currentElement = 0;

    int m_progressCounter = 0;

    sf::FloatRect m_rect;
    Orientation m_orientation = Orientation::Horizontal;
    FillDirection m_fillDir = FillDirection::Front;
    std::vector<sf::RectangleShape> m_bar;
};

