#include "FlightController.h"
#include "FlightLog.h"
#include <iostream>
#include <string>
#include <thread>

void loggerThread(FlightLog* fPtr){
    fPtr->run();
}
int main()
{
    std::cout << "Trifan initializing...\n";
    FlightController* ctrl = new FlightController;
    std::string log_name = "flight.log";
    int log_interval = 3; // write status ever 3 seconds
    FlightLog* fLog = new FlightLog(ctrl, log_name);
    std::cout << ctrl->getStatus() << "\n" << "...Initialized!\n" << "Logging to " << log_name << "\n\n";
    std::string flight_command;
    std::thread lt(loggerThread, fLog);
    while(true) {
        std::cout << "Input command...\n";
        std::getline(std::cin, flight_command);
        std::cout << "Executing, " << flight_command << "\n";
        if(flight_command == "shutdown"){
            std::cout << "Aborting...\n";
            break;
        }
    }
    fLog->signalStop();
    lt.join();
    delete ctrl;
    return 0;
}