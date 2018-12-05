#include "benchmark.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cmath>
#include <ctime>
#include "algorithms.hpp"

Benchmark::Benchmark(std::string filename, int startNode, int goalNode)
    : filename(filename)
    , startNode(startNode)
    , goalNode(goalNode)
{
    buildCoordsMap();
    buildGraph();
}

void Benchmark::run()
{
    unsigned long aStarExpandedNodes, dijkstraExpandedNodes, aStarSphericalExpandedNodes;
    double aStarTime, dijkstraTime, aStarSphericalTime;
    std::clock_t timeBegin, timeEnd;
    std::map<int, int> previous;
    std::map<int, double> costToNode;
    Heuristic<int> heuristic;

    // Run Dijkstra benchmark
    timeBegin = std::clock();
    dijkstra(&graph, startNode, goalNode, previous, costToNode);
    timeEnd = std::clock();
    dijkstraExpandedNodes = costToNode.size();
    dijkstraTime = double(timeEnd - timeBegin) / CLOCKS_PER_SEC;

    // Reset structures
    costToNode.clear();
    previous.clear();

    // Run A* benchmark with euclidean distance
    heuristic = std::bind(&Benchmark::linearDistance,
                                         this,
                                         std::placeholders::_1,
                                         std::placeholders::_2);
    std::cout << "2->48 (linear) = " << heuristic(2, 48) << std::endl;
    timeBegin = std::clock();
    aStar(&graph, startNode, goalNode, previous, costToNode, heuristic);
    timeEnd = std::clock();
    aStarExpandedNodes = costToNode.size();
    aStarTime = double(timeEnd - timeBegin) / CLOCKS_PER_SEC;

    // Reset structures
    costToNode.clear();
    previous.clear();

    // Run A* benchmark with spherical distance
    heuristic = std::bind(&Benchmark::sphericalDistance,
                                         this,
                                         std::placeholders::_1,
                                         std::placeholders::_2);
    std::cout << "2->48 (spherical) = " << heuristic(2, 48) << std::endl;
    timeBegin = std::clock();
    aStar(&graph, startNode, goalNode, previous, costToNode, heuristic);
    timeEnd = std::clock();
    aStarSphericalExpandedNodes = costToNode.size();
    aStarSphericalTime = double(timeEnd - timeBegin) / CLOCKS_PER_SEC;

    // Show results
    std::cout << "Dijkstra\n----------\nExpanded nodes: " << dijkstraExpandedNodes
              << "\nTime elapsed: " << dijkstraTime << std::endl;
    std::cout << std::endl;
    std::cout << "A* (Linear distance)\n----------\nExpanded nodes: " << aStarExpandedNodes
              << "\nTime elapsed: " << aStarTime << std::endl;
    std::cout << "Total time lookup: " << totalTimeLookup << std::endl;
    std::cout << "Total time calc: " << totalTimeCalc << std::endl;
    std::cout << std::endl;
    std::cout << "A* (Spherical distance)\n----------\nExpanded nodes: " << aStarSphericalExpandedNodes
              << "\nTime elapsed: " << aStarSphericalTime << std::endl;
}

void Benchmark::buildCoordsMap()
{
    std::cout << "Building coordinate map..." << std::endl;
    try
    {
        const std::string delimiter = " ";
        std::ifstream file(filename + ".co");
        if (!file.is_open())
        {
            throw std::runtime_error("Error opening file");
        }
        std::string line;
        std::vector<std::string> parts;

        // Search for the specification line
        while (parts.empty() || parts[0] != "p")
        {
            std::getline(file, line);
            parts = splitLine(line, delimiter);
        }
        // Get number of nodes
        numNodes = std::stoi(parts[4]);
        // Discard next two lines as they are comments
        std::getline(file, line);
        std::getline(file, line);
        // Build the map
        Coord coord;
        for (int i = 0; i < numNodes; ++i)
        {
            std::getline(file, line);
            parts = splitLine(line, delimiter);
            int node = std::stoi(parts[1]);
            double latitude = std::stoi(parts[2]),
                    longitude = std::stoi(parts[3]);
            double latRads = (latitude / 1e6) * M_PI / 180,
                    longRads = (longitude / 1e6) * M_PI / 180;
            coord = std::make_pair(latRads, longRads);
            // Add node coords to the map
            nodeToCoords[node] = coord;
        }
    }
    catch (std::exception &ex)
    {
        throw std::runtime_error("Benchmark error");
    }
}

void Benchmark::buildGraph()
{
    std::cout << "Building graph..." << std::endl;
    try
    {
        const std::string delimiter = " ";
        std::ifstream file(filename + ".gr");
        if (!file.is_open())
        {
            throw std::runtime_error("Error opening file");
        }
        std::string line;
        std::vector<std::string> parts;

        // Search for the specification line
        while (parts.empty() || parts[0] != "p")
        {
            std::getline(file, line);
            parts = splitLine(line, delimiter);
        }
        // Get number of edges
        int edgeCount = std::stoi(parts[3]);
        // Discard next two lines as they are comments
        std::getline(file, line);
        std::getline(file, line);
        // Build the graph
        for (int i = 0; i < edgeCount; ++i)
        {
            std::getline(file, line);
            parts = splitLine(line, delimiter);
            int node1 = std::stoi(parts[1]);
            int node2 = std::stoi(parts[2]);
            double weight = std::stod(parts[3]);
            // Add node to the graph
            graph.addEdge(node1, node2, weight);
        }
    }
    catch (std::exception &ex)
    {
        throw std::runtime_error("Benchmark error");
    }
}

double Benchmark::linearDistance(int first, int second)
{
    Coord coordFirst = nodeToCoords[first],
            coordSecond = nodeToCoords[second];
    double latDif = coordFirst.first - coordSecond.first,
            longDif = coordFirst.second - coordSecond.second;
    double cosLatMidpoint = std::cos(0.5 * (coordFirst.first + coordSecond.first));
    return earthRadius * std::sqrt(
                latDif * latDif + cosLatMidpoint * cosLatMidpoint * longDif * longDif
                );
}

double Benchmark::sphericalDistance(int first, int second)
{
    Coord coordFirst = nodeToCoords[first],
            coordSecond = nodeToCoords[second];
    return haversineDistance(coordFirst, coordSecond, earthRadius);
}
