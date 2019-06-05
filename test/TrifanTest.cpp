#include <iostream>
#include <fstream>
#include <assert.h>     /* assert */
#include "../src/FlightController.h"
#include "../src/FlightLog.h"

bool TestMotorInit()
{
    FlightController ctrl;
    int speed = ctrl.getMotorSpeed(0);
    assert(speed == 0);
    return true;
}

bool TestFlightStatusMessage()
{
    FlightController ctrl;
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
    FlightController ctrl;
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

int main ()
{
    TestMotorInit();
    TestFlightStatusMessage();
    TestFlightLogging();
    std::cout << "----------\n" << "All Tests Pass!\n" << "----------\n";
    return 0;
}