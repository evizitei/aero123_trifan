#include "FlightController.h"
#include "Gps.h"
#include <thread>
#include <iostream>
#include <string>

FlightController::FlightController(Gps* gps)
{
    mtx.lock();
    gpsPtr = gps;
    motor_front_right_bottom_speed = 0;
    motor_front_right_top_speed = 0;
    motor_back_bottom_speed = 0;
    motor_back_top_speed = 0;
    motor_front_left_bottom_speed = 0;
    motor_front_left_top_speed = 0;
    tilt_servo_angle = 0;
    elevon_right_angle = 0;
    elevon_left_angle = 0;
    landing_gear_servo_angle = 0;
    state = "off";
    state_threshold = 0;
    mtx.unlock();
}

int FlightController::getMotorSpeed(int motorIndex)
{
    int speedValue;
    mtx.lock();
    switch (motorIndex)
    {
        case 0:
            speedValue = motor_front_right_bottom_speed;
            break;
        case 1:
            speedValue = motor_front_right_top_speed;
            break;
        case 2:
            speedValue = motor_back_bottom_speed;
            break;
        case 3:
            speedValue = motor_back_top_speed;
            break;
        case 4:
            speedValue = motor_front_left_bottom_speed;
            break;
        case 5:
            speedValue = motor_front_left_top_speed;
            break;
        default:
            speedValue = -1;
            break;
    }
    mtx.unlock();

    return speedValue;
}

int FlightController::getTiltAngle()
{
    return tilt_servo_angle;
}

int FlightController::getElevonAngle(int idx){
    if(idx == 0)
    {
        return elevon_right_angle;
    }
    else if(idx == 1)
    {
        return elevon_left_angle;
    }
    else
    {
        std::cout << "WARNING: No such elevon index: " << std::to_string(idx);
        return 0;
    }
    
}

void FlightController::lockedMotorUpdate(TrifanMotorConfig conf)
{
    motor_front_right_bottom_speed = conf.right_prop.bottom_speed;
    motor_front_right_top_speed = conf.right_prop.top_speed;
    motor_back_bottom_speed = conf.back_prop.bottom_speed;
    motor_back_top_speed = conf.back_prop.top_speed;
    motor_front_left_bottom_speed = conf.left_prop.bottom_speed;
    motor_front_left_top_speed = conf.left_prop.top_speed;
}

void FlightController::lockedUpdateMotors(int rpm)
{
    TrifanMotorConfig cfg = {
        { rpm, rpm},
        { rpm, rpm},
        { rpm, rpm}
    };
    lockedMotorUpdate(cfg);
}

void FlightController::updateMotors(TrifanMotorConfig conf)
{
    mtx.lock();
    lockedMotorUpdate(conf);
    mtx.unlock();
}

// Convenience method when you want to set all motors to the same
// rpm level with a simple parameter
void FlightController::updateMotors(int rpm)
{
    TrifanMotorConfig cfg = {
        { rpm, rpm},
        { rpm, rpm},
        { rpm, rpm}
    };
    updateMotors(cfg);
}

// 0 degrees is vertical hover
// 90 degrees is full forward flight
void FlightController::tiltProps(int deg)
{
    mtx.lock();
    tilt_servo_angle = deg;
    mtx.unlock();
}

// 0 is neutral
// positive numbers are angled above the wing (climb)
// negative numbers are angled below the wing (dive)
void FlightController::setElevons(int deg)
{
    mtx.lock();
    elevon_right_angle = deg;
    elevon_left_angle = deg;
    mtx.unlock();
}

// 0 is gear deployed
// 90 is gear fully retracted
void FlightController::setGearSrv(int degrees)
{
    mtx.lock();
    landing_gear_servo_angle = degrees;
    mtx.unlock();
}

std::string FlightController::getState()
{
    return state;
}

std::string FlightController::getStatus()
{
    mtx.lock();
    std::string status("FLIGHT STATUS\n");
    status = status + "  STATE: " + state + " || THRESHOLD: " + std::to_string(state_threshold) + "\n"; 
    status = status + "  MOTORS [ RB: " + std::to_string(motor_front_right_bottom_speed);
    status = status + " RT: " + std::to_string(motor_front_right_top_speed);
    status = status + " BB: " + std::to_string(motor_back_bottom_speed);
    status = status + " BT: " + std::to_string(motor_back_top_speed);
    status = status + " LB: " + std::to_string(motor_front_left_bottom_speed);
    status = status + " LT: " + std::to_string(motor_front_left_top_speed) + " ]\n";

    status = status + "  TILT Srv: " + std::to_string(tilt_servo_angle) + "\n";
    status = status + "  ELEVONS [ Right: " + std::to_string(elevon_right_angle) + " Left: " + std::to_string(elevon_left_angle) + " ]\n";
    status = status + "  GEAR Srv: " + std::to_string(landing_gear_servo_angle) + "\n";
    status = status + "  LOCATION [ LAT: " + std::to_string(gpsPtr->getLatitude());
    status = status + " LNG: " + std::to_string(gpsPtr->getLongitude());
    status = status + " ALT: " + std::to_string(gpsPtr->getAltitude()) + " ]\n";
    mtx.unlock();
    return status;
}

void FlightController::landingStep()
{
    // So a thought occurs that at different levels of wind and such these values wouldn't always work.
    // 2500 might keep you at the same altitude as there is an updraft.
    // the comments "stable - ###" is the values off the motor speed that would allow for descent
    mtx.lock();
    double altitude = gpsPtr->getAltitude();
    if(altitude >50.0){                                //if drone is currently over 50 units in altitude
        lockedUpdateMotors(2500); // stable - 500 OR 0.8333 of stable 
    }else if(altitude > 20.0){  //if drone is currently between 50 and 20 units
        lockedUpdateMotors(2750);  // stable - 250 OR 0.9167 of stable
    }else if(altitude >0.0 && altitude <= 20){     //if drone is below 20 units but not on the ground
        lockedUpdateMotors(2900); 
    }else if(altitude <= 0.0){
        state = "landed";
        state_threshold = 0;
    }
    mtx.unlock();
}

void FlightController::armingStep()
{
    mtx.lock();
    if(motor_front_right_bottom_speed < state_threshold){
        lockedUpdateMotors(motor_front_right_bottom_speed + 100);
    }else if(motor_front_right_bottom_speed >= state_threshold){
        lockedUpdateMotors(state_threshold);
        state = "armed";
    }
    mtx.unlock();
}

void FlightController::disarmingStep()
{
    mtx.lock();
    if(motor_front_right_bottom_speed > 0){
        lockedUpdateMotors(motor_front_right_bottom_speed - 100);
    }else if(motor_front_right_bottom_speed <= 0){
        lockedUpdateMotors(0);
        state = "disarmed";
    }
    mtx.unlock();
}

void FlightController::run()
{
    int ctrlInterval = 1;
    while (true){
        std::this_thread::sleep_for(std::chrono::milliseconds(ctrlInterval * 1000));
        if (state == "landing"){
            landingStep();
        } else if (state == "arming"){
            armingStep();
        } else if (state == "disarming"){
            disarmingStep();
        } else if(state == "shutdown"){
            break;
        }
        // other states require no action in this thread
    }
}

void FlightController::signalStop()
{
    mtx.lock();
    state = "shutdown";
    state_threshold = 0;
    mtx.unlock();
}

void FlightController::setState(std::string state_string, int threshold)
{
    mtx.lock();
    state = state_string;
    state_threshold = threshold;
    mtx.unlock();
}