#include "FlightController.h"
#include "Gps.h"
#include <thread>
#include <iostream>
#include <string>

const int RPM_EQUILLIBRIUM = 3000;
const int TRANS_POWER_EQUILLIBRIUM = 2600;
const int FF_POWER_EQUILLIBRIUM = 1500;

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
        lockedUpdateMotors(RPM_EQUILLIBRIUM - 1800); 
    }else if(altitude > 20.0){  //if drone is currently between 50 and 20 units
        lockedUpdateMotors(RPM_EQUILLIBRIUM - 1400);  // stable - 250 OR 0.9167 of stable
    }else if(altitude >0.0 && altitude <= 20){     //if drone is below 20 units but not on the ground
        lockedUpdateMotors(RPM_EQUILLIBRIUM - 700); 
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
        lockedUpdateMotors(motor_front_right_bottom_speed + 250);
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
        lockedUpdateMotors(motor_front_right_bottom_speed - 500);
    }else if(motor_front_right_bottom_speed <= 0){
        lockedUpdateMotors(0);
        state = "disarmed";
    }
    mtx.unlock();
}

void FlightController::hoverClimbStep()
{
    mtx.lock();
    double curAlt = gpsPtr->getAltitude();
    if(curAlt < state_threshold){
        if(motor_back_bottom_speed < RPM_EQUILLIBRIUM){
            lockedUpdateMotors(motor_front_right_bottom_speed + 750);
        } else if((state_threshold - curAlt) > 300) {
            lockedUpdateMotors(motor_front_right_bottom_speed + 200);
        }else if((state_threshold - curAlt) > 150){
            lockedUpdateMotors(RPM_EQUILLIBRIUM + 1500);
        }else if((state_threshold - curAlt) > 50){
            lockedUpdateMotors(RPM_EQUILLIBRIUM + 1000);
        }else if((state_threshold - curAlt) < 25){
            lockedUpdateMotors(RPM_EQUILLIBRIUM + 500);
        }
    }else if(curAlt >= state_threshold){
        lockedUpdateMotors(RPM_EQUILLIBRIUM);
        state = "hover";
    }
    mtx.unlock();
}

void FlightController::transFromHoverStep()
{
    mtx.lock();
    if(tilt_servo_angle < state_threshold){
        if((state_threshold - tilt_servo_angle) < 5){
            tilt_servo_angle = state_threshold;
        }else{
            tilt_servo_angle = tilt_servo_angle + 5;
        }
        lockedUpdateMotors(motor_back_bottom_speed - 50);
    } else {
        lockedUpdateMotors(TRANS_POWER_EQUILLIBRIUM);
        state = "transitional_flight";
    }
    mtx.unlock();
}

void FlightController::transFromForwardStep()
{
     mtx.lock();
    if(tilt_servo_angle > state_threshold){
        if((tilt_servo_angle - state_threshold) < 5){
            tilt_servo_angle = state_threshold;
        }else{
            tilt_servo_angle = tilt_servo_angle - 5;
        }
        lockedUpdateMotors(motor_back_bottom_speed + 50);
    } else {
        lockedUpdateMotors(TRANS_POWER_EQUILLIBRIUM);
        state = "transitional_flight";
    }
    mtx.unlock();
}

void FlightController::transToHoverStep()
{
    mtx.lock();
    if(tilt_servo_angle > 0){
        if((tilt_servo_angle) < 5){
            tilt_servo_angle = 0;
        }else{
            tilt_servo_angle = tilt_servo_angle - 5;
        }
        lockedUpdateMotors(motor_back_bottom_speed + 50);
    } else {
        lockedUpdateMotors(RPM_EQUILLIBRIUM);
        state = "hover";
    }
    mtx.unlock();
}

void FlightController::transToForwardStep()
{
    mtx.lock();
    if(tilt_servo_angle < 90){
        if(tilt_servo_angle > 85){
            tilt_servo_angle = 90;
        }else{
            tilt_servo_angle = tilt_servo_angle + 5;
        }
        lockedUpdateMotors(motor_back_bottom_speed - 50);
    } else {
        lockedUpdateMotors(FF_POWER_EQUILLIBRIUM);
        state = "flying_forward";
    }
    mtx.unlock();
}

bool FlightController::processStep()
{
    if (state == "landing"){
        landingStep();
    } else if (state == "arming"){
        armingStep();
    } else if (state == "disarming"){
        disarmingStep();
    } else if (state == "hover_climbing"){
        hoverClimbStep();
    } else if (state == "transitioning_from_hover"){
        transFromHoverStep();
    } else if (state == "transitioning_from_ff"){
        transFromForwardStep();
    } else if (state == "transitioning_to_hover"){
        transToHoverStep();
    } else if (state == "transitioning_to_forward"){
        transToForwardStep();
    } else if(state == "shutdown"){
        return true;
    }
    // other states require no action in this thread
    return false;
}

void FlightController::run()
{
    int ctrlInterval = 1;
    while (true){
        std::this_thread::sleep_for(std::chrono::milliseconds(ctrlInterval * 1000));
        bool shouldAbort = processStep();
        if(shouldAbort)
            break;
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