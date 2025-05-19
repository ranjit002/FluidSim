#include "Ensemble.h"

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


Ensemble::Ensemble(int numParticles, float radius) {

    float mass = 1.0f;
    
    // Reserve memory for particle properties
    positions.reserve(numParticles);
    velocities.reserve(numParticles);
    accelerations.reserve(numParticles);
    radii.reserve(numParticles);
    masses.reserve(numParticles);

    // Random setup
    std::random_device rd;
    std::mt19937 gen(rd());

    float w = 5;
    std::uniform_real_distribution<float> distPosX(WINDOW_WIDTH / 2 - w, WINDOW_WIDTH / 2 + w);
    std::uniform_real_distribution<float> distPosY(10 * WINDOW_HEIGHT / 100, 20 * WINDOW_HEIGHT / 100);
    std::uniform_real_distribution<float> distVel(-50.0f, 50.0f);

    for (int i = 0; i < numParticles; ++i) {
        positions.emplace_back(distPosX(gen), distPosY(gen));
        velocities.emplace_back(distVel(gen), distVel(gen));
        accelerations.emplace_back(0.f, 0.f);  // Initially zero
        radii.emplace_back(radius);
        masses.emplace_back(mass);
    }

    // Setup collision grid
    float maxRadius = *std::max_element(radii.begin(), radii.end());
    cellSize = 2.0f * maxRadius;
    gridCols = static_cast<int>(WINDOW_WIDTH / cellSize) + 1;
    gridRows = static_cast<int>(WINDOW_HEIGHT / cellSize) + 1;
    
    grid.resize(gridCols * gridRows);
    populateGrid();
}


inline int Ensemble::getGridCol(const sf::Vector2f& position) const {
    return static_cast<int>(position.x / cellSize);
}

inline int Ensemble::getGridRow(const sf::Vector2f& position) const {
    return static_cast<int>(position.y / cellSize);
}

inline size_t Ensemble::getGridIndex(int row, int col) const {
    return row * gridCols + col;
}

void Ensemble::clearGrid() {
    for (auto& cell : grid) cell.clear();
}

void Ensemble::populateGrid() {
    clearGrid();

    for (size_t i = 0; i < positions.size(); i++) {
        auto& position = positions[i];
        int row = getGridRow(position);
        int col = getGridCol(position);
        int cellIndex = getGridIndex(row, col);

        if (col >= 0 && col < gridCols && row >= 0 && row < gridRows) {
            grid[cellIndex].push_back(i);
        }
    }

    // build list of non-empty cells
    activeCells.clear();
    for (size_t i = 0; i < grid.size(); ++i) {
        if (!grid[i].empty())
            activeCells.push_back(i);
    }
}

void Ensemble::draw(sf::RenderWindow& window) {
    
    sf::CircleShape shape;
    shape.setFillColor(color);

    for (size_t i = 0; i < positions.size(); ++i) {
        shape.setRadius(radii[i]);
        shape.setPosition(positions[i]);
        window.draw(shape);
    }
}

void Ensemble::update(float dt) {
    #pragma omp parallel for
    for (size_t i = 0; i < positions.size(); ++i) {
        positions[i] += velocities[i] * dt + 0.5f * accelerations[i] * dt * dt;
        velocities[i] += 0.5f * accelerations[i] * dt;
    }

    populateGrid();
}

void Ensemble::collideBorder() {

    #pragma omp parallel for
    for (size_t i = 0; i < positions.size(); ++i) {
        auto& position = positions[i];
        auto& velocity = velocities[i];
        const float& radius = radii[i];

        // X axis
        if (position.x < radius || position.x > WINDOW_WIDTH - radius)
        {
            position.x = (position.x < radius) ? radius : (WINDOW_HEIGHT - radius);
            velocity.x *= (COLLISION_DAMPING - 1.f);
        }
        
        // Y axis
        if (position.y < radius || position.y > WINDOW_HEIGHT - radius)
        {
            position.y = (position.y < radius) ? radius : (WINDOW_HEIGHT - radius);
            velocity.y *= (COLLISION_DAMPING - 1.f);
        }
        
    }
}

void Ensemble::handleCollision(size_t i, size_t j) {

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

void Ensemble::collideParticles() {
    const int dRows[] = { 0, 1, 1, 1 };
    const int dCols[] = { 1, 1, 0, -1 };

    size_t N = activeCells.size();
    #pragma omp parallel for schedule(dynamic)
    for (size_t ci = 0; ci < N; ++ci) {
        size_t cellIndex = activeCells[ci];

        // recover row/col if you need them
        int row = cellIndex / gridCols;
        int col = cellIndex % gridCols;

        const auto& cell = grid[cellIndex];

        // collisions within the cell
        for (size_t a = 0; a < cell.size(); ++a) {
            for (size_t b = a + 1; b < cell.size(); ++b) {
                handleCollision(cell[a], cell[b]);
            }
        }

        // collisions with forward neighbors
        for (int k = 0; k < 4; ++k) {
            int nrow = row + dRows[k];
            int ncol = col + dCols[k];
            if (nrow < 0 || ncol < 0 || nrow >= gridRows || ncol >= gridCols)
                continue;
            size_t nidx = getGridIndex(nrow, ncol);
            const auto& neighbour = grid[nidx];
            if (neighbour.empty()) continue;

            for (auto i : cell) {
                for (auto j : neighbour) {
                    handleCollision(i, j);
                }
            }
        }
    }
}

void Ensemble::setAcceleration(sf::Vector2f a) {
    std::fill(accelerations.begin(), accelerations.end(), a);
}
