#include "FlightController.h"
#include "FlightLog.h"
#include "Gps.h"
#include "Simulator.h"
#include <iostream>
#include <string>
#include <thread>

void loggerThread(FlightLog* fPtr){
    fPtr->run();
}

void simThread(Simulator* sim){
    sim->run();
}

int main()
{
    std::cout << "Trifan initializing...\n";
    Gps* gps = new Gps(stable);
    FlightController* ctrl = new FlightController(gps);
    std::string log_name = "flight.log";
    int log_interval = 3; // write status ever 3 seconds
    FlightLog* fLog = new FlightLog(ctrl, log_name);
    Simulator* sim = new Simulator(ctrl, gps);
    std::cout << ctrl->getStatus() << "\n" << "...Initialized!\n" << "Logging to " << log_name << "\n\n";
    std::string flight_command;
    std::thread lt(loggerThread, fLog);
    std::thread st(simThread, sim);
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
    sim->signalStop();
    lt.join();
    st.join();
    delete ctrl;
    return 0;
}