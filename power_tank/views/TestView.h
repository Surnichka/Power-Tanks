#pragma once
#include "IView.h"
#include "SFML/Graphics/RectangleShape.hpp"
#include "../boids/DumbEnemy.h"
#include "../boid_manager/SteeringManager.h"
#include "../boid_manager/CollisionManager.h"
#include "../particles/ParticleSystem.h"
#include "../utils/ProgressBar.h"
class TestView : public IView
{
public:
    TestView();
    void Init() final override;
    void Update(float dt) final override;
    void Draw(sf::RenderWindow& window) final override;
    void Show() final override;
    void Hide() final override;
private:
    ParticleSystem m_particleSystem;
    ProgressBar bar1;
    ProgressBar bar2;
    ProgressBar bar3;
    ProgressBar bar4;
};

