#include "FlightController.h"
#include <string>

FlightController::FlightController()
{
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
}

std::string FlightController::getStatus()
{
    std::string status("Put status output here...");
    return status;
}