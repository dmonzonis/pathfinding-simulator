#include "csvencoder.h"
#include <fstream>
#include <sstream>

CSVEncoder::CSVEncoder(std::string filename, std::string delimiter)
    : filename(filename),
      delimiter(delimiter)
{
}

void CSVEncoder::saveGridGraph(GridGraph *graph) const
{
    std::ofstream file(filename);
    std::vector<std::string> parts;

    // Write the dimensions in the first line
    int width = graph->getWidth();
    int height = graph->getHeight();
    parts.push_back(std::to_string(width));
    parts.push_back(std::to_string(height));
    file << joinParts(parts) << "\n";

    // Write all the weights
    std::pair<int, int> topLeft = graph->getTopLeft();
    int left = topLeft.first;
    int top = topLeft.second;
    for (int y = top; y < top + height; ++y)
    {
        parts.clear();
        for (int x = left; x < left + width; ++x)
        {
            parts.push_back(std::to_string(graph->getCost(Tile{x, y})));
        }
        file << joinParts(parts) << "\n";
    }
}

GridGraph* CSVEncoder::loadGridGraph() const
{
    std::ifstream file(filename);
    if (!file)
    {
        throw std::runtime_error("Error reading file");
    }
    std::string line;
    std::vector<std::string> parts;

    // First line is width, height of the graph
    std::getline(file, line);
    parts = splitLine(line);

    if (parts.size() != 2)
    {
        throw std::runtime_error("Error reading file");
    }
    int width = std::stoi(parts[0]);
    int height = std::stoi(parts[1]);

    // Create an empty graph
    int left = -width / 2;
    int top = -height / 2;
    // Make adjustments for odd widths and heights
    if (width % 2 != 0)
    {
        --left;
    }
    if (height % 2 != 0)
    {
        --top;
    }
    GridGraph *graph = new GridGraph(left, top, width, height);

    // Load all costs
    for (int y = top; y < top + height; ++y)
    {
        std::getline(file, line);
        parts = splitLine(line);
        if (parts.size() != width)
        {
            throw std::runtime_error("Error reading file");
        }
        for (int x = left; x < left + width; ++x)
        {
            double cost = std::stod(parts[x - left]);
            graph->setCost(Tile{x, y}, cost);
        }
    }

    return graph;
}

std::vector<std::string> CSVEncoder::splitLine(std::string line) const
{
    std::vector<std::string> result;
    // Split and add to the vector as long as the delimiter is found in the string
    auto pos = line.find(delimiter);
    while ((pos = line.find(delimiter)) != std::string::npos)
    {
        result.push_back(line.substr(0, pos));
        line.erase(0, pos + delimiter.length());
        pos = line.find(delimiter);
    }
    // Push the remains of the string
    result.push_back(line);
    return result;
}

std::string CSVEncoder::joinParts(std::vector<std::string> parts) const
{
    std::stringstream stream;
    for (size_t i = 0; i < parts.size(); ++i)
    {
        if (i != 0)
        {
            stream << delimiter;
        }
        stream << parts[i];
    }
    return stream.str();
}
