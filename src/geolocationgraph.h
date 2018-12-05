#ifndef GEOLOCATIONGRAPH_H
#define GEOLOCATIONGRAPH_H

#include "graph.hpp"

typedef struct Geolocation
{
    int id, latitude, longitude;  // latitude and longitude in microdegrees
    double x, y, z;  // in meters, with origin at the center of the Earth

    void computeCartesianCoordinates();

    // Overload some operators so that tiles can be used as map keys
    bool operator<(const Geolocation &other) const
    {
        return id < other.id;
    }

    bool operator==(const Geolocation &other) const
    {
        return id == other.id;
    }

    bool operator!=(const Geolocation &other) const
    {
        return id != other.id;
    }
} Geolocation;

typedef SimpleGraph<Geolocation> GeolocationGraph;

#endif // GEOLOCATIONGRAPH_H
