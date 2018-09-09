#include "Graph.h"

SimpleGraph::SimpleGraph()
    : Graph()
{
}

SimpleGraph::~SimpleGraph()
{
}

std::vector<int> SimpleGraph::neighbors(int id)
{
    return edges[id];
}

GridGraph::GridGraph(int left, int top, int width, int height)
    : Graph(),
      left(left),
      top(top),
      right(left + width),
      bottom(top + height),
      diagonalAllowed(false),
      cornerMovementAllowed(false)
{
}

bool GridGraph::isOutOfBounds(Tile tile)
{
    return tile.x < left || tile.x > right
            || tile.y < top || tile.y > bottom;
}

bool GridGraph::isWall(Tile tile)
{
    return getCost(tile) < 0;
}

std::vector<Tile> GridGraph::neighbors(Tile tile)
{
    std::vector<Tile> result;
    std::vector<Tile> dirs{GridGraph::DIRS};
    // If diagonal is allowed, add diagonal directions
    if (diagonalAllowed)
    {
        dirs.insert(dirs.end(),
                    GridGraph::DIAGONAL_DIRS.begin(),
                    GridGraph::DIAGONAL_DIRS.end());
    }
    // For each possible direction, check for adjacent walkable tiles
    for (auto dir : dirs)
    {
        Tile adjacentTile{tile.x + dir.x, tile.y + dir.y};
        if (!isOutOfBounds(adjacentTile) && !isWall(adjacentTile)
                && (!diagonalAllowed || cornerMovementAllowed
                    || !isCornerMovement(tile, dir)))
        {
            // Walkable tile, add to neighbors vector
            result.push_back(adjacentTile);
        }
    }
    return result;
}

double GridGraph::getCost(Tile tile)
{
    // Use 1 as the default cost instead of 0
    auto it = costs.find(tile);
    if (it == costs.end())
    {
        return 1;
    }
    return costs[tile];
}

void GridGraph::setCost(Tile tile, double cost)
{
    // TODO: Throw error if tile is out of bounds
    costs[tile] = cost;
}

void GridGraph::setDiagonalAllowed(bool allowed)
{
    diagonalAllowed = allowed;
}

void GridGraph::setCornerMovementAllowed(bool allowed)
{
    cornerMovementAllowed = allowed;
}

bool GridGraph::isCornerMovement(Tile tile, Tile direction)
{
    Tile corner1{tile.x + direction.x, tile.y};
    Tile corner2{tile.x, tile.y + direction.y};
    return isWall(corner1) || isWall(corner2);
}

std::vector<Tile> GridGraph::DIRS = {
    Tile{1, 0}, Tile{0, -1}, Tile{-1, 0}, Tile{0, 1}
};

std::vector<Tile> GridGraph::DIAGONAL_DIRS = {
    Tile{1, 1}, Tile{-1, -1}, Tile{-1, 1}, Tile{1, -1}
};
