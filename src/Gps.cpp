#include "Gps.h"

Gps::Gps(gps_sim_type st)
{
    // assume initial location is Columbia College
    mtx.lock();
    lat = 38.9579;
    lng = -92.3264;
    alt = 0.0;
    sim_t = st;
    mtx.unlock();
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

void Gps::updateAltitude(double delta)
{
    mtx.lock();
    alt = alt + delta;
    mtx.unlock();
}

void Gps::updatePosition(double delatLat, double deltaLng)
{
    mtx.lock();
    lat = lat + delatLat;
    lng = lng + deltaLng;
    mtx.unlock();
}
