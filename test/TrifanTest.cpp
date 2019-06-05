#include <iostream>
#include <assert.h>     /* assert */
#include "../src/FlightController.h"

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
    assert(flightStat.find("[ RB: 3 RT: 3 BB: 3 BT: 3 LB: 3 LT: 3 ]"));
    return true;
}

int main ()
{
    TestMotorInit();
    TestFlightStatusMessage();
    std::cout << "----------\n" << "All Tests Pass!\n" << "----------\n";
    return 0;
}