#include "graph.h"

SimpleGraph::SimpleGraph()
    : Graph()
{
}

SimpleGraph::~SimpleGraph()
{
}

std::vector<int> SimpleGraph::neighbors(int node)
{
    std::vector<int> result;
    Edges nodeEdges = _edges[node];
    for (auto it = nodeEdges.begin(); it != nodeEdges.end(); ++it)
    {
        // Push back only the identifiers
        result.push_back(it->first);
    }
    return result;
}

bool SimpleGraph::exists(int node)
{
    auto it = _edges.find(node);
    return it != _edges.end();
}

bool SimpleGraph::exists(int node1, int node2)
{
    if (!exists(node1) || !exists(node2))
    {
        return false;
    }
    Edges node1Edges = _edges[node1];
    auto it = node1Edges.find(node2);
    return it != node1Edges.end();
}

double SimpleGraph::getCost(int node1, int node2)
{
    // If the start node doesn't exist, don't bother continuing
    if (!exists(node1))
    {
        return -1;
    }
    // Get edges coming out of node1
    Edges nodeEdges = _edges[node1];
    // If there's no connection to node2, return -1
    auto it = nodeEdges.find(node2);
    if (it == nodeEdges.end())
    {
        return -1;
    }
    return it->second;
}

void SimpleGraph::addNode(int node)
{
    // Add an empty Edges map
    _edges[node] = Edges();
}

void SimpleGraph::addEdge(int node1, int node2, double cost)
{
    // First see if any of the nodes don't exist, and add them if they don't
    if (!exists(node1))
    {
        addNode(node1);
    }
    if (!exists(node2))
    {
        addNode(node2);
    }

    // Add edge from node1 to node2 with the given cost
    _edges[node1][node2] = cost;
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
    return tile.x < left || tile.x >= right
            || tile.y < top || tile.y >= bottom;
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

std::vector<Tile> GridGraph::adjacentTiles(Tile tile)
{
    std::vector<Tile> result;
    // For each possible direction, check for adjacent walkable tiles
    for (auto dir : GridGraph::DIRS)
    {
        Tile adjacentTile{tile.x + dir.x, tile.y + dir.y};
        if (!isOutOfBounds(adjacentTile))
        {
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

std::pair<int, int> GridGraph::getTopLeft() const
{
    return std::make_pair(left, top);
}

int GridGraph::getWidth() const
{
    return right - left;
}

int GridGraph::getHeight() const
{
    return bottom - top;
}
