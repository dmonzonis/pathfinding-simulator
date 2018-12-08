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
    std::cout << "### Running grid graph benchmark ###" << std::endl;
    runGridBenchmark(count);

    // Reset result structures
    timesDijkstra.clear();
    timesAstar.clear();
    timesAstarAlt.clear();
    expandedDijkstra.clear();
    expandedAstar.clear();
    expandedAstarAlt.clear();
    distDijkstra.clear();
    distAstar.clear();
    distAstarAlt.clear();
    distGreedy.clear();

    std::cout << "### Running geolocation graph benchmark ###" << std::endl;
    runRoadBenchmark(count);
}

void Benchmark::runGridBenchmark(int count)
{
    // Assume the randomgrid.csv file has been generated
    std::cout << "Loading grid graph..." << std::endl;
    CSVEncoder encoder("randomgrid.csv");
    gridGraph = encoder.loadGridGraph();

    // Write header of benchmark results CSV file
    std::ofstream file("benchmark_grid.csv");
    file << "dijDist,dijNodes,dijTime,A*Dist,A*Nodes,A*Time,A*altDist,A*altNodes,A*altTime,greedyDist,greedyNodes,greedyTime"
         << std::endl;

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
    std::ofstream file("benchmark_road.csv");
    file << "dijDist,dijNodes,dijTime,A*Dist,A*Nodes,A*Time,A*altDist,A*altNodes,A*altTime,greedyDist,greedyNodes,greedyTime"
         << std::endl;

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
    double optimalDistance;
    std::map<Tile, Tile> previous;
    std::map<Tile, double> costToNode;
    Algorithm algorithm;
    Heuristic<Tile> heuristic;

    try
    {
        // Dijkstra
        algorithm = std::bind(&dijkstra<Tile, GridGraph>,
                              gridGraph, startTile, goalTile,
                              std::ref(previous), std::ref(costToNode));
        evaluateAlgorithm(algorithm, timesDijkstra, expandedDijkstra);
        optimalDistance = costToNode[goalTile];
        if (previous.find(goalTile) == previous.end()
                || optimalDistance == std::numeric_limits<double>::infinity())
        {
            // There doesn't exist a path
            std::cout << "No path found. Repeating with different start/goal tiles." << std::endl;
            return false;
        }
        distDijkstra.push_back(optimalDistance);

        // Reset structures
        costToNode.clear();
        previous.clear();

        // A* with Manhattan distance
        heuristic = manhattanDistance;
        algorithm = std::bind(&aStar<Tile, GridGraph>,
                              gridGraph, startTile, goalTile,
                              std::ref(previous), std::ref(costToNode), heuristic);
        evaluateAlgorithm(algorithm, timesAstar, expandedAstar);
        distAstar.push_back(costToNode[goalTile]);

        // Reset structures
        costToNode.clear();
        previous.clear();

        // A* with Euclidean distance
        heuristic = euclideanDistance;
        algorithm = std::bind(&aStar<Tile, GridGraph>,
                              gridGraph, startTile, goalTile,
                              std::ref(previous), std::ref(costToNode), heuristic);
        evaluateAlgorithm(algorithm, timesAstarAlt, expandedAstarAlt);
        distAstarAlt.push_back(costToNode[goalTile]);

        // Reset structures
        costToNode.clear();
        previous.clear();

        // Greedy with Manhattan distance
        heuristic = manhattanDistance;
        algorithm = std::bind(&greedyBestFirstSearch<Tile, GridGraph>,
                              gridGraph, startTile, goalTile,
                              std::ref(previous), std::ref(costToNode), heuristic);
        evaluateAlgorithm(algorithm, timesGreedy, expandedGreedy);
        distGreedy.push_back(costToNode[goalTile]);

        // Write partial results to CSV file
        std::ofstream file("benchmark_grid.csv", std::ios_base::app);
        file << distDijkstra.back() << ","
             << expandedDijkstra.back() << ","
             << timesDijkstra.back() << ","
             << distAstar.back() << ","
             << expandedAstar.back() << ","
             << timesAstar.back() << ","
             << distAstarAlt.back() << ","
             << expandedAstarAlt.back() << ","
             << timesAstarAlt.back() << ","
             << distGreedy.back() << ","
             << expandedGreedy.back() << ","
             << timesGreedy.back()
             << std::endl;

        return true;
    }
    catch (std::exception &ex)
    {
        std::cout << "There was an error: " << ex.what() << std::endl;
        std::cout << "Repeating the benchmark." << std::endl;
        return false;
    }
}

