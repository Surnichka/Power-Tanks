#pragma once
#include "IView.h"
#include "SFML/Graphics/RectangleShape.hpp"
#include "../boids/DumbEnemy.h"
#include "../boid_manager/SteeringManager.h"
#include "../boid_manager/CollisionManager.h"

class TestView : public IView
{
public:
    void Init() final override;
    void Update(float dt) final override;
    void Draw(sf::RenderWindow& window) final override;
    void Show() final override;
    void Hide() final override;
private:
    CollisionManager colMgr;
    std::vector<DumbBoid::Ptr> dumbies;
};