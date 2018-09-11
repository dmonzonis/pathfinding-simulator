#ifndef CSVENCODER_H
#define CSVENCODER_H

#include <string>
#include "graph.h"

class CSVEncoder
{
public:
    CSVEncoder(std::string filename, std::string delimiter = ",");
    void saveGridGraph(GridGraph *graph) const;
    GridGraph* loadGridGraph() const;

private:
    std::vector<std::string> splitLine(std::string line) const;
    std::string joinParts(std::vector<std::string> parts) const;

private:
    std::string filename, delimiter;
};

#endif // CSVENCODER_H
