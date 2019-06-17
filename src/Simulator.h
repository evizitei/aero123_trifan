#include <mutex>
#include <string>
#include <iostream>
#include <fstream>

class FlightController;
class Gps;
class FlightLog;
class Gyroscope;

namespace TextSim {
    double computeHeading(double cur_heading, double roll_angle);
}

class Simulator
{
private:
    FlightController* ctrl;
    Gps* gps;
    Gyroscope* gyro;
    bool shouldStop;
    std::string logFileName;
    std::ofstream logStream;
    double aoa; // in degrees, 0 is level, positive is above plane of wing, negative below.
    double heading; // in degrees, 0 is east, think unit circle
    std::mutex mtx;
    void simulateHover();
    void simulateForwardFlight();
    void simulateTransitionalFlight();
    void simulateCoordinateMotion(double lateral_component);
    void logTime();
    void logMessage(std::string message);

public:
    Simulator(FlightController* cPtr, Gps* gpsPtr, Gyroscope* gyro);
    void run();
    void signalStop();
    std::string getStatus();
};