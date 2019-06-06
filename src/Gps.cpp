#include "Gps.h"

Gps::Gps(gps_sim_type st)
{
    // assume initial location is Columbia College
    lat = 38.9579;
    lng = -92.3264;
    alt = 0.0;
    sim_t = st;
}

double Gps::getLatitude()
{
    return lat;
}

double Gps::getLongitude()
{
    return lng;
}

double Gps::getAltitude()
{
    return alt;
}