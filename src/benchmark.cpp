#include "benchmark.h"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cmath>
#include <ctime>
#include "algorithms.hpp"
#include "csvencoder.h"

Benchmark::Benchmark(std::string filename)
    : filename(filename),
      gridGraph(nullptr)
{
}

Benchmark::~Benchmark()
{
    if (gridGraph)
        delete gridGraph;
}


void Benchmark::run(int count)
{
    runGridBenchmark(count);

    // Reset result structures
    timesDijkstra.clear();
    timesAstar.clear();
    timesAstarAlt.clear();
    expandedDijkstra.clear();
    expandedAstar.clear();
    expandedAstarAlt.clear();

//    runRoadBenchmark(count);
}

void Benchmark::runGridBenchmark(int count)
{
    // Assume the randomgrid.csv file has been generated
    std::cout << "Loading grid graph..." << std::endl;
    CSVEncoder encoder("randomgrid.csv");
    gridGraph = encoder.loadGridGraph();

    Tile startTile, goalTile;
    srand(time(nullptr));
    for (int i = 1; i <= count; ++i)
    {
        bool success;
        // Repeat until we find a combination of connected start/goal nodes
        do
        {
            // Repeat until we find start/goal nodes that are not walls
            do
            {
                startTile = Tile{-gridGraph->getWidth() / 2 + rand() % gridGraph->getWidth(),
                        -gridGraph->getHeight() / 2 + rand() % gridGraph->getWidth()};
                goalTile = Tile{-gridGraph->getHeight() / 2 + rand() % gridGraph->getWidth(),
                        -gridGraph->getHeight() / 2 + rand() % gridGraph->getHeight()};
            } while (gridGraph->isWall(startTile) || gridGraph->isWall(goalTile));

            std::cout << "Executing benchmark " << i << "/" << count
                      << " with start=(" << startTile.x << "," << startTile.y
                      << ") and goal=(" << goalTile.x << "," << goalTile.y << ")"
                      << std::endl;
            success = runGridSingle(startTile, goalTile);
        } while (!success);

    }
    std::cout << "Running summary..." << std::endl;
    runSummary();
}

void Benchmark::runRoadBenchmark(int count)
{
    // Build needed structures
    buildCoordsMap();
    buildGeolocationGraph();

    // Write header of benchmark results CSV file
    std::ofstream file("benchmark.csv");
    file << "dist,dijNodes,dijTime,A*Nodes,A*Time,A*sphNodes,A*sphTime" << std::endl;

    int startId, goalId;
    srand(time(nullptr));
    for (int i = 1; i <= count; ++i)
    {
        startId = 1 + rand() % numNodes;
        goalId = 1 + rand() % numNodes;

        std::cout << "Executing benchmark " << i << "/" << count << " with start=" << startId
                  << " and goal=" << goalId << std::endl;
        runRoadSingle(startId, goalId);
    }
    std::cout << "Running summary..." << std::endl;
    runSummary();
}

bool Benchmark::runGridSingle(Tile startTile, Tile goalTile)
{
    unsigned long aStarExpandedNodes, dijkstraExpandedNodes;
    double aStarTime, dijkstraTime;
    double optimalDistance;
    std::clock_t timeBegin, timeEnd;
    std::map<Tile, Tile> previous;
    std::map<Tile, double> costToNode;
    Heuristic<Tile> heuristic;

    // Run Dijkstra benchmark
    timeBegin = std::clock();
    dijkstra(gridGraph, startTile, goalTile, previous, costToNode);
    timeEnd = std::clock();
    dijkstraExpandedNodes = costToNode.size();
    dijkstraTime = double(timeEnd - timeBegin) / CLOCKS_PER_SEC;
    optimalDistance = costToNode[goalTile];
    std::cout << "Optimal distance: " << optimalDistance << std::endl;
    if (previous.find(goalTile) == previous.end()
            || optimalDistance == std::numeric_limits<double>::infinity())
    {
        // There doesn't exist a path
        std::cout << "No path found. Repeating with different start/goal tiles." << std::endl;
        return false;
    }

    // Reset structures
    costToNode.clear();
    previous.clear();

    // Run A* benchmark with Manhattan distance
    heuristic = manhattanDistance;
    timeBegin = std::clock();
    aStar(gridGraph, startTile, goalTile, previous, costToNode, heuristic);
    timeEnd = std::clock();
    aStarExpandedNodes = costToNode.size();
    aStarTime = double(timeEnd - timeBegin) / CLOCKS_PER_SEC;

    // Save partial results
    timesDijkstra.push_back(dijkstraTime);
    timesAstar.push_back(aStarTime);
    expandedDijkstra.push_back(dijkstraExpandedNodes);
    expandedAstar.push_back(aStarExpandedNodes);

    return true;
}

