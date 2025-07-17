// Initialiser.h
#pragma once

#include <SFML/System.hpp>
#include <vector>

#include "SFML/Graphics/Color.hpp"

void initialiseParticles(int numParticles,
    float radius,
    std::vector<sf::Vector2f>& positions,
    std::vector<sf::Vector2f>& velocities,
    std::vector<sf::Vector2f>& accelerations,
    std::vector<float>& radii,
    std::vector<float>& masses,
    std::vector<sf::Color>& colors);