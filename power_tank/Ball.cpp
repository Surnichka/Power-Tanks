#include "Ball.h"
#include "Window.h"

namespace
{
size_t getFreeId()
{
    static size_t id = 0;
    return id++;
}
}

Ball::Ball(glm::vec2 pos, glm::vec2 vel, float r, float m)
{
    Init(pos, vel, r, m);
}

void Ball::Init(glm::vec2 pos, glm::vec2 vel, float r, float m)
{
    id = getFreeId();

    position = pos;
    velocity = vel;
    radius = r;
    mass = m;

    circle.setPosition(pos.x, pos.y);
    circle.setRadius(r);
    circle.setOrigin(r, r);
    circle.setFillColor(sf::Color::White);
    circle.setOutlineThickness(1);
    circle.setOutlineColor(sf::Color::Black);
}

void Ball::Update()
{
    position += velocity;
    if( enteringScreen )
    {
        static const sf::FloatRect windowRect = {0, 0, Window::width, Window::height};
        if(windowRect.contains(position.x, position.y))
        {
            enteringScreen = false;
        }
    }
    else
    {
        float w = Window::width - radius;
        float h = Window::height - radius;

        if( position.x <= 0 || position.x >= w) velocity.x *= (-1);
        if( position.y <= 0 || position.y >= h) velocity.y *= (-1);

        position.x = glm::clamp<float>(position.x, 0, w);
        position.y = glm::clamp<float>(position.y, 0, h);
    }

    //Update the circle for drawing
    circle.setPosition(position.x, position.y);
}

void Ball::Draw(sf::RenderWindow &window)
{
    window.draw(circle);
}

bool Ball::IsCollide(const Ball &ball)
{
    auto distance = glm::distance(position, ball.position);
    float radiusSqrt = powf(radius + ball.radius, 2);
    return distance < radiusSqrt;
}

void Ball::ResolveCollision(Ball &ball)
{

}

//void Ball::ResolveCollision(Ball &ball)
//{
//    glm::vec2 delta = position - ball.position;
//    float d = delta.length();

//    // minimum translation distance to push balls apart after intersecting
//    glm::vec2 mtd = delta * ((radius + ball.radius)-d)/d;


//    // resolve intersection --
//    // inverse mass quantities
//    float im1 = 1 / mass;
//    float im2 = 1 / ball.mass;

//    // push-pull them apart based off their mass
//    position = position + mtd * (im1 / (im1 + im2));
//    ball.position = ball.position - mtd * im2 / (im1 + im2);

//    // impact speed
//    glm::vec2 v = velocity - ball.velocity;
//    float vn = glm::dot(v, glm::normalize(mtd));

//    // sphere intersecting but moving away from each other already
//    if (vn > 0.0f)
//    {
//        return;
//    }

//    float restitution = 0.5f;
//    // collision impulse
//    float i = (-(1.0f + restitution) * vn) / (im1 + im2);
//    glm::vec2 impulse = glm::normalize(mtd) * i;

//    velocity = velocity + impulse * im1;
//    ball.velocity = ball.velocity - impulse * im2;
//}
