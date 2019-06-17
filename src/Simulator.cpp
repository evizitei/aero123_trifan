#include "FlightController.h"
#include "Simulator.h"
#include "Gps.h"
#include "Gyroscope.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <math.h>

#define PI 3.14159265

const int RPM_EQUILLIBRIUM = 3000;
const int TRANS_POWER_EQUILLIBRIUM = 2600;
const int FF_POWER_EQUILLIBRIUM = 1500;

namespace TextSim {
    double computeHeading(double cur_heading, double roll_angle)
    {
        double new_heading = cur_heading + (double(roll_angle) / 2.0);
        if(new_heading < 0.0){
            new_heading = 360 + new_heading;
        }else if (new_heading > 360.0) {
            new_heading = new_heading - 360.0;
        }
        return new_heading;
    }
}

Simulator::Simulator(FlightController* cPtr, Gps* gpsPtr, Gyroscope* gyPtr)
{
    mtx.lock();
    ctrl = cPtr;
    gps = gpsPtr;
    gyro = gyPtr;
    shouldStop = false;
    aoa = 0.0;
    heading = 0.0;
    logFileName = "simulator.log";
    logStream.open(logFileName);
    mtx.unlock();
}

void Simulator::simulateHover()
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
        }else{
            // make sure we don't update the altitude to be below the ground
            gps->updateAltitude(gps->getAltitude() * -1);
        }
    }
    else
    {
        // completely made up magnitude, but it will go in the right direction
        gps->updateAltitude(double((rpms - RPM_EQUILLIBRIUM) / 100));
    }
}

void Simulator::simulateForwardFlight()
{
    // we're in full forward flight, compute forward position change 
    // and use elevon angle for altitude (approximate)
    //
    // 1) check elevons, adjust angle of attack
    mtx.lock();
    int era = ctrl->getElevonAngle(0);
    int ela = ctrl->getElevonAngle(1);
    double roll = gyro->getRoll();
    if(era == ela)
    {
        // elevons are equal, so adjust AOA by an invented factor
        aoa = aoa + (double(era) / 8.0);
    }else{
        // elevons are offset, bank by difference.
        double elDelta = era - ela;
        roll = roll + (elDelta / 5.0);
    }
    mtx.unlock();

    bool altShortCircuited = false;
    if(aoa > 55.0)
    {
        // Angle of Attack is too high, stall
        double delta = -25.0;
        gps->updateAltitude(delta);
        altShortCircuited = true;
    }else if (aoa < -55.0)
    {
        // Angle of Attack is too steep, dive
        double delta = -50.0;
        gps->updateAltitude(delta);
        altShortCircuited = true;
    }
    gyro->updateOrientation(aoa, roll);
    heading = TextSim::computeHeading(heading, roll);

    // AOA is stable
    // assume all motors at same speed for now
    int rpms = ctrl->getMotorSpeed(0);
    if(rpms < FF_POWER_EQUILLIBRIUM)
    {
        // power too low, we're losing altitude no matter what
        double delta = double((rpms - FF_POWER_EQUILLIBRIUM) / 100);
        gps->updateAltitude(delta);
        altShortCircuited = true;
    }
    // motors fast enough for powered flight, compute magnitude of vector
    // LET 1500 RPM == 10.0 m/s, and every 500 RPMs be an additional 2.5 m/s
    double vector_magnitude = 10.0 + ((double(rpms - FF_POWER_EQUILLIBRIUM) / 500.0) * 2.5);
    // compare with AOA to determine altitude displacement
    double lateral_component = vector_magnitude;
    if (aoa != 0.0)
    {
        // sin(aoa) = vertical_comp / vector => vertical_comp = sin(aoa) * vector
        double altitude_component = sin(abs(aoa) * (PI/180)) * vector_magnitude;
        lateral_component = sqrt((vector_magnitude*vector_magnitude) - (altitude_component*altitude_component));
        //3.28 feet to meter
        double delta = altitude_component * 3.28;
        if(aoa < 0.0)
        {
            delta = delta * -1.0;
        }
        if(!altShortCircuited)
            gps->updateAltitude(delta);
    }
    simulateCoordinateMotion(lateral_component);
}

