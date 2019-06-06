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

void printHelp()
{
    std::cout << "************ TRIFAN HELP *************** \n";
    std::cout << "Command Options: \n";
    std::cout << "  'help'     -> print this message \n";
    std::cout << "  'shutdown' -> kill the program \n";
    std::cout << "  'takeoff'  -> turn motors up to gain hover altitude\n";
    std::cout << "  'land'     -> turn motors down to shed hover altitude\n";
    std::cout << "  'status'   -> print flight log current entry\n";
    std::cout << "**************************************** \n\n\n";
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
    std::cout << "\n" << "...Initialized!\n" << "Logging to " << log_name << "\n\n";
    printHelp();
    std::string flight_command;
    // background threads
    // the first one writes to the flight log every three seconds
    std::thread lt(loggerThread, fLog);
    // this one updates the GPS position from the
    // flight configuration (approximately)
    std::thread st(simThread, sim);
    while(true) {
        std::cout << "Input command...\n";
        std::getline(std::cin, flight_command);
        std::cout << "Executing, " << flight_command << "\n";
        if(flight_command == "shutdown")
        {
            std::cout << "Aborting...\n";
            break;
        }
        else if(flight_command == "help")
        {
            printHelp();
        }
        else if(flight_command == "takeoff")
        {
            // probably should check arm/disarm here and abort if
            // disarmed
            ctrl->updateMotors(3500);
        }
        else if(flight_command == "land")
        {
            // need handling for what if in transitional or forward flight,
            // just turning the motors down is insufficient to land.
            ctrl->updateMotors(2500);
        }
        else if(flight_command == "status")
        {
            std::cout << ctrl->getStatus();
        }
        else
        {
            std::cout << "NO SUCH COMMAND!! " << flight_command << "\n\n";
            printHelp();
        }
        
    }
    fLog->signalStop();
    sim->signalStop();
    lt.join();
    st.join();
    delete ctrl;
    return 0;
}