#pragma once
#include "IView.h"
#include "SFML/Graphics.hpp"
#include "../boid_manager/CollisionManager.h"
#include "../boid_manager/SteeringManager.h"
#include "../boid_manager/IBoid.h"

struct SimpleEnemy : public IBoid
{
public:
    using Ptr = std::shared_ptr<SimpleEnemy>;

    SimpleEnemy() = default;
    SimpleEnemy(int x, int y);
    void Init(int x, int y);
    void Draw(sf::RenderWindow& window);
    sf::CircleShape m_circle;

    bool hurt(){health = std::max<float>(0.0f, health-1.0f);}
    bool isDead() const { return health == 0.0f; }

    int health = 10;
    bool hunter = false;
};

class BoidsExampleView : public IView
{
public:
    void Init() final override;
    void Update(float dt) final override;
    void Draw(sf::RenderWindow& window) final override;
    void Show() final override;
    void Hide() final override;
private:
    std::vector<IBoid::Ptr> boids;
    CollisionManager collisionMgr;
};
