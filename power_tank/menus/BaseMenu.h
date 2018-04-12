#pragma once
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Text.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class BaseMenu
{
public:
    using Callback = std::function<void()>;

    void AddButton(const std::string& text, Callback cb);
    void Draw(sf::RenderWindow& window);
protected:
    void HandleInput(glm::vec2 mousePos);
    bool enabled = false;

    struct Button
    {
        sf::RectangleShape rect;
        sf::Text text;
        std::function<void()> callback;
    };
    std::vector<Button> buttons;
};
