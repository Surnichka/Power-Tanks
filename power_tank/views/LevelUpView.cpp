#include "LevelUpView.h"
#include "../Window.h"
#include "../utils/ResourceMgr.h"

void LevelUpView::Init()
{
    rect.setSize({Window::Playable::width,Window::Playable::height});
    rect.setFillColor({170,170,170});
    levelMenu.Init();
}

void LevelUpView::Update(float dt)
{

}

void LevelUpView::Draw(sf::RenderWindow &window)
{
    window.draw(rect);
    levelMenu.Draw(window);
}

void LevelUpView::Show()
{

}

void LevelUpView::Hide()
{

}
