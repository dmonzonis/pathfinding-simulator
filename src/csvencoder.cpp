#include "csvencoder.h"
#include <fstream>
#include <sstream>
#include "utils.h"

CSVEncoder::CSVEncoder(std::string filename, std::string delimiter)
    : filename(filename),
      delimiter(delimiter)
{
}

void CSVEncoder::saveGridGraph(GridGraph *graph, Tile start, Tile end) const
{
    std::ofstream file(filename);
    std::stringstream stream;

    // Write the dimensions in the first line
    int width = graph->getWidth();
    int height = graph->getHeight();
    stream << width << delimiter << height << std::endl;
    file << stream.rdbuf();
    stream.str("");  // Clear stream

    // Write start and goal tile coordinates on second line
    stream << start.x << delimiter << start.y << delimiter;
    stream << end.x << delimiter << end.y << std::endl;
    file << stream.rdbuf();
    stream.str("");

    // Write all the weights
    std::pair<int, int> topLeft = graph->getTopLeft();
    int left = topLeft.first;
    int top = topLeft.second;
    for (int y = top; y < top + height; ++y)
    {
        stream.str("");
        for (int x = left; x < left + width; ++x)
        {
            if (x != left)
            {
                stream << delimiter;
            }
            stream << graph->getCost(Tile{x, y});
        }
        stream << std::endl;
        file << stream.rdbuf();
    }
}

GridGraph* CSVEncoder::loadGridGraph()
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
    parts = splitLine(line, delimiter);
    if (parts.size() != 2)
    {
        throw std::runtime_error("Error reading file");
    }
    int width = std::stoi(parts[0]);
    int height = std::stoi(parts[1]);

    // Second line is start and goal tile coordinates
    std::getline(file, line);
    parts = splitLine(line, delimiter);
    if (parts.size() != 4)
    {
        throw std::runtime_error("Error reading file");
    }
    start = Tile{std::stoi(parts[0]), std::stoi(parts[1])};
    goal = Tile{std::stoi(parts[2]), std::stoi(parts[3])};

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
        parts = splitLine(line, delimiter);
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
