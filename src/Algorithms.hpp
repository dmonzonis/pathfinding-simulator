#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <algorithm>
#include <map>
#include <queue>
#include <utility>
#include <vector>
#include "Graph.h"

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

template <typename Node, typename Graph>
void bfs(Graph graph,
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
        for (Node next : graph.neighbors(current))
        {
            if (previous.find(next) == previous.end())
            {
                double cost = costToNode[current] + graph.getCost(next);
                costToNode[next] = cost;
                previous[next] = current;
                nodeQueue.push(next);
            }
        }
    }
}

template <typename Node, typename Graph>
void dijkstra(Graph graph,
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
        for (Node next : graph.neighbors(current))
        {
            double cost = costToNode[current] + graph.getCost(next);
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

inline double manhattanDistance(Tile a, Tile b)
{
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

template <typename Node, typename Graph>
void aStar(Graph graph,
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
        for (Node next : graph.neighbors(current))
        {
            double cost = costToNode[current] + graph.getCost(next);
            // Also consider visited nodes which would have a lesser cost from this new path
            if (previous.find(next) == previous.end()
                    || cost < costToNode[next])
            {
                costToNode[next] = cost;
                // TODO: Allow for use of different heuristics
                double priority = cost + manhattanDistance(next, goal);
                previous[next] = current;
                nodeQueue.emplace(priority, next);
            }
        }
    }
}

#endif // ALGORITMS_H
