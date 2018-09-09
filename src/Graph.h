#ifndef GRAPH_H
#define GRAPH_H

#include <map>
#include <vector>

template <typename T>
class Graph
{
public:
    Graph() = default;
    virtual ~Graph() = default;
    virtual std::vector<T> neighbors(T id) = 0;
};

/**
 * @brief A SimpleGraph is a weighteddirected graph in which nodes are
 * identified by integers.
 */
class SimpleGraph : public Graph<int>
{
public:
    using Graph = Graph<int>;

    /**
     * @brief Default constructor.
     *
     * Creates a graph with no nodes.
     */
    SimpleGraph();
    ~SimpleGraph();

    /**
     * @brief Return a list of all the nodes that can be accessed from this node.
     */
    std::vector<int> neighbors(int id);

private:
    // TODO: Add weights to edges
    std::map<int, std::vector<int>> edges;
};

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
     * @return The cost or weight of a tile in the grid, which is the cost of moving
     * to that tile from an adjacent one.
     */
    double getCost(Tile tile);

    /**
     * @brief Sets the cost of a given tile in the grid.
     */
    void setCost(Tile tile, double cost);

    /**
     * @brief Sets whether or not diagonal tile movement is allowed.
     */
    void setDiagonalAllowed(bool allowed);

private:
    int left, top, right, bottom;  // Bounds for x and y coordinates
    std::map<Tile, double> costs;
    bool diagonalAllowed;
};

#endif // GRAPH_H
