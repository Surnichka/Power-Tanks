#include "BoidsExampleView.h"
#include <random>
#include <iostream>
#include "../utils/FontMgr.h"

namespace
{
    static std::random_device rd;
    static std::default_random_engine rng(rd());
    using Random = std::uniform_real_distribution<float>;
}

void BoidsExampleView::Init()
{
    collisionMgr.SetOnCollisionCB([](IBoid::Ptr me, IBoid::Ptr other)
    {
        auto me1 = std::static_pointer_cast<SimpleEnemy>(me);
        auto other1 = std::static_pointer_cast<SimpleEnemy>(other);

        bool bothHunter = me1->hunter && other1->hunter;
        if( bothHunter )
        {
            return;
        }

        if( me1->hunter ){ other1->hurt(); }
        if( other1->hunter ) { me1->hurt(); }
    });
    collisionMgr.SetResolveWallHitFunc(CollisionManager::Preset::MoveThroughWall);

    for(int i = 0; i < 300; i++)
    {
        int x = Random(300, 700)(rng);
        int y = Random(300, 700)(rng);

        auto boid = std::make_shared<SimpleEnemy>(x, y);

        float isHunter = Random(0, 1)(rng);
        boid->hunter = isHunter > 0.90f ? true : false;
        if( boid->hunter )
        {
            boid->m_circle.setFillColor(sf::Color::Black);
        }
        boids.push_back(boid);
    }
}

void BoidsExampleView::Update(float dt)
{
    //Remove dead boids
    auto iter = std::remove_if(std::begin(boids), std::end(boids), [](const auto& boid)
    {
        return std::static_pointer_cast<SimpleEnemy>(boid)->isDead();
    });
    boids.erase(iter, std::end(boids));

    auto Closeset = [this](IBoid::Ptr me, bool prey)->IBoid::Ptr
    {
        IBoid::Ptr closestTarget = nullptr;
        IBoid::Ptr mosetThreating = nullptr;
        for(auto boid : boids)
        {
            if( false == std::static_pointer_cast<SimpleEnemy>(boid)->hunter)
            {
                if( nullptr == closestTarget )
                {
                    closestTarget = boid;
                }
                else
                {
                    auto d1 = glm::distance(me->location, boid->location);
                    auto d2 = glm::distance(me->location, closestTarget->location);
                    if( d1 < d2 )
                    {
                        closestTarget = boid;
                    }
                }
            }
            else
            {
                if (nullptr == mosetThreating)
                {
                    mosetThreating = boid;
                }
                else
                {
                    auto d1 = glm::distance(me->location, boid->location);
                    auto d2 = glm::distance(me->location, mosetThreating->location);
                    if( d1 < d2 )
                    {
                        mosetThreating = boid;
                    }
                }
            }
        }
        if (prey)
        {
            return closestTarget;
        }
        else
        {
            return mosetThreating;
        }
    };

    for(auto boid : boids)
    {

    }

    for(auto boid : boids)
    {
        auto b = std::static_pointer_cast<SimpleEnemy>(boid);

        if( false == b->hunter )
        {
           SteeringManager::Wander(b);
        }
        else
        {
            auto prey = Closeset(b, true);
            if(nullptr != prey)
            {
                SteeringManager::Pursuit(b, prey);
            }
            else
            {
                SteeringManager::Wander(b);
            }
        }
    }

    //Update boids
    for(auto& boid1 : boids)
    {
        for(auto& boid2 : boids)
        {
            if (&boid1 == &boid2)
            {
                continue;
            }
            collisionMgr.Process(boid1, boid2);
        }
    }
    for(auto& boid : boids)
    {
        auto b = std::static_pointer_cast<SimpleEnemy>(boid);
        if( b->hunter)
        {
            //b->health -= 0.001f;
        }
        boid->Update();
    }
}

void BoidsExampleView::Draw(sf::RenderWindow &window)
{
    window.clear(sf::Color::White);
    for(auto& boid : boids) { std::static_pointer_cast<SimpleEnemy>(boid)->Draw(window); }
}

void BoidsExampleView::Show()
{

}

void BoidsExampleView::Hide()
{

}

SimpleEnemy::SimpleEnemy(int x, int y)
{
    Init(x, y);
}

void SimpleEnemy::Init(int x, int y)
{
    location.x = x;
    location.y = y;
    velocity.x = Random(-1.0f, 1.0f)(rng);
    velocity.y = Random(-1.0f, 1.0f)(rng);

    static const std::vector<sf::Color> pinkyy =
    {
        sf::Color(255,192,203),
        sf::Color(255,182,193),
        sf::Color(255,105,180),
        sf::Color(255,20,147),
        sf::Color(219,112,147),
    };

    sf::CircleShape circle;
    circle.setPosition(x, y);
    auto color = pinkyy.at( Random(0, pinkyy.size())(rng));

    m_circle.setFillColor(color);
    m_circle.setOrigin(radius, radius);
    m_circle.setOutlineColor(sf::Color::Red);
    m_circle.setOutlineThickness(1);
    m_circle.setRadius(radius);
}

void SimpleEnemy::Draw(sf::RenderWindow &window)
{
    auto dest_pos = location + velocity;

    m_circle.setPosition(location.x, location.y);

    double dx = double(location.x - dest_pos.x);
    double dy = double(location.y - dest_pos.y);
    float angle = float(atan2(dy, dx) * 180 / M_PI - 270);

    sf::RectangleShape stering;
    stering.setPosition(location.x, location.y);
    stering.setSize({1, 24});
    stering.setRotation(angle);
    stering.setFillColor(sf::Color::Red);

    auto& font = FontMgr::Get().GetFont();
    sf::Text life;
    life.setString(std::to_string(health));
    life.setFont(font);
    life.setPosition(location.x - 15, location.y - 30);
    life.setScale(0.7f, 0.7f);
    life.setFillColor(sf::Color::Black);

    window.draw(stering);
    window.draw(m_circle);
    //window.draw(life);
}
