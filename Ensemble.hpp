#pragma once
#include <SFML/Graphics.hpp>

#include "globals.hpp"
#include <iostream>

inline float length(const sf::Vector2f& v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

inline float dot(const sf::Vector2f& a, const sf::Vector2f& b) {
    return a.x * b.x + a.y * b.y;
}

inline sf::Vector2f normalize(const sf::Vector2f& v) {
    float len = length(v);
    return len > 0.f ? v / len : sf::Vector2f{0.f, 0.f};
}


struct Ensemble {

    std::vector<sf::Vector2f> positions, velocities, accelerations;
    std::vector<float> radii, masses;
    std::vector<std::vector<size_t>> grid;
    sf::Color color{64, 164, 223};
    int gridCols, gridRows;
    float cellSize;

    
    Ensemble(int num_particles) {

        float radius = 3.0f, mass = 1.0f;
        
        // Reserve memory for particle properties
        positions.reserve(num_particles);
        velocities.reserve(num_particles);
        accelerations.reserve(num_particles);
        radii.reserve(num_particles);
        masses.reserve(num_particles);

        // Random setup
        std::random_device rd;
        std::mt19937 gen(rd());

        float w = 5;
        std::uniform_real_distribution<float> distPosX(WINDOW_WIDTH / 2 - w, WINDOW_WIDTH / 2 + w);
        std::uniform_real_distribution<float> distPosY(10 * WINDOW_HEIGHT / 100, 20 * WINDOW_HEIGHT / 100);
        std::uniform_real_distribution<float> distVel(-50.0f, 50.0f);

        for (int i = 0; i < num_particles; ++i) {
            positions.emplace_back(distPosX(gen), distPosY(gen));
            velocities.emplace_back(distVel(gen), distVel(gen));
            accelerations.emplace_back(0.f, 0.f);  // Initially zero
            radii.emplace_back(radius);
            masses.emplace_back(mass);
        }

        // Setup collision grid
        float max_radius = *std::max_element(radii.begin(), radii.end());
        cellSize = 2.0f * max_radius;
        gridCols = static_cast<int>(WINDOW_WIDTH / cellSize) + 1;
        gridRows = static_cast<int>(WINDOW_HEIGHT / cellSize) + 1;
        
        grid.resize(gridCols * gridRows);
        populate_grid();
    }

    int get_grid_col(const sf::Vector2f& position) const {
        return static_cast<int>(position.x / cellSize);
    }

    int get_grid_row(const sf::Vector2f& position) const {
        return static_cast<int>(position.y / cellSize);
    }

    size_t get_grid_index(int col, int row) const {
        return row * gridCols + col;
    }

    void clear_grid() {
        grid.assign(grid.size(), {});
    }

    void populate_grid() {
        clear_grid();

        for (size_t i=0; i < positions.size(); i++) {
            auto& position = positions[i];
            int row = get_grid_row(position);
            int col = get_grid_col(position);
            int cell_index = get_grid_index(row, col);

            if (col >= 0 && col < gridCols && row >= 0 && row < gridRows) {
                grid[cell_index].push_back(i);
            }

        }
    }

    void draw(sf::RenderWindow& window) {
        sf::CircleShape shape;

        for (size_t i = 0; i < positions.size(); ++i) {
            shape.setRadius(radii[i]);
            shape.setFillColor(color);
            shape.setPosition(positions[i]);
            window.draw(shape);
        }
    }

    // Verlet integration
    void update(float dt) {
        for (size_t i = 0; i < positions.size(); ++i) {
            positions[i] += velocities[i] * dt + 0.5f * accelerations[i] * dt * dt;
            velocities[i] += 0.5f * accelerations[i] * dt;
            accelerations[i] = {};  // Reset to zero vector
        }

        populate_grid();
    }

    void border_collision() 
    {
        for (size_t i = 0; i < positions.size(); ++i) {
            auto& position = positions[i];
            auto& velocity = velocities[i];
            const float& radius = radii[i];

    
            // X axis
            if (position.x < radius) {
                position.x = radius;
                velocity.x = -velocity.x * (1.f - COLLISION_DAMPING);
            }
            else if (position.x > WINDOW_WIDTH - radius) {
            position.x = WINDOW_WIDTH - radius;
            velocity.x = -velocity.x * (1.f - COLLISION_DAMPING);
            }
            

            // Y axis
            if (position.y < radius) {
                position.y = radius;
                velocity.y = -velocity.y * (1.f - COLLISION_DAMPING);
            }
            else if (position.y > WINDOW_HEIGHT - radius) {
                position.y = WINDOW_HEIGHT - radius;
                velocity.y = -velocity.y * (1.f - COLLISION_DAMPING);
            }
        }
    }
        
    void handleCollision(size_t i, size_t j) {
        sf::Vector2f& posA = positions[i];
        sf::Vector2f& posB = positions[j];
        sf::Vector2f& velA = velocities[i];
        sf::Vector2f& velB = velocities[j];
        float radiusA = radii[i];
        float radiusB = radii[j];
        float massA = masses[i];
        float massB = masses[j];

        sf::Vector2f separation = posB - posA;
        float dist = length(separation);
        float minDist = radiusA + radiusB;
        float tolerance = 0.1f;

        if (dist > 0.f && dist < minDist + tolerance) {
            sf::Vector2f normal = separation / dist;

            sf::Vector2f relVel = velB - velA;
            float velAlongNormal = dot(relVel, normal);

            if (velAlongNormal > 0.f) return;

            float impulseMag = (2.f * velAlongNormal) / (massA + massB);
            sf::Vector2f impulse = normal * impulseMag;

            velA += impulse * (1.f / massA);
            velB -= impulse * (1.f / massB);

            sf::Vector2f correction = normal * (minDist - dist) * 0.5f;
            posA -= correction;
            posB += correction;
        }
    }


    void collideParticles() {
        // Iterate over cells
        for (size_t row = 0; row < gridRows; row++) {
            for (size_t col = 0; col < gridCols; col++) {
                
                int cell_index = get_grid_index(row, col);
                const auto& cell = grid[cell_index];

                // Iterate over the same cell
                for (size_t i = 0; i < cell.size(); i++) {
                    for (size_t j = i + 1; j < cell.size(); j++) {
                        handleCollision(cell[i], cell[j]);
                    }
                }

                // Iterate over neighouring cells
                for (int drow = -1; drow <= 1 ; drow++) {
                    for (int dcol = -1; dcol <= 1 ; dcol++) {
                        if (drow == 0 && dcol == 0) continue;

                        int neighbour_row = row + drow;
                        int neighbour_col = col + dcol;
                        // Avoid repeat calculations
                        if (neighbour_row < 0 || neighbour_col < 0 || neighbour_row >= gridRows || neighbour_col >= gridCols) continue;

                        int neighbour_index = get_grid_index(neighbour_row, neighbour_col);
                        const auto& neighbour_cell = grid[neighbour_index];
                        
                        for (auto& i : cell) {
                            for (auto& j : neighbour_cell) {
                                handleCollision(i, j);
                            }
                        }
                        
                    }
                }
                                
            }
        }
    }
  
    void set_acceleration(sf::Vector2f a) {
    
        std::fill(accelerations.begin(), accelerations.end(), a);
    }

};