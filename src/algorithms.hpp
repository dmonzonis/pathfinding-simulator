#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <algorithm>
#include <cmath>
#include <functional>
#include <map>
#include <queue>
#include <utility>
#include <vector>
#include "gridgraph.h"
#include "geolocationgraph.h"

template <class Node>
using Heuristic = typename std::function<double(Node, Node)>;

/**
 * Reconstructs the path to a vector with nodes going from start to goal.
 *
 * The input contains the start and goal nodes, as well as a map which for every
 * node, contains the node it came from in the path, in case it has any.
 */
template <typename Node>
std::vector<Node> reconstructPath(Node start,
                                  Node goal,
                                  std::map<Node, Node> &previous)
{
    std::vector<Node> path;
    Node current = goal;
    // Follow the path backwards until we reach the starting node
    while (current != start)
    {
        path.push_back(current);
        current = previous[current];
    }
    path.push_back(start);
    // Reverse so that it starts with start and ends with goal
    std::reverse(path.begin(), path.end());
    return path;
}

/**
 * Compute the path between two nodes using Breadth-first search with early exit.
 */
template <typename Node, typename Graph>
void bfs(Graph *graph,
         Node start,
         Node goal,
         std::map<Node, Node> &previous,
         std::map<Node, double> &costToNode)
{
    std::queue<Node> nodeQueue;
    nodeQueue.push(start);

    previous[start] = start;

    while (!nodeQueue.empty())
    {
        // Get next node to examine
        Node current = nodeQueue.front();
        nodeQueue.pop();

        // Early exit condition
        if (current == goal)
        {
            return;
        }

        // Push unvisited neighbors to queue and update total cost to reach them
        for (Node next : graph->neighbors(current))
        {
            if (previous.find(next) == previous.end())
            {
                double cost = costToNode[current] + graph->getCost(next, current);
                costToNode[next] = cost;
                previous[next] = current;
                nodeQueue.push(next);
            }
        }
    }
}

/**
 * Compute the optimal path between two nodes using Dijkstra's algorithm with early exit.
 */
template <typename Node, typename Graph>
void dijkstra(Graph *graph,
              Node start,
              Node goal,
              std::map<Node, Node> &previous,
              std::map<Node, double> &costToNode)
{
    typedef std::pair<double, Node> queuePair;
    std::priority_queue<queuePair, std::vector<queuePair>,
            std::greater<queuePair>> nodeQueue;
    nodeQueue.emplace(0, start);

    previous[start] = start;

    while (!nodeQueue.empty())
    {
        // Get next node to examine
        Node current = nodeQueue.top().second;
        nodeQueue.pop();

        // Early exit condition
        if (current == goal)
        {
            return;
        }

        // Push unvisited neighbors to queue
        for (Node next : graph->neighbors(current))
        {
            double cost = costToNode[current] + graph->getCost(next, current);
            // Also consider visited nodes which would have a lesser cost from this new path
            if (previous.find(next) == previous.end()
                    || cost < costToNode[next])
            {
                costToNode[next] = cost;
                previous[next] = current;
                nodeQueue.emplace(cost, next);
            }
        }
    }
}

/**
 * @brief Compute the Manhattan distance between two tiles.
 */
inline double manhattanDistance(Tile a, Tile b)
{
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

/**
 * @brief Compute the Euclidean distance between two tiles.
 */
inline double euclideanDistance(Tile a, Tile b)
{
    return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
}

/**
 * @brief Compute the Chebyshev distance between two tiles.
 */
inline double chebyshevDistance(Tile a, Tile b)
{
    return std::max(std::abs(a.x - b.x), std::abs(a.y - b.y));
}

/**
 * @brief Compute the Chebyshev distance between two tiles.
 */
inline double octileDistance(Tile a, Tile b)
{
    double dx = std::abs(a.x - b.x);
    double dy = std::abs(a.y - b.y);
    return std::sqrt(2) * std::min(dx, dy) + std::abs(dx - dy);
}

inline double euclideanDistance3D(Geolocation a, Geolocation b)
{
    double dx = a.x - b.x,
            dy = a.y - b.y,
            dz = a.z - b.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

/**
 * @brief Find the distance along a sphere between two points on it using the Haversine formula.
 */
inline double haversineDistance(Geolocation a, Geolocation b)
{
    double lat1 = (a.latitude / 1e6) * M_PI / 180,
            lat2 = (b.latitude / 1e6) * M_PI / 180,
            long1 = (a.longitude / 1e6) * M_PI / 180,
            long2 = (b.longitude / 1e6) * M_PI / 180;
    double sinLat = std::sin((lat2 - lat1) / 2);
    double sinLong = std::sin((long2 - long1) / 2);
    double asinArgument = std::sqrt(
                sinLat * sinLat + std::cos(lat1) * std::cos(lat2) * sinLong * sinLong
                );
    return 2 * a.radius * std::asin(asinArgument);
}

/**
 * Compute the optimal path between two nodes using the A* algorithm with early exit.
 */
template <typename Node, typename Graph>
void aStar(Graph *graph,
           Node start,
           Node goal,
           std::map<Node, Node> &previous,
           std::map<Node, double> &costToNode,
           Heuristic<Node> heuristic)

{
    typedef std::pair<double, Node> queuePair;
    std::priority_queue<queuePair, std::vector<queuePair>,
            std::greater<queuePair>> nodeQueue;
    nodeQueue.emplace(0, start);

    previous[start] = start;

    while (!nodeQueue.empty())
    {
        // Get next node to examine
        Node current = nodeQueue.top().second;
        nodeQueue.pop();

        // Early exit condition
        if (current == goal)
        {
            return;
        }

        // Push unvisited neighbors to queue
        for (Node next : graph->neighbors(current))
        {
            double cost = costToNode[current] + graph->getCost(next, current);
            // Also consider visited nodes which would have a lesser cost from this new path
            if (previous.find(next) == previous.end()
                    || cost < costToNode[next])
            {
                costToNode[next] = cost;
                double priority = cost + heuristic(next, goal);
                previous[next] = current;
                nodeQueue.emplace(priority, next);
            }
        }
    }
}

/**
 * Compute the path between two nodes using the greedy best-first search algorithm with early exit.
 */
template <typename Node, typename Graph>
void greedyBestFirstSearch(Graph *graph,
                           Node start,
                           Node goal,
                           std::map<Node, Node> &previous,
                           std::map<Node, double> &costToNode,
                           Heuristic<Node> heuristic)
{
    typedef std::pair<double, Node> queuePair;
    std::priority_queue<queuePair, std::vector<queuePair>,
            std::greater<queuePair>> nodeQueue;
    nodeQueue.emplace(0, start);

    previous[start] = start;

    while (!nodeQueue.empty())
    {
        // Get next node to examine
        Node current = nodeQueue.top().second;
        nodeQueue.pop();

        // Early exit condition
        if (current == goal)
        {
            return;
        }

        // Push unvisited neighbors to queue
        for (Node next : graph->neighbors(current))
        {
            double cost = costToNode[current] + graph->getCost(next, current);
            // Also consider visited nodes which would have a lesser cost from this new path
            if (previous.find(next) == previous.end())
            {
                costToNode[next] = cost;
                double priority = heuristic(next, goal);
                previous[next] = current;
                nodeQueue.emplace(priority, next);
            }
        }
    }
}

#endif // ALGORITMS_H
