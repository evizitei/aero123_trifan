#include <mutex>
#include <string>

struct PropConfig {
    int bottom_speed;
    int top_speed;
};

struct TrifanMotorConfig {
    PropConfig right_prop;
    PropConfig back_prop;
    PropConfig left_prop;
};

class Gps;

/*
internal members are confirgured to match
the main hardware outputs from a VTOL tiltrotor
aircraft in PX4:
https://docs.px4.io/en/airframes/airframe_reference.html#vtol-tiltrotor
*/
class FlightController
{
private:
    Gps* gpsPtr;
    int motor_front_right_bottom_speed;
    int motor_front_right_top_speed;
    int motor_back_bottom_speed;
    int motor_back_top_speed;
    int motor_front_left_bottom_speed;
    int motor_front_left_top_speed;
    int tilt_servo_angle;
    int elevon_right_angle;
    int elevon_left_angle;
    int landing_gear_servo_angle;
    double takeoff_alt;
    std::mutex mtx;

public:
    FlightController(Gps* gps);
    int getMotorSpeed(int motorIndex);
    int getTiltAngle();
    int getElevonAngle(int elevonIndex);
    double getTakeoffAlt();
    void updateMotors(TrifanMotorConfig conf);
    void updateMotors(int rpm);
    void setGearSrv(int degrees);
    void tiltProps(int degrees);
    void setElevons(int angle);
    void setTakeoffAlt(double alt);
    std::string getStatus();
};
