#include "BaseMenu.h"

void BaseMenu::AddButton(const std::string &text, BaseMenu::Callback cb)
{
//    Button button;
//    button.rect = rect;
//    button.text = sfText;
//    button.callback = cb;

//    buttons.push_back(button);
}

void BaseMenu::Draw(sf::RenderWindow &window)
{
    if( false == enabled )
    {
        return;
    }

    for(const auto& button : buttons)
    {
        window.draw(button.rect);
        window.draw(button.text);
    }
}

void BaseMenu::HandleInput(glm::vec2 mousePos)
{
    if( false == enabled )
    {
        return;
    }

    for(const auto& button : buttons)
    {
        if( button.rect.getGlobalBounds().contains(mousePos.x, mousePos.y) )
        {
            if( nullptr != button.callback)
            {
                button.callback();
            }
        }
    }
}
