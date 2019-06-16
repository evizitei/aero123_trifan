#include "Gyroscope.h"

Gyroscope::Gyroscope()
{
    // assume initial orientation is level
    mtx.lock();
    roll_angle = 0.0;
    pitch_angle = 0.0;
    mtx.unlock();
}

double Gyroscope::getRoll(){
    return roll_angle;
}

double Gyroscope::getPitch(){
    return pitch_angle;
}

void Gyroscope::updateOrientation(double pitch, double roll){
    mtx.lock();
    roll_angle = roll;
    pitch_angle = pitch;
    mtx.unlock();
}