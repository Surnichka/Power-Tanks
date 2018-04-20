#include "TestView.h"
#include <random>
#include "SFML/Graphics.hpp"

namespace
{
    static std::random_device rd;
    static std::default_random_engine rng(rd());
    using Random = std::uniform_real_distribution<float>;
}

TestView::TestView()
{

}

void TestView::Init()
{

}

void TestView::Update(float dt)
{
    m_particleSystem.Update(dt);
}

void TestView::Draw(sf::RenderWindow& window)
{
    window.clear(sf::Color::White);
    m_particleSystem.Draw(window);

    if(sf::Mouse::isButtonPressed(sf::Mouse::Right))
    {
        m_particleSystem.Emit(sf::Mouse::getPosition(window));
    }
}

void TestView::Show()
{

}

void TestView::Hide()
{

}
