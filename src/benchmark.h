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

private:
    // Information about the problem to benchmark
    std::string filename;
    int startNodeId, goalNodeId;
    std::map<int, Geolocation> mapIdToGeolocation;
    GeolocationGraph graph;
    int numNodes;
};

#endif // BENCHMARK_H
