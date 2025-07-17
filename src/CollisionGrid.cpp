#include "CollisionGrid.h"

#include <omp.h>

CollisionGrid::CollisionGrid(float width, float height, float cellSize)
    : cellSize(cellSize), gridCols(0), gridRows(0)
{
    gridCols = static_cast<int>(width / cellSize) + 1;
    gridRows = static_cast<int>(height / cellSize) + 1;

    grid.resize(gridCols * gridRows);
}

void CollisionGrid::clear()
{
#pragma omp parallel for
    for (size_t i = 0; i < activeCells.size(); ++i)
        grid[activeCells[i]].clear();

    activeCells.clear();
}

void CollisionGrid::populate(const std::vector<sf::Vector2f>& positions)
{
    clear();
    for (size_t i = 0; i < positions.size(); ++i)
    {
        int row = getGridRow(positions[i]);
        int col = getGridCol(positions[i]);

        if (col < 0 || col >= gridCols || row < 0 || row >= gridRows)
            continue;

        size_t cellIndex = getGridIndex(row, col);
        grid[cellIndex].push_back(i);
    }

    for (size_t i = 0; i < grid.size(); ++i)
    {
        if (!grid[i].empty())
            activeCells.push_back(i);
    }
}

const std::vector<size_t>& CollisionGrid::getCell(size_t index) const
{
    return grid[index];
}

const std::vector<size_t>& CollisionGrid::getActiveCells() const
{
    return activeCells;
}

int CollisionGrid::getGridRow(const sf::Vector2f& pos) const
{
    return static_cast<int>(pos.y / cellSize);
}

int CollisionGrid::getGridCol(const sf::Vector2f& pos) const
{
    return static_cast<int>(pos.x / cellSize);
}

size_t CollisionGrid::getGridIndex(int row, int col) const
{
    return row * gridCols + col;
}
