// Ensemble.h
#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

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
    std::vector<std::vector<size_t>> grid;
    std::vector<size_t> activeCells;

    float cellSize;
    int gridRows, gridCols;
    sf::Color color = sf::Color::White;

    void populateGrid();
    void clearGrid();
    void handleCollision(size_t i, size_t j);
    int getGridRow(const sf::Vector2f& pos) const;
    int getGridCol(const sf::Vector2f& pos) const;
    size_t getGridIndex(int row, int col) const;
};
