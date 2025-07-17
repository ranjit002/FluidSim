// CollisionGrid.h
#pragma once

#include <SFML/System/Vector2.hpp>
#include <vector>

class CollisionGrid
{
   public:
    CollisionGrid(float width, float height, float cellSize);

    void clear();
    void populate(const std::vector<sf::Vector2f>& positions);

    const std::vector<size_t>& getCell(size_t index) const;
    const std::vector<size_t>& getActiveCells() const;

    int getGridRow(const sf::Vector2f& pos) const;
    int getGridCol(const sf::Vector2f& pos) const;
    size_t getGridIndex(int row, int col) const;

    int getGridRows() const { return gridRows; }
    int getGridCols() const { return gridCols; }

   private:
    std::vector<std::vector<size_t>> grid;
    std::vector<size_t> activeCells;
    float cellSize;
    int gridRows;
    int gridCols;
};
