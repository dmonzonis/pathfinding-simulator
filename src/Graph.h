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

class SimpleGraph : public Graph<int>
{
public:
    using Graph = Graph<int>;
    SimpleGraph();
    ~SimpleGraph();
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

class GridGraph : public Graph<Tile>
{
public:
    static std::array<Tile, 4> DIRS;

public:
    using Graph = Graph<Tile>;
    GridGraph(int left, int top, int width, int height);
    bool isOutOfBounds(Tile tile);
    bool isWall(Tile tile);
    std::vector<Tile> neighbors(Tile tile);
    double getCost(Tile tile);
    void setCost(Tile tile, double cost);

private:
    int left, top, right, bottom;  // Bounds for x and y coordinates
    std::map<Tile, double> costs;
};


#endif // GRAPH_H
