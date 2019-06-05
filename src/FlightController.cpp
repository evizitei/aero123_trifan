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

int FlightController::getMotorSpeed(int motorIndex)
{
    switch (motorIndex)
    {
        case 0:
            return motor_front_right_bottom_speed;
        case 1:
            return motor_front_right_top_speed;
        case 2:
            return motor_back_bottom_speed;
        case 3:
            return motor_back_top_speed;
        case 4:
            return motor_front_left_bottom_speed;
        case 5:
            return motor_front_left_top_speed;
        default:
            return -1;
    }
}

void FlightController::updateMotors(TrifanMotorConfig conf)
{
    motor_front_right_bottom_speed = conf.right_prop.bottom_speed;
    motor_front_right_top_speed = conf.right_prop.top_speed;
    motor_back_bottom_speed = conf.back_prop.bottom_speed;
    motor_back_top_speed = conf.back_prop.top_speed;
    motor_front_left_bottom_speed = conf.left_prop.bottom_speed;
    motor_front_left_top_speed = conf.left_prop.top_speed;
}

std::string FlightController::getStatus()
{
    std::string status("FLIGHT STATUS\n");
    status = status + "  MOTORS[ RB: " + std::to_string(motor_front_right_bottom_speed);
    status = status + " RT: " + std::to_string(motor_front_right_top_speed);
    status = status + " BB: " + std::to_string(motor_back_bottom_speed);
    status = status + " BT: " + std::to_string(motor_back_top_speed);
    status = status + " LB: " + std::to_string(motor_front_left_bottom_speed);
    status = status + " LT: " + std::to_string(motor_front_left_top_speed) + " ]\n";

    status = status + "  TILT Srv: " + std::to_string(tilt_servo_angle) + "\n";
    status = status + "  ELEVONS [ ONE: " + std::to_string(elevon_one_angle) + " TWO: " + std::to_string(elevon_two_angle) + " ]\n";
    status = status + "  GEAR Srv: " + std::to_string(landing_gear_servo_angle) + "\n";
    return status;
}