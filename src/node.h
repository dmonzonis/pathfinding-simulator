#ifndef NODE_H
#define NODE_H

#include <map>
#include <vector>

/**
 * @brief Represents a node from a graph with weighted edges.
 *
 * Contains information about its neighbor nodes (the nodes that can be accessed from
 * this node), and the weights of those connections.
 */
class Node
{
public:
    Node();
    std::vector<std::pair<Node, double>> getNeighbors();
    void connect(Node& node, double weight);
    void disconnect(Node& node);

private:
    std::map<Node, double> neighbors;
};

#endif // NODE_H
