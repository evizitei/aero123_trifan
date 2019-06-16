#include <iostream>
#include <fstream>
#include <assert.h>
#include "../src/FlightController.h"
#include "../src/FlightLog.h"
#include "../src/Gps.h"
#include "../src/Gyroscope.h"

bool TestMotorInit()
{
    Gps gps { stable };
    Gyroscope gyro { };
    FlightController ctrl { &gps, &gyro };
    int speed = ctrl.getMotorSpeed(0);
    assert(speed == 0);
    return true;
}

bool TestFlightStatusMessage()
{
    Gps gps { stable };
    Gyroscope gyro { };
    FlightController ctrl { &gps, &gyro };
    TrifanMotorConfig cfg = {
        { 3, 3},
        { 3, 3},
        { 3, 3}
    };
    ctrl.updateMotors(cfg);
    std::string flightStat = ctrl.getStatus();
    assert(flightStat.find("[ RB: 3 RT: 3 BB: 3 BT: 3 LB: 3 LT: 3 ]") > 0);
    return true;
}

bool TestFlightLogging() 
{
    Gps gps { stable };
    Gyroscope gyro { };
    FlightController ctrl { &gps, &gyro };
    FlightLog* fLog = new FlightLog(&ctrl, "test.log");
    fLog->logMessage("Test The Messaging");
    fLog->close();
    delete(fLog);
    std::ifstream testStream;
    testStream.open("test.log");
    std::string logLines;
    testStream >> logLines;
    assert(logLines.find("***************************\nTest The Messaging\n***************************") > 0);
    return true;
}

bool TestGpsInit()
{
    Gps gps { stable };
    assert(gps.getAltitude() == 0.0);
    assert(gps.getLatitude() == 38.9579);
    assert(gps.getLongitude() == -92.3264);
    return true;
}

int main ()
{
    TestMotorInit();
    TestFlightStatusMessage();
    TestFlightLogging();
    TestGpsInit();
    std::cout << "----------\n" << "All Tests Pass!\n" << "----------\n";
    return 0;
}