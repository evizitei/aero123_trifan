#include "FlightController.h"
#include <iostream>
#include <string>

int main()
{
    std::cout << "Trifan initializing...\n";
    FlightController* ctrl = new FlightController;
    std::cout << ctrl->getStatus() << "\n" << "...Initialized!\n";
    std::string flight_command;
    while(true) {
        std::cout << "Input command...\n";
        std::getline(std::cin, flight_command);
        std::cout << "Executing, " << flight_command << "\n";
        if(flight_command == "shutdown"){
            std::cout << "Aborting...\n";
            break;
        }
    }
    delete ctrl;
    return 0;
}