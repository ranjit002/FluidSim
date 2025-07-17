#include "Ensemble.h"

#include <algorithm>

#include "Globals.h"
#include "Initialiser.h"
#include "Utility.h"

Ensemble::Ensemble(int numParticles, float radius)
{
    float mass = 1.0f;

    positions.reserve(numParticles);
    velocities.reserve(numParticles);
    accelerations.reserve(numParticles);
    radii.reserve(numParticles);
    masses.reserve(numParticles);

    initialiseParticles(numParticles,
        radius,
        positions,
        velocities,
        accelerations,
        radii,
        masses);

    float maxRadius = *std::max_element(radii.begin(), radii.end());
    cellSize = 2.0f * maxRadius;

    gridCols = static_cast<int>(WINDOW_WIDTH / cellSize) + 1;
    gridRows = static_cast<int>(WINDOW_HEIGHT / cellSize) + 1;

    grid.resize(gridCols * gridRows);
    populateGrid();
}

int Ensemble::getGridCol(const sf::Vector2f& position) const
{
    return static_cast<int>(position.x / cellSize);
}
int Ensemble::getGridRow(const sf::Vector2f& position) const
{
    return static_cast<int>(position.y / cellSize);
}
size_t Ensemble::getGridIndex(int row, int col) const
{
    return row * gridCols + col;
}

void Ensemble::clearGrid()
{
#pragma omp parallel for
    for (size_t i = 0; i < activeCells.size(); ++i)
        grid[activeCells[i]].clear();
}

void Ensemble::populateGrid()
{
    clearGrid();
    activeCells.clear();

    for (size_t i = 0; i < positions.size(); ++i)
    {
        int row = getGridRow(positions[i]);
        int col = getGridCol(positions[i]);
        if (col < 0 || col >= gridCols || row < 0 || row >= gridRows)
            continue;
        size_t idx = getGridIndex(row, col);
        grid[idx].push_back(i);
    }
    for (size_t i = 0; i < grid.size(); ++i)
        if (!grid[i].empty())
            activeCells.push_back(i);
}

void Ensemble::update(float dt)
{
#pragma omp parallel for
    for (size_t i = 0; i < positions.size(); ++i)
    {
        positions[i] += velocities[i] * dt + 0.5f * accelerations[i] * dt * dt;
        velocities[i] += 0.5f * accelerations[i] * dt;
    }
    populateGrid();
}

void Ensemble::collideBorder()
{
#pragma omp parallel for
    for (size_t i = 0; i < positions.size(); ++i)
    {
        auto& p = positions[i];
        auto& v = velocities[i];
        float r = radii[i];
        if (p.x < r || p.x > WINDOW_WIDTH - r)
        {
            p.x = std::clamp(p.x, r, WINDOW_WIDTH - r);
            v.x *= (COLLISION_DAMPING - 1.f);
        }
        if (p.y < r || p.y > WINDOW_HEIGHT - r)
        {
            p.y = std::clamp(p.y, r, WINDOW_HEIGHT - r);
            v.y *= (COLLISION_DAMPING - 1.f);
        }
    }
}

void Ensemble::handleCollision(size_t i, size_t j)
{
    auto& posA = positions[i];
    auto& posB = positions[j];
    float rA = radii[i], rB = radii[j];
    float mA = masses[i], mB = masses[j];
    sf::Vector2f delta = posB - posA;
    float dist = length(delta);
    float minDist = rA + rB;
    const float tol = 0.1f;
    if (dist > 0.f && dist < minDist + tol)
    {
        sf::Vector2f normal = delta / dist;
        sf::Vector2f rel = velocities[j] - velocities[i];
        float vn = dot(rel, normal);
        if (vn > 0)
            return;
        float jmag = (2.f * vn) / (mA + mB);
        sf::Vector2f imp = normal * jmag;
        velocities[i] += imp * (1.f / mA);
        velocities[j] -= imp * (1.f / mB);
        sf::Vector2f corr = normal * ((minDist - dist) * 0.5f);
        positions[i] -= corr;
        positions[j] += corr;
    }
}

void Ensemble::collideParticles()
{
    const int dr[] = {0, 1, 1, 1};
    const int dc[] = {1, 1, 0, -1};
#pragma omp parallel for schedule(dynamic)
    for (size_t ci = 0; ci < activeCells.size(); ++ci)
    {
        int idx = activeCells[ci];
        int row = idx / gridCols;
        int col = idx % gridCols;
        const auto& cell = grid[idx];
        for (size_t a = 0; a < cell.size(); ++a)
            for (size_t b = a + 1; b < cell.size(); ++b)
                handleCollision(cell[a], cell[b]);
        for (int k = 0; k < 4; ++k)
        {
            const auto& neigh = grid[getGridIndex(row + dr[k], col + dc[k])];

            for (auto i : cell)
                for (auto j : neigh) handleCollision(i, j);
        }
    }
}

void Ensemble::draw(sf::RenderWindow& window)
{
    sf::CircleShape shape;
    shape.setFillColor(color);
    for (size_t i = 0; i < positions.size(); ++i)
    {
        shape.setRadius(radii[i]);
        shape.setPosition(positions[i]);
        window.draw(shape);
    }
}

void Ensemble::setAcceleration(sf::Vector2f a)
{
    std::fill(accelerations.begin(), accelerations.end(), a);
}