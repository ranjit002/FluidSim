#pragma once
#include <SFML/Graphics.hpp>
#include <random>
#include <omp.h>
#include "globals.h"
#include <iostream>

inline float length(const sf::Vector2f& v);
inline float dot(const sf::Vector2f& a, const sf::Vector2f& b);
inline sf::Vector2f normalize(const sf::Vector2f& v);


struct Ensemble {

    std::vector<sf::Vector2f> positions, velocities, accelerations;
    std::vector<std::vector<size_t>> grid;
    std::vector<size_t> activeCells;
    std::vector<float> radii, masses;
    sf::Color color{64, 164, 223};
    float cellSize;
    int gridCols, gridRows;

    Ensemble(int numParticles, float radius);

    int getGridCol(const sf::Vector2f& position) const;

    int getGridRow(const sf::Vector2f& position) const;

    size_t getGridIndex(int row, int col) const;

    void clearGrid();

    void populateGrid();

    void draw(sf::RenderWindow& window);

    void update(float dt);

    void collideBorder();
        
    void handleCollision(size_t i, size_t j);

    void collideParticles();

    void setAcceleration(sf::Vector2f a);
};