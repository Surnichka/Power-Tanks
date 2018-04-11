#pragma once
#include "glm/glm.hpp"

class Boid
{
public:
    Boid() = default;
    Boid(glm::vec2 pos);
    void seek(glm::vec2 target, float slowingRadius = 30.0f);
    void flee(glm::vec2 target);
    void wander();
    void evade(glm::vec2 target_pos, glm::vec2 target_velocity);
    void pursuit(glm::vec2 target_pos, glm::vec2 target_velocity);

    void update();
    void reset();
public: //Temporaray should be private
    glm::vec2 doSeek(glm::vec2 target, float slowingRadius = 0.0f);
    glm::vec2 doFlee(glm::vec2 target);
    glm::vec2 doWander();
    glm::vec2 doEvade(glm::vec2 target_pos, glm::vec2 target_velocity);
    glm::vec2 doPursuit(glm::vec2 target_pos, glm::vec2 target_velocity);

    glm::vec2 m_steering;

    glm::vec2 m_position = {100, 100};
    glm::vec2 m_velocity = {1, 0};

    static constexpr float mass = 3.0f;
    static constexpr float max_speed = 7.0f;

    static constexpr float max_force = 5.0f;

    static constexpr float wander_circle_distance = 5.0f;
    static constexpr float wander_angle_change = 15.0f;
};
