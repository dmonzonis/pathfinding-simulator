#include "benchmark.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cmath>
#include <ctime>
#include "algorithms.hpp"

Benchmark::Benchmark(std::string filename, int startNodeId, int goalNodeId)
    : filename(filename)
    , startNodeId(startNodeId)
    , goalNodeId(goalNodeId)
{
    buildCoordsMap();
    buildGraph();
}

void Benchmark::run()
{
    unsigned long aStarExpandedNodes, dijkstraExpandedNodes, aStarSphericalExpandedNodes;
    double aStarTime, dijkstraTime, aStarSphericalTime;
    std::clock_t timeBegin, timeEnd;
    std::map<Geolocation, Geolocation> previous;
    std::map<Geolocation, double> costToNode;
    Heuristic<Geolocation> heuristic;
    Geolocation startNode = mapIdToGeolocation[startNodeId];
    Geolocation goalNode = mapIdToGeolocation[goalNodeId];

    // Run Dijkstra benchmark
    timeBegin = std::clock();
    dijkstra(&graph, startNode, goalNode, previous, costToNode);
    timeEnd = std::clock();
    dijkstraExpandedNodes = costToNode.size();
    dijkstraTime = double(timeEnd - timeBegin) / CLOCKS_PER_SEC;

    // Reset structures
    costToNode.clear();
    previous.clear();

    heuristic = euclideanDistance3D;
//    std::cout << "2->48 (linear) = " << heuristic(2, 48) << std::endl;
    timeBegin = std::clock();
    aStar(&graph, startNode, goalNode, previous, costToNode, heuristic);
    timeEnd = std::clock();
    aStarExpandedNodes = costToNode.size();
    aStarTime = double(timeEnd - timeBegin) / CLOCKS_PER_SEC;

    // Reset structures
    costToNode.clear();
    previous.clear();

    // Show results
    std::cout << "Dijkstra\n----------\nExpanded nodes: " << dijkstraExpandedNodes
              << "\nTime elapsed: " << dijkstraTime << std::endl;
    std::cout << std::endl;
    std::cout << "A* (Linear distance)\n----------\nExpanded nodes: " << aStarExpandedNodes
              << "\nTime elapsed: " << aStarTime << std::endl;
    std::cout << std::endl;
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
        for (int i = 0; i < numNodes; ++i)
        {
            std::getline(file, line);
            parts = splitLine(line, delimiter);
            int id = std::stoi(parts[1]);
            int latitude = std::stoi(parts[2]),
                    longitude = std::stoi(parts[3]);
            Geolocation node{id, latitude, longitude};
            node.computeCartesianCoordinates();
            // Add node to the map
            mapIdToGeolocation[id] = node;
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
            int id1 = std::stoi(parts[1]);
            int id2 = std::stoi(parts[2]);
            double weight = std::stod(parts[3]);
            // Add node to the graph
            Geolocation node1 = mapIdToGeolocation[id1];
            Geolocation node2 = mapIdToGeolocation[id2];
            graph.addEdge(node1, node2, weight);
        }
    }
    catch (std::exception &ex)
    {
        throw std::runtime_error("Benchmark error");
    }
}