void Benchmark::runRoadSingle(int startId, int goalId)
{
    std::map<Geolocation, Geolocation> previous;
    std::map<Geolocation, double> costToNode;
    Algorithm algorithm;
    Heuristic<Geolocation> heuristic;
    Geolocation startNode = mapIdToGeolocation[startId];
    Geolocation goalNode = mapIdToGeolocation[goalId];

    // Dijkstra
    algorithm = std::bind(&dijkstra<Geolocation, GeolocationGraph>,
                          &geolocationGraph, startNode, goalNode,
                          std::ref(previous), std::ref(costToNode));
    evaluateAlgorithm(algorithm, timesDijkstra, expandedDijkstra);
    distDijkstra.push_back(costToNode[goalNode]);

    // Reset structures
    costToNode.clear();
    previous.clear();

    // A* with linear distance
    heuristic = euclideanDistance3D;
    algorithm = std::bind(&aStar<Geolocation, GeolocationGraph>,
                          &geolocationGraph, startNode, goalNode,
                          std::ref(previous), std::ref(costToNode), heuristic);
    evaluateAlgorithm(algorithm, timesAstar, expandedAstar);
    distAstar.push_back(costToNode[goalNode]);

    // Reset structures
    costToNode.clear();
    previous.clear();

    // A* with Haversine distance
    heuristic = haversineDistance;
    algorithm = std::bind(&aStar<Geolocation, GeolocationGraph>,
                          &geolocationGraph, startNode, goalNode,
                          std::ref(previous), std::ref(costToNode), heuristic);
    evaluateAlgorithm(algorithm, timesAstarAlt, expandedAstarAlt);
    distAstarAlt.push_back(costToNode[goalNode]);

    // Reset structures
    costToNode.clear();
    previous.clear();

    // Greedy with linear distance
    heuristic = euclideanDistance3D;
    algorithm = std::bind(&greedyBestFirstSearch<Geolocation, GeolocationGraph>,
                          &geolocationGraph, startNode, goalNode,
                          std::ref(previous), std::ref(costToNode), heuristic);
    evaluateAlgorithm(algorithm, timesGreedy, expandedGreedy);
    distGreedy.push_back(costToNode[goalNode]);

    // Write partial results to CSV file
    std::ofstream file("benchmark_road.csv", std::ios_base::app);
    file << distDijkstra.back() << ","
         << expandedDijkstra.back() << ","
         << timesDijkstra.back() << ","
         << distAstar.back() << ","
         << expandedAstar.back() << ","
         << timesAstar.back() << ","
         << distAstarAlt.back() << ","
         << expandedAstarAlt.back() << ","
         << timesAstarAlt.back() << ","
         << distGreedy.back() << ","
         << expandedGreedy.back() << ","
         << timesGreedy.back()
         << std::endl;
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
            Geolocation node1 = mapIdToGeolocation[std::stoi(parts[1])];
            Geolocation node2 = mapIdToGeolocation[std::stoi(parts[2])];
            // Calculate weight using Haversine distance formula
            double weight = haversineDistance(node1, node2);
            // Add node to the graph
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
    double dijkstraTotalNodes, aStarTotalNodes, aStarAltTotalNodes, greedyTotalNodes;
    double dijkstraTotalTime, aStarTotalTime, aStarAltTotalTime, greedyTotalTime;

    // Compute totals
    dijkstraTotalNodes = std::accumulate(expandedDijkstra.begin(), expandedDijkstra.end(), 0);
    aStarTotalNodes = std::accumulate(expandedAstar.begin(), expandedAstar.end(), 0);
    aStarAltTotalNodes = std::accumulate(expandedAstarAlt.begin(),
                                               expandedAstarAlt.end(),
                                               0);
    greedyTotalNodes = std::accumulate(expandedGreedy.begin(), expandedGreedy.end(), 0);

    dijkstraTotalTime = std::accumulate(timesDijkstra.begin(), timesDijkstra.end(), 0.);
    aStarTotalTime = std::accumulate(timesAstar.begin(), timesAstar.end(), 0.);
    aStarAltTotalTime = std::accumulate(timesAstarAlt.begin(),
                                               timesAstarAlt.end(),
                                               0.);
    greedyTotalTime = std::accumulate(timesGreedy.begin(), timesGreedy.end(), 0.);

    // Report summary
    std::cout << "\n###############\nSummary\n###############\n";
    std::cout << "Algorithm\t\tTotal nodes\t\tTotal time\n";
    std::cout << "Dijkstra\t\t" << dijkstraTotalNodes << "\t\t" << dijkstraTotalTime << std::endl;
    std::cout << "A*\t\t" << aStarTotalNodes << "\t\t" << aStarTotalTime << std::endl;
    std::cout << "A*(alt)\t\t" << aStarAltTotalNodes << "\t\t"
              << aStarAltTotalTime << std::endl;
    std::cout << "Greedy\t\t" << greedyTotalNodes << "\t\t" << greedyTotalTime << std::endl;
}

void Benchmark::evaluateAlgorithm(std::function<unsigned long(void)> alg,
                                  std::vector<double> &timeVec,
                                  std::vector<unsigned long> &nodeVec)
{
    double timeBegin, timeEnd, elapsedTime;
    timeBegin = std::clock();
    unsigned long expandedNodes = alg();
    timeEnd = std::clock();
    elapsedTime = double(timeEnd - timeBegin) / CLOCKS_PER_SEC;

    timeVec.push_back(elapsedTime);
    nodeVec.push_back(expandedNodes);
}
