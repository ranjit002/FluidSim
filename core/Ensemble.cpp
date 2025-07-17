#include "Ensemble.h"

#include <algorithm>

#include "Globals.h"
#include "Initialiser.h"
#include "Utility.h"

Ensemble::Ensemble(int numParticles, float radius)
    : cellSize(0.0f),
      collisionGrid(WINDOW_WIDTH,
          WINDOW_HEIGHT,
          1.0f)  // temp init, will set below
{
    positions.reserve(numParticles);
    velocities.reserve(numParticles);
    accelerations.reserve(numParticles);
    radii.reserve(numParticles);
    masses.reserve(numParticles);
    colors.reserve(numParticles);

    initialiseParticles(numParticles,
        radius,
        positions,
        velocities,
        accelerations,
        radii,
        masses,
        colors);

    float maxRadius = *std::max_element(radii.begin(), radii.end());
    cellSize = 2.0f * maxRadius;

    // Reinitialize collision grid with correct cell size
    collisionGrid = CollisionGrid(WINDOW_WIDTH, WINDOW_HEIGHT, cellSize);

    collisionGrid.populate(positions);
}

void Ensemble::update(float dt)
{
#pragma omp parallel for
    for (size_t i = 0; i < positions.size(); ++i)
    {
        positions[i] += velocities[i] * dt + 0.5f * accelerations[i] * dt * dt;
        velocities[i] += 0.5f * accelerations[i] * dt;
    }
    collisionGrid.populate(positions);
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
        sf::Vector2f corr = normal * ((minDist - dist) * 0.7f);
        positions[i] -= corr;
        positions[j] += corr;
    }
}

void Ensemble::collideParticles()
{
    const int dr[] = {0, 1, 1, 1};
    const int dc[] = {1, 1, 0, -1};

    const auto& activeCells = collisionGrid.getActiveCells();

#pragma omp parallel for schedule(dynamic)
    for (size_t ci = 0; ci < activeCells.size(); ++ci)
    {
        int idx = activeCells[ci];
        int row = idx / collisionGrid.getGridCols();
        int col = idx % collisionGrid.getGridCols();
        const auto& cell = collisionGrid.getCell(idx);

        // Collisions within the same cell
        for (size_t a = 0; a < cell.size(); ++a)
            for (size_t b = a + 1; b < cell.size(); ++b)
                handleCollision(cell[a], cell[b]);

        // Collisions with forward neighboring cells
        for (int k = 0; k < 4; ++k)
        {
            int nrow = row + dr[k];
            int ncol = col + dc[k];

            if (nrow < 0 || ncol < 0 || nrow >= collisionGrid.getGridRows() ||
                ncol >= collisionGrid.getGridCols())
                continue;

            const auto& neigh =
                collisionGrid.getCell(collisionGrid.getGridIndex(nrow, ncol));

            for (auto i : cell)
                for (auto j : neigh) handleCollision(i, j);
        }
    }
}

void Ensemble::draw(sf::RenderWindow& window)
{
    sf::CircleShape shape;
    for (size_t i = 0; i < positions.size(); ++i)
    {
        shape.setRadius(radii[i]);
        shape.setPosition(positions[i]);
        shape.setFillColor(colors[i]);
        window.draw(shape);
    }
}

void Ensemble::setAcceleration(sf::Vector2f a)
{
    std::fill(accelerations.begin(), accelerations.end(), a);
}
