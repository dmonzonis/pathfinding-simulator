#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <exception>
#include <map>
#include <string>
#include <functional>
#include "gridgraph.h"
#include "geolocationgraph.h"
#include "utils.h"

class Benchmark
{
public:
    Benchmark(std::string filename);
    ~Benchmark();

    /**
     * @brief Run a benchmark with randomized start and goal nodes a certain amount of times
     * and show results.
     * @param count Number of randomized benchmarks to run.
     *
     * Stores intermediate results in a file called benchmark.txt
     */
    void run(int count);

private:
    typedef std::function<unsigned long(void)> Algorithm;

private:
    void buildCoordsMap();
    void buildGeolocationGraph();
    bool runGridSingle(Tile startTile, Tile goalTile);
    void runRoadSingle(int startId, int goalId);
    void runSummary();
    void runGridBenchmark(int count);
    void runRoadBenchmark(int count);
    void evaluateAlgorithm(std::function<unsigned long(void)> alg,
                           std::vector<double> &timeVec,
                           std::vector<unsigned long> &nodeVec);

private:
    // Information about the problem to benchmark
    std::string filename;
    std::map<int, Geolocation> mapIdToGeolocation;
    GridGraph *gridGraph;
    GeolocationGraph geolocationGraph;
    int numNodes;
    std::vector<double> timesDijkstra, timesAstar, timesAstarAlt, timesGreedy;
    std::vector<unsigned long> expandedDijkstra, expandedAstar, expandedAstarAlt, expandedGreedy;
};

#endif // BENCHMARK_H
