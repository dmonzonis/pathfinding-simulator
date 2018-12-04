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
 * @brief A SimpleGraph is a positively weighted directed graph in which nodes are
 * identified by integers.
 *
 * The edges are unique, so there can't be more than one edge
 * going from one node to another, but can be bidirectional and not necessarily have the
 * same weight, so the edge from A to B can have one weight, and the node from B to A
 * a different weight, or there can be an edge from A to B but not one from B to A.
 */
class SimpleGraph : public Graph<int>
{
public:
    typedef std::map<int, double> Edges;
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
    std::vector<int> neighbors(int node);

    /**
     * @brief Checks if the node exists in the graph.
     */
    bool exists(int node);

    /**
     * @brief Checks if an edge from node1 to node2 exists in the graph.
     */
    bool exists(int node1, int node2);

    /**
     * @return The cost of the edge in the graph groing from node1 to node2.
     *
     * If there's no edge from the start to goal nodes, it returns -1, meaning an infinite
     * cost.
     */
    double getCost(int node1, int node2);

    /**
     * @brief Adds a new node with no connections.
     *
     * If the node already exists, does nothing.
     */
    void addNode(int node);

    /**
     * @brief Adds a connection, or edge, between two nodes with the given cost.
     *
     * If any of the nodes don't exist in the graph, they're automatically added.
     * If the edge already exists, it updates its cost.
     */
    void addEdge(int node1, int node2, double cost);

private:
    std::map<int, Edges> _edges;
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

#endif // GRAPH_H
