#include "Initialiser.h"

#include <random>

#include "Globals.h"

void initialiseParticles(int numParticles,
    float radius,
    std::vector<sf::Vector2f>& positions,
    std::vector<sf::Vector2f>& velocities,
    std::vector<sf::Vector2f>& accelerations,
    std::vector<float>& radii,
    std::vector<float>& masses,
    std::vector<sf::Color>& colors)
{
    float mass = 1.0f;
    std::random_device rd;
    std::mt19937 rng(rd());

    // Spiral burst config
    const sf::Vector2f center(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 5.f);
    std::uniform_real_distribution<float> angleNoise(-0.3f, 0.3f);
    std::uniform_real_distribution<float> speedNoise(-10.f, 10.f);

    for (int i = 0; i < numParticles; ++i)
    {
        float t = static_cast<float>(i) / numParticles;
        float angle = 8 * M_PI * t + angleNoise(rng);
        float radiusOffset = 10.f + 100.f * t;
        sf::Vector2f dir(std::cos(angle), std::sin(angle));
        sf::Vector2f pos = center + dir * radiusOffset;
        sf::Vector2f vel = dir * (100.f + speedNoise(rng));

        // Rainbow fade
        uint8_t r = static_cast<uint8_t>(127 + 127 * std::sin(6.2831f * t + 0));
        uint8_t g = static_cast<uint8_t>(127 + 127 * std::sin(6.2831f * t + 2));
        uint8_t b = static_cast<uint8_t>(127 + 127 * std::sin(6.2831f * t + 4));
        sf::Color color(r, g, b);

        positions.emplace_back(pos);
        velocities.emplace_back(vel);
        accelerations.emplace_back(0.f, 0.f);
        radii.emplace_back(radius);
        masses.emplace_back(mass);
        colors.emplace_back(color);
    }
}
