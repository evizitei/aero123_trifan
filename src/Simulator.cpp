#include "Simulator.h"
#include <chrono>
#include <thread>

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
        /*
        TODO: examine the flight characteristics of the controller
        and update the GPS position accordingly so that we can see
        progress in "physical" space.
        */
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