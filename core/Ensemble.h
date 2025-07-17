#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "CollisionGrid.h"

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
    CollisionGrid collisionGrid;
    float cellSize;
    sf::Color color = sf::Color::White;

    void handleCollision(size_t i, size_t j);
};