void Simulator::simulateTransitionalFlight()
{
    // transitional flight.  compute vector of force based on tilt angle
    // and update altitude and lateral position accordingly.
    int rpms = ctrl->getMotorSpeed(0);
    bool altShortCircuited = false;
    if(rpms < TRANS_POWER_EQUILLIBRIUM)
    {
        // power too low, we're losing altitude no matter what
        double delta = double((rpms - TRANS_POWER_EQUILLIBRIUM) / 100);
        gps->updateAltitude(delta);
        altShortCircuited = true;
    }
    
    // motors fast enough for powered flight, compute magnitude of vector
    // LET 2000 RPM == 10.0 m/s, and every 500 RPMs be an additional 2.5 m/s
    double vector_magnitude = 10.0 + ((double(rpms - TRANS_POWER_EQUILLIBRIUM) / 500.0) * 2.5);
    // compare with rotor tilt to determine altitude displacement
    double lateral_component = vector_magnitude;
    // sin(90 - tilt) = vertical_comp / vector => vertical_comp = sin(90 - tilt) * vector
    int tilt = ctrl->getTiltAngle();
    double altitude_component = sin(abs(90 - tilt) * (PI/180)) * vector_magnitude;
    double weight_factor = 6.5;
    lateral_component = sqrt((vector_magnitude*vector_magnitude) - (altitude_component*altitude_component));
    altitude_component = altitude_component - weight_factor;
    //3.28 feet to meter
    double delta = altitude_component * 3.28;
    if(!altShortCircuited)
        gps->updateAltitude(delta);
    simulateCoordinateMotion(lateral_component);
}

void Simulator::simulateCoordinateMotion(double lateral_component)
{
    // compute lat/lng components from remaining vector magnitude based on heading
    double heading_rad = heading * (PI / 180);
    double lng_comp = cos(heading_rad);
    double lat_comp = sin(heading_rad);
    // turn meters into lat/lng coordinate displacement;
    double lat_delta = double(lat_comp * lateral_component) / 10000.0;
    double lng_delta = double(lng_comp * lateral_component) / 10000.0;
    gps->updatePosition(lat_delta, lng_delta);

    if(gps->getAltitude() <= 0.0){
        std::string fail_msg = "CATASTROPHIC FAILURE: COLLISION WITH TERRAIN!";
        std::cout << fail_msg << "\n\n :( \n\n";
        logMessage(fail_msg);
        exit(-1);
    }
    if(gps->getAltitude() < 50.0){
        std::string terrain_warning = "WARNING: TERRAIN!";
        std::cout << terrain_warning << "\n";
        logMessage(terrain_warning);
    }
}

void Simulator::run()
{
    int simInterval = 1;
    while (true){
        std::this_thread::sleep_for(std::chrono::milliseconds(simInterval * 1000));
        if(ctrl->getTiltAngle() == 0)
        {
            simulateHover();
        }
        else if(ctrl->getTiltAngle() == 90)
        {
            simulateForwardFlight();
        }
        else
        {
            simulateTransitionalFlight();
        }
        logMessage(getStatus() + ctrl->getStatus());
        if(shouldStop){
            break;
        }
    }
    mtx.lock();
    logStream.flush();
    logStream.close();
    mtx.unlock();
}

void Simulator::logTime()
{
    std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now();
    std::time_t tsTime = std::chrono::system_clock::to_time_t(timestamp);
    mtx.lock();
    logStream << " ---- " << std::ctime(&tsTime) << "";
    mtx.unlock();
}

void Simulator::logMessage(std::string message)
{
    logTime();
    mtx.lock();
    logStream << "***************************\n" << message << "\n***************************\n\n";
    logStream.flush();
    mtx.unlock();
}

std::string Simulator::getStatus()
{
    mtx.lock();
    std::string status("SIMULATOR STATUS\n");
    status = status + "  HEADING: " + std::to_string(heading) + "\n";
    status = status + "  ANGLE OF ATTACK: " + std::to_string(aoa) + "\n";
    mtx.unlock();
    return status;
}

void Simulator::signalStop()
{
    mtx.lock();
    shouldStop = true;
    mtx.unlock();
}