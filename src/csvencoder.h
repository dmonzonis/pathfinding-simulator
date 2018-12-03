#ifndef CSVENCODER_H
#define CSVENCODER_H

#include <string>
#include "graph.h"

class CSVEncoder
{
public:
    CSVEncoder(std::string filename, std::string delimiter = ",");
    void saveGridGraph(GridGraph *graph, Tile start, Tile end) const;
    GridGraph* loadGridGraph();

    Tile getStartTile() const { return start; }
    Tile getGoalTile() const { return goal; }

private:
    std::string filename, delimiter;
    Tile start, goal;
};

#endif // CSVENCODER_H
