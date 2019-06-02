#include "FlightController.h"
#include <iostream>
#include <string>

int main()
{
    std::cout << "Hello, PX4!\n";
    FlightController ctrl;
    std::cout << ctrl.getStatus() << "\n";
    return 0;
}