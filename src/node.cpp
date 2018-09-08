#include "node.h"

Node::Node()
{

}

std::vector<std::pair<Node, double>> Node::getNeighbors()
{
    std::vector<std::pair<Node, double>> result;
    // Iterate over map, adding (Node, double) pairs to the result vector
    for (auto it = neighbors.begin(); it != neighbors.end(); ++it)
    {
        std::pair<Node, double> pair(it->first, it->second);
        result.push_back(pair);
    }
    return result;
}

void Node::connect(Node& node, double weight)
{
    neighbors[node] = weight;
}

void Node::disconnect(Node& node)
{
    neighbors.erase(node);
}
