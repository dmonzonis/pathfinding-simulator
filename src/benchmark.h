#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <exception>
#include <map>
#include <string>
#include "graph.h"

class Benchmark
{
public:
    Benchmark(std::string filename, int startNode, int goalNode);
    void run();

private:
    void buildCoordsMap();
    void buildGraph();
    double euclideanDistance(int first, int second);
    // double sphericalDistance(int a, int b);

private:
    std::string filename;
    int startNode, goalNode;
    std::map<int, std::pair<int, int>> nodeToCoords;
    SimpleGraph graph;
};

#endif // BENCHMARK_H
