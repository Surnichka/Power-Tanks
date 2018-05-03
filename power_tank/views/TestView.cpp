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
    bar1.Init({100, 100, 500, 20}, 50, ProgressBar::Orientation::Horizontal, ProgressBar::FillDirection::Front);
    bar2.Init({100, 300, 500, 20}, 10, ProgressBar::Orientation::Horizontal, ProgressBar::FillDirection::End);

    bar3.Init({800, 100, 20, 500}, 100, ProgressBar::Orientation::Vertical, ProgressBar::FillDirection::Front);
    bar4.Init({1000, 100, 20, 500}, 200, ProgressBar::Orientation::Vertical, ProgressBar::FillDirection::End);
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
        //m_particleSystem.Emit(sf::Mouse::getPosition(window));
        bar1.Add();
        bar2.Add();
        bar3.Add();
        bar4.Add();
    }
    else if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        bar1.Pop();
        bar2.Pop();
        bar3.Pop();
        bar4.Pop();
    }

    bar1.Draw(window);
    bar2.Draw(window);
    bar3.Draw(window);
    bar4.Draw(window);
}

void TestView::Show()
{

}

void TestView::Hide()
{

}
