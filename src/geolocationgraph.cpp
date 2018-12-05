#include "geolocationgraph.h"
#include <cmath>

void Geolocation::computeCartesianCoordinates()
{
    // Theta and Phi angles must be in radians
    double theta = (90 - (latitude / 1e6)) * M_PI / 180;
    double phi = (longitude / 1e6) * M_PI / 180;
    // Compute Cartesian coordinates
    x = radius * std::sin(theta) * std::cos(phi);
    y = radius * std::sin(theta) * std::sin(phi);
    z = radius * std::cos(theta);
}
