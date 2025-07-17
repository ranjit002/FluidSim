#include "Initialiser.h"

#include <random>

#include "globals.h"

void initialiseParticles(int numParticles,
    float radius,
    std::vector<sf::Vector2f>& positions,
    std::vector<sf::Vector2f>& velocities,
    std::vector<sf::Vector2f>& accelerations,
    std::vector<float>& radii,
    std::vector<float>& masses)
{
    float mass = 1.0f;
    std::random_device rd;
    std::mt19937 gen(rd());

    float w = 5.0f;
    std::uniform_real_distribution<float> distPosX(
        WINDOW_WIDTH / 2.0f - w, WINDOW_WIDTH / 2.0f + w);
    std::uniform_real_distribution<float> distPosY(
        0.1f * WINDOW_HEIGHT, 0.2f * WINDOW_HEIGHT);
    std::uniform_real_distribution<float> distVel(-50.0f, 50.0f);

    for (int i = 0; i < numParticles; ++i)
    {
        positions.emplace_back(distPosX(gen), distPosY(gen));
        velocities.emplace_back(distVel(gen), distVel(gen));
        accelerations.emplace_back(0.f, 0.f);
        radii.emplace_back(radius);
        masses.emplace_back(mass);
    }
}