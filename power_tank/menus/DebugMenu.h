#pragma once
#include "BaseMenu.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Text.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class DebugMenu : public BaseMenu
{
public:
    DebugMenu();
    void AddButton(const std::string& text, Callback cb);
    void Draw(sf::RenderWindow& window);
private:
    void HandleInput(glm::vec2 mousePos);

    static const int maxRow = 4;
    static const int maxCol = 8;
    static const int cellWidth = 125;
    static const int cellHeight = 75;
};

DebugMenu& GetDebugMenu();


