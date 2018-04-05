#pragma once
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Text.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class DebugMenu
{
public:
    using Callback = std::function<void()>;
    DebugMenu();
    void AddButton(const std::string& text, Callback cb);
    void Draw(sf::RenderWindow& window);
private:
    void HandleInput(glm::vec2 mousePos);
    bool enabled = false;

    static const int maxRow = 3;
    static const int maxCol = 7;
    static const int cellWidth = 125;
    static const int cellHeight = 75;

    struct Button
    {
        sf::RectangleShape rect;
        sf::Text text;
        std::function<void()> callback;
    };
    std::vector<Button> buttons;
};

DebugMenu& GetDebugMenu();


