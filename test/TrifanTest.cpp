#include <iostream>
#include <assert.h>     /* assert */
#include "../src/FlightController.h"

int main ()
{
    FlightController ctrl;
    int speed = ctrl.getMotorSpeed(0);
    assert(speed == 0);
    std::cout << "----------\n" << "All Tests Pass!\n" << "----------\n";
    return 0;
}