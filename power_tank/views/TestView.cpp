#include "TestView.h"
#include <random>
#include "SFML/Graphics.hpp"

namespace
{
    static std::random_device rd;
    static std::default_random_engine rng(rd());
    using Random = std::uniform_real_distribution<float>;
}

void TestView::Init()
{
    for(int i = 0; i < 10; i++)
    {
        glm::vec2 loc;
        loc.x = Random(50, 1000)(rng);
        loc.y = Random(50, 800)(rng);

        auto dumb = DumbBoid::Create(loc);
        dumbies.push_back(dumb);
    }
//    colMgr.SetResolveCollisionFunc(nullptr);
}

void TestView::Update(float dt)
{
    for(auto dumb1 : dumbies)
    {
        for(auto dumb2 : dumbies)
        {
            colMgr.Process(dumb1, dumb2);
        }
    }

    for(auto dumb : dumbies)
    {
        dumb->Update();
    }

}

void TestView::Draw(sf::RenderWindow& window)
{
    auto m = sf::Mouse::getPosition(window);
    if( sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        dumbies.push_back(DumbBoid::Create({m.x, m.y}));
    }

    window.clear({169, 169, 169});
    for(auto dumb : dumbies)
    {
        dumb->Draw(window);
    }

    for(auto dumb : dumbies)
    {
        //SteeringManager::Wander(dumb);
    }
}

void TestView::Show()
{

}

void TestView::Hide()
{

}
