#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <exception>
#include <map>
#include <string>
#include "geolocationgraph.h"
#include "utils.h"

class Benchmark
{
public:
    Benchmark(std::string filename, int startNode, int goalNode);
    void run();

private:
    void buildCoordsMap();
    void buildGraph();
    double linearDistance(int a, int b);
    double sphericalDistance(int a, int b);

private:
    // Information about the problem to benchmark
    const double earthRadius = 6.357e6 /* m */;
    std::string filename;
    int startNode, goalNode;
    std::map<int, Coord> nodeToCoords;
    GeolocationGraph graph;
    int numNodes;
};

#endif // BENCHMARK_H
