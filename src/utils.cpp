#include "utils.h"
#include <cmath>
#include <iostream>

void showErrorMessage(std::string msg)
{
    QMessageBox errorMsg;
    errorMsg.critical(nullptr, "Error", QString::fromStdString(msg));
    errorMsg.setFixedSize(500, 200);
}

bool approxEqual(double a, double b, double tolerance)
{
    return std::abs(a - b) < tolerance;
}

void printUsage()
{
    std::cout << "Usage:" << std::endl;
    std::cout << "pathfinding [command] [option]" << std::endl;
    std::cout << "Available commands:" << std::endl;
    std::cout << "-b FILENAME START GOAL\t\tRun benchmark using the graph and coordinates from DIMACS with the given filename from the START node to the GOAL node" << std::endl;
}

std::vector<std::string> splitLine(std::string line) const
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

std::string joinParts(std::vector<std::string> parts) const
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
