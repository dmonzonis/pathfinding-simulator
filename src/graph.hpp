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
 * @brief A SimpleGraph is a positively weighted directed graph.
 *
 * The edges are unique, so there can't be more than one edge
 * going from one node to another, but can be bidirectional and not necessarily have the
 * same weight, so the edge from A to B can have one weight, and the node from B to A
 * a different weight, or there can be an edge from A to B but not one from B to A.
 */
template <typename T>
class SimpleGraph : public Graph<T>
{
public:
    typedef std::map<T, double> Edges;

    /**
     * @brief Default constructor.
     *
     * Creates a graph with no nodes.
     */
    SimpleGraph() : Graph<T>() {}

    /**
     * @brief Return a list of all the nodes that can be accessed from this node.
     */
    virtual std::vector<T> neighbors(T node)
    {
        std::vector<T> result;
        Edges nodeEdges = _edges[node];
        for (auto it = nodeEdges.begin(); it != nodeEdges.end(); ++it)
        {
            // Push back only the identifiers
            result.push_back(it->first);
        }
        return result;
    }

    /**
     * @brief Checks if the node exists in the graph.
     */
    virtual bool exists(T node)
    {
        auto it = _edges.find(node);
        return it != _edges.end();
    }

    /**
     * @brief Checks if an edge from node1 to node2 exists in the graph.
     */
    virtual bool exists(T node1, T node2)
    {
        if (!exists(node1) || !exists(node2))
        {
            return false;
        }
        Edges node1Edges = _edges[node1];
        auto it = node1Edges.find(node2);
        return it != node1Edges.end();
    }

    /**
     * @return The cost of the edge in the graph groing from node1 to node2.
     *
     * If there's no edge from the start to goal nodes, it returns -1, meaning an infinite
     * cost.
     */
    virtual double getCost(T node2, T node1)
    {
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

    /**
     * @brief Adds a new node with no connections.
     *
     * If the node already exists, does nothing.
     */
    virtual void addNode(T node)
    {
        // Add an empty Edges map
        _edges[node] = Edges();
    }

    /**
     * @brief Adds a connection, or edge, between two nodes with the given cost.
     *
     * If any of the nodes don't exist in the graph, they're automatically added.
     * If the edge already exists, it updates its cost.
     */
    virtual void addEdge(T node1, T node2, double cost)
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

private:
    std::map<T, Edges> _edges;
};

#endif // GRAPH_H
