#ifndef GRIDGRAPH_H
#define GRIDGRAPH_H

#include "graph.hpp"

typedef struct Tile {
    int x, y;

    // Overload some operators so that tiles can be used as map keys
    bool operator==(const Tile &other) const
    {
        return x == other.x && y == other.y;
    }

    bool operator<(const Tile &other) const
    {
        return x < other.x || (x == other.x && y < other.y);
    }

    bool operator!=(const Tile &other) const
    {
        return x != other.x || y != other.y;
    }
} Tile;

/**
 * @brief A GridGraph is a graph representation of a tiled grid.
 *
 * In a GridGraph, nodes are tiles and each tile is connected to its adjacent
 * tiles in the vertical and horizontal directions.
 */
class GridGraph : public Graph<Tile>
{
public:
    static std::vector<Tile> DIRS;
    static std::vector<Tile> DIAGONAL_DIRS;

public:
    using Graph = Graph<Tile>;

    /**
     * @brief Constructs a grid graph with the given bounds.
     *
     * The resulting graph will be a rectangular array of tiles, with the
     * top-left coordinates given by the parameters, and a given width and
     * height which determine its other bounds.
     *
     * @param left Minimum x value for a tile in the grid.
     * @param top Minimum y value for a tile in the grid.
     * @param width Width of the grid, in tiles.
     * @param height Height of the grid, in tiles.
     */
    GridGraph(int left, int top, int width, int height);

    /**
     * @return true if the tile is outside of the bounds of the grid, false
     * otherwise.
     */
    bool isOutOfBounds(Tile tile);

    /**
     * @return true if the tile is a wall in the grid, which is represented by
     * having a negative weight.
     */
    bool isWall(Tile tile);

    /**
     * @return A list of all adjacent tiles to the given tile, if they're walkable,
     * that is, they're inside the grid's bounds and are not walls.
     */
    std::vector<Tile> neighbors(Tile tile);

    /**
     * @brief Similar to the neighbors method, but also returns wall tiles, and ignores
     * diagonal movement.
     */
    std::vector<Tile> adjacentTiles(Tile tile);

    /**
     * @return The cost or weight of a tile in the grid, which is the cost of moving
     * to that tile from an adjacent one.
     */
    double getCost(Tile tile, Tile previous=Tile{0, 0});

    /**
     * @brief Sets the cost of a given tile in the grid.
     */
    void setCost(Tile tile, double cost);

    /**
     * @brief Sets whether or not diagonal tile movement is allowed.
     */
    void setDiagonalAllowed(bool allowed);

    /**
     * @brief Sets whether or not corner movement is allowed.
     */
    void setCornerMovementAllowed(bool allowed);

    /**
     * @brief Return the tile coordinates of the top-left tile.
     */
    std::pair<int, int> getTopLeft() const;

    /**
     * @return The width of the graph, in tiles.
     */
    int getWidth() const;

    /**
     * @return The height of the graph, in tiles.
     */
    int getHeight() const;

private:
    /**
     * @brief Tests whether moving in the given direction from a tile is a corner movement.
     *
     * A corner movement is defined by moving around a corner tile on one of the sides in a
     * diagonal movement. For example, if moving NE, it would be considered a corner
     * movement if there were a wall tile on the N or E directions.
     */
    bool isCornerMovement(Tile tile, Tile direction);

private:
    int left, top, right, bottom;  // Bounds for x and y coordinates
    std::map<Tile, double> costs;
    bool diagonalAllowed, cornerMovementAllowed;
};

#endif // GRIDGRAPH_H