void Benchmark::runRoadSingle(int startId, int goalId)
{
    unsigned long aStarExpandedNodes, dijkstraExpandedNodes, aStarAltExpandedNodes;
    double aStarTime, dijkstraTime, aStarAltTime;
    double optimalDistance;
    std::clock_t timeBegin, timeEnd;
    std::map<Geolocation, Geolocation> previous;
    std::map<Geolocation, double> costToNode;
    Heuristic<Geolocation> heuristic;
    Geolocation startNode = mapIdToGeolocation[startId];
    Geolocation goalNode = mapIdToGeolocation[goalId];

    // Run Dijkstra benchmark
    timeBegin = std::clock();
    dijkstra(&geolocationGraph, startNode, goalNode, previous, costToNode);
    timeEnd = std::clock();
    dijkstraExpandedNodes = costToNode.size();
    dijkstraTime = double(timeEnd - timeBegin) / CLOCKS_PER_SEC;
    optimalDistance = costToNode[goalNode];

    // Reset structures
    costToNode.clear();
    previous.clear();

    // Run A* benchmark with linear distance
    heuristic = euclideanDistance3D;
    timeBegin = std::clock();
    aStar(&geolocationGraph, startNode, goalNode, previous, costToNode, heuristic);
    timeEnd = std::clock();
    aStarExpandedNodes = costToNode.size();
    aStarTime = double(timeEnd - timeBegin) / CLOCKS_PER_SEC;

    // Reset structures
    costToNode.clear();
    previous.clear();

    // Run A* benchmark with Haversine distance
    heuristic = haversineDistance;
    timeBegin = std::clock();
    aStar(&geolocationGraph, startNode, goalNode, previous, costToNode, heuristic);
    timeEnd = std::clock();
    aStarAltExpandedNodes = costToNode.size();
    aStarAltTime = double(timeEnd - timeBegin) / CLOCKS_PER_SEC;

    // Save partial results
    timesDijkstra.push_back(dijkstraTime);
    timesAstar.push_back(aStarTime);
    timesAstarAlt.push_back(aStarAltTime);
    expandedDijkstra.push_back(dijkstraExpandedNodes);
    expandedAstar.push_back(aStarExpandedNodes);
    expandedAstarAlt.push_back(aStarAltExpandedNodes);

    // Write partial results to CSV file
    std::ofstream file("benchmark.csv", std::ios_base::app);
    file << optimalDistance << ","
         << dijkstraExpandedNodes << ","
         << dijkstraTime << ","
         << aStarExpandedNodes << ","
         << aStarTime << ","
         << aStarAltExpandedNodes << ","
         << aStarAltTime << std::endl;;
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

void Benchmark::buildGeolocationGraph()
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
            geolocationGraph.addEdge(node1, node2, weight);
        }
    }
    catch (std::exception &ex)
    {
        throw std::runtime_error("Benchmark error");
    }
}

void Benchmark::runSummary()
{
    double dijkstraTotalNodes, aStarTotalNodes, aStarAltTotalNodes;
    double dijkstraTotalTime, aStarTotalTime, aStarAltTotalTime;

    // Compute totals
    dijkstraTotalNodes = std::accumulate(expandedDijkstra.begin(), expandedDijkstra.end(), 0);
    aStarTotalNodes = std::accumulate(expandedAstar.begin(), expandedAstar.end(), 0);
    aStarAltTotalNodes = std::accumulate(expandedAstarAlt.begin(),
                                               expandedAstarAlt.end(),
                                               0);
    dijkstraTotalTime = std::accumulate(timesDijkstra.begin(), timesDijkstra.end(), 0.);
    aStarTotalTime = std::accumulate(timesAstar.begin(), timesAstar.end(), 0.);
    aStarAltTotalTime = std::accumulate(timesAstarAlt.begin(),
                                               timesAstarAlt.end(),
                                               0.);

    // Report summary
    std::cout << "\n###############\nSummary\n###############\n";
    std::cout << "Algorithm\t\tTotal nodes\t\tTotal time\n";
    std::cout << "Dijkstra\t\t" << dijkstraTotalNodes << "\t\t" << dijkstraTotalTime << std::endl;
    std::cout << "A*lin\t\t" << aStarTotalNodes << "\t\t" << aStarTotalTime << std::endl;
    std::cout << "A*sph\t\t" << aStarAltTotalNodes << "\t\t"
              << aStarAltTotalTime << std::endl;
}

