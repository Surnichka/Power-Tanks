#include "TestView.h"
#include "../utils/SignalSystem.h"
#include <random>
#include "../Window.h"

namespace
{
static std::random_device rd;
static std::default_random_engine rng(rd());
using Random = std::uniform_real_distribution<float>;
}
void TestView::Init()
{
    static const std::vector<sf::Color> colors =
    {
        sf::Color(0, 153, 255), sf::Color::Green, sf::Color::Yellow, sf::Color(255, 113, 181)
    };

    static const std::vector<sf::Color> pinkyy =
    {
        sf::Color(255,192,203),
        sf::Color(255,182,193),
        sf::Color(255,105,180),
        sf::Color(255,20,147),
        sf::Color(219,112,147),
    };

    for(int i = 0; i < 1000; i++)
    {
        int x = Random(100, 800)(rng);
        int y = Random(100, 800)(rng);
        m_boids.push_back(Boid({x, y}));

        sf::CircleShape circle;
        circle.setPosition(x, y);
        auto color = pinkyy.at( Random(0, colors.size())(rng));

        float radius = 8;
        circle.setFillColor(color);
        circle.setOrigin(radius, radius);
        circle.setOutlineColor(sf::Color::Red);
        circle.setOutlineThickness(1);
        circle.setRadius(radius);
        m_circles.push_back(circle);
    }
    GetSignals().ConnectSlot("pause_game", [this]()
    {
        pause = !pause;
    });
}

void TestView::Update(float dt)
{
    if( pause )
    {
        return;
    }

    for(auto& boid : m_boids) boid.wander();

    if( seek)
    {
        for(auto& boid : m_boids) boid.seek(m_mousePos);
    }

    if( flee )
    {
        for(auto& boid : m_boids) boid.flee(m_mousePos);
    }

    for(auto& boid : m_boids) boid.update();
}

void TestView::Draw(sf::RenderWindow &window)
{
    window.clear(sf::Color::White);

    auto mP = sf::Mouse::getPosition(window);
    m_mousePos = {mP.x, mP.y};
    seek = false;
    flee = false;
    if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
    {
        seek = true;
    }
    if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
    {
        flee = true;
    }

    for(size_t i = 0; i < m_boids.size(); i++)
    {
        const auto& src_pos = m_boids[i].m_position;
        const auto& dest_pos = m_boids[i].m_position + m_boids[i].m_velocity;

        m_circles[i].setPosition(src_pos.x, src_pos.y);

        double dx = double(src_pos.x - dest_pos.x);
        double dy = double(src_pos.y - dest_pos.y);
        float angle = float(atan2(dy, dx) * 180 / M_PI - 270);

        sf::RectangleShape rect;
        rect.setPosition(src_pos.x, src_pos.y);
        rect.setSize({1, 21});
        rect.setRotation(angle);
        rect.setFillColor(sf::Color::Red);

        window.draw(m_circles[i]);
        window.draw(rect);
    }
}

void TestView::Show()
{

}

void TestView::Hide()
{

}
