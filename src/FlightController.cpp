#include "FlightController.h"
#include "Gps.h"
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
    elevon_one_angle = 0;
    elevon_two_angle = 0;
    landing_gear_servo_angle = 0;
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

void FlightController::updateMotors(TrifanMotorConfig conf)
{
    mtx.lock();
    motor_front_right_bottom_speed = conf.right_prop.bottom_speed;
    motor_front_right_top_speed = conf.right_prop.top_speed;
    motor_back_bottom_speed = conf.back_prop.bottom_speed;
    motor_back_top_speed = conf.back_prop.top_speed;
    motor_front_left_bottom_speed = conf.left_prop.bottom_speed;
    motor_front_left_top_speed = conf.left_prop.top_speed;
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

std::string FlightController::getStatus()
{
    mtx.lock();
    std::string status("FLIGHT STATUS\n");
    status = status + "  MOTORS [ RB: " + std::to_string(motor_front_right_bottom_speed);
    status = status + " RT: " + std::to_string(motor_front_right_top_speed);
    status = status + " BB: " + std::to_string(motor_back_bottom_speed);
    status = status + " BT: " + std::to_string(motor_back_top_speed);
    status = status + " LB: " + std::to_string(motor_front_left_bottom_speed);
    status = status + " LT: " + std::to_string(motor_front_left_top_speed) + " ]\n";

    status = status + "  TILT Srv: " + std::to_string(tilt_servo_angle) + "\n";
    status = status + "  ELEVONS [ ONE: " + std::to_string(elevon_one_angle) + " TWO: " + std::to_string(elevon_two_angle) + " ]\n";
    status = status + "  GEAR Srv: " + std::to_string(landing_gear_servo_angle) + "\n";
    status = status + "  LOCATION [ LAT: " + std::to_string(gpsPtr->getLatitude());
    status = status + " LNG: " + std::to_string(gpsPtr->getLongitude());
    status = status + " ALT: " + std::to_string(gpsPtr->getAltitude()) + " ]\n";
    mtx.unlock();
    return status;
}