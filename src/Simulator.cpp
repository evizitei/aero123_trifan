#include "FlightController.h"
#include "Simulator.h"
#include "Gps.h"
#include <iostream>
#include <chrono>
#include <thread>

const int RPM_EQUILLIBRIUM = 3000;

Simulator::Simulator(FlightController* cPtr, Gps* gpsPtr)
{
    ctrl = cPtr;
    gps = gpsPtr;
    shouldStop = false;
}


void Simulator::run()
{
    int simInterval = 1;
    while (true){
        std::this_thread::sleep_for(std::chrono::milliseconds(simInterval * 1000));
        if(ctrl->getTiltAngle() == 0)
        {
            // we're hovering, just process altitude change
            // assume all motors at same speed for now
            int rpms = ctrl->getMotorSpeed(0);
            if(gps->getAltitude() <= 0.0)
            {
                //only change altitude if we're going up from the ground
                if(rpms > RPM_EQUILLIBRIUM){
                    double delta = double((rpms - RPM_EQUILLIBRIUM) / 100);
                    gps->updateAltitude(delta);
                }
            }
            else
            {
                // completely made up magnitude, but it will go in the right direction
                gps->updateAltitude(double((rpms - RPM_EQUILLIBRIUM) / 100));
            }
        }
        else if(ctrl->getTiltAngle() == 90)
        {
            // we're in full forward flight, compute forward position change 
            // and use elevon angle for altitude (approximate)
            // *TODO*
        }
        else
        {
            // transitional flight.  compute vector of force based on tilt angle
            // and update altitude and lateral position accordingly.
            // *TODO*
        }

        if(shouldStop){
            break;
        }
    }
}

void Simulator::signalStop()
{
    mtx.lock();
    shouldStop = true;
    mtx.unlock();
}