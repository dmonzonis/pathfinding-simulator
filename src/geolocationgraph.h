#ifndef GEOLOCATIONGRAPH_H
#define GEOLOCATIONGRAPH_H

#include "graph.hpp"

typedef struct Geolocation
{
    Geolocation(int latitude, int longitude);
    int latitude, longitude;  // in microdegrees
    double x, y, z;  // in meters, with origin at the center of the Earth

    // Overload some operators so that tiles can be used as map keys
    bool operator==(const Geolocation &other) const
    {
        return latitude == other.latitude && longitude == other.longitude;
    }

    bool operator<(const Geolocation &other) const
    {
        return latitude < other.latitude || (latitude == other.latitude && longitude < other.longitude);
    }

    bool operator!=(const Geolocation &other) const
    {
        return latitude != other.latitude || longitude != other.longitude;
    }
} Geolocation;

typedef SimpleGraph<Geolocation> GeolocationGraph;

#endif // GEOLOCATIONGRAPH_H
