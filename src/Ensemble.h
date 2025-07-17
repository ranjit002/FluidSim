#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "CollisionGrid.h"
#include "SFML/Graphics/Color.hpp"

class Ensemble
{
   public:
    Ensemble(int numParticles, float radius);
    void update(float dt);
    void draw(sf::RenderWindow& window);
    void collideBorder();
    void collideParticles();
    void setAcceleration(sf::Vector2f a);

   private:
    std::vector<sf::Vector2f> positions, velocities, accelerations;
    std::vector<float> radii, masses;
    std::vector<sf::Color> colors;
    CollisionGrid collisionGrid;
    float cellSize;

    void handleCollision(size_t i, size_t j);
};
