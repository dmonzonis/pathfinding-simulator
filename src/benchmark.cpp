#include "benchmark.h"
#include <fstream>
#include <sstream>
#include <cassert>
#include <cmath>
#include "utils.h"

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
    // TODO: Implement
}

void Benchmark::buildCoordsMap()
{
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
        int nodeCount = std::stoi(parts[4]);
        // Discard next two lines as they are comments
        std::getline(file, line);
        std::getline(file, line);
        // Build the map
        std::pair<int, int> coords;
        for (int i = 0; i < nodeCount; ++i)
        {
            std::getline(file, line);
            parts = splitLine(line, delimiter);
            int node = std::stoi(parts[1]);
            coords = std::make_pair(std::stoi(parts[2]),
                                    std::stoi(parts[3]));
            // Add node coords to the map
            nodeToCoords[node] = coords;
        }
    }
    catch (std::exception &ex)
    {
        throw std::runtime_error("Benchmark error");
    }
}

void Benchmark::buildGraph()
{
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

double Benchmark::euclideanDistance(int first, int second)
{
    std::pair<int, int> coordFirst = nodeToCoords[first],
            coordSecond = nodeToCoords[second];
    double latDif = coordFirst.first - coordSecond.first,
            longDif = coordFirst.second - coordSecond.second;
    return std::sqrt(latDif * latDif + longDif * longDif);
}
