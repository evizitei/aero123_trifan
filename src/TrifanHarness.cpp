#include "FlightController.h"
#include "FlightLog.h"
#include "Gps.h"
#include "Simulator.h"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <regex>

void loggerThread(FlightLog* fPtr){
    fPtr->run();
}

void simThread(Simulator* sim){
    sim->run();
}

void ctrlThread(FlightController* fCtrl){
    fCtrl->run();
}

void printHelp()
{
    std::cout << "************ TRIFAN HELP *************** \n";
    std::cout << "Command Options: \n";
    std::cout << "  'help'           -> print this message \n";
    std::cout << "  'arm'            -> turn motors on to idle \n";
    std::cout << "  'disarm'         -> turn motors off \n";
    std::cout << "  'shutdown'       -> kill the program \n";
    std::cout << "  'takeoff [alt]'  -> turn motors up to gain hover altitude\n";
    std::cout << "  'land'           -> turn motors down to shed hover altitude\n";
    std::cout << "  'forward_flight' -> flip propellors to 90deg (forward)\n";
    std::cout << "  'rotors_forward' -> nudge propellors towards forward flight\n";
    std::cout << "  'rotors_up'      -> nudge propellors towards hover\n";
    std::cout << "  'hover'          -> flip props to 0 (hover) \n";
    std::cout << "  'elvs_up'        -> nudge angle of elevons for climb\n";
    std::cout << "  'elvs_down'      -> nudge angle of elevons for dive\n";
    std::cout << "  'elvs_level'     -> snap elevons to neutral\n";
    std::cout << "  'throttle_up'    -> turn motors up by 250 RPM\n";
    std::cout << "  'throttle_down'  -> turn motors down by 250 RPM\n";
    std::cout << "  'gear_up'        -> stow landing gear\n";
    std::cout << "  'gear_down'      -> deploy landing gear\n";
    std::cout << "  'status'         -> print flight log current entry\n";
    std::cout << "**************************************** \n\n\n";
}

bool carryOutCommand(FlightController* ctrl, Simulator* sim, std::string flight_command)
{
    if(flight_command == "shutdown") {
        std::cout << "Aborting...\n";
        return true;
    } else if(flight_command == "help") {
        printHelp();
    } else if(flight_command == "arm") {
        std::string cur_state = ctrl->getState();
        if(cur_state == "off" || cur_state == "disarmed") {
            ctrl->setState("arming", 500);
        } else {
            std::cout << "ERROR STATE:Unable to arm in this state...\n";
            std::cout << ctrl->getStatus();
        }
    } else if(flight_command == "disarm") {
        std::string cur_state = ctrl->getState();
        if(cur_state == "armed" || cur_state == "landed") {
            ctrl->setState("disarming", 0);
        } else {
            std::cout << "ERROR STATE:Unable to disarm in this state...\n";
            std::cout << ctrl->getStatus();
        }
    } else if(flight_command.substr(0, 7) == "takeoff") {
        std::regex r("\\b\\d+");
        std::string takeoff_command;
        std::smatch match;
        if (std::regex_search(flight_command, match, r) != true){
            std::cout << "ERROR STATE: You must provide an altitude to takeoff!\n\n";
            return false;
        }
        std::string alt_param_str = match.str(0);
        int alt_param = std::stod(alt_param_str);
        if(ctrl->getState() == "armed"){
            ctrl->setState("hover_climbing", alt_param);
        }else{
            std::cout << "ERROR STATE: Unable to takeoff in this state...\n";
            std::cout << ctrl->getStatus();
        }
    } else if(flight_command == "land") {
        if(ctrl->getTiltAngle() == 0) {
            ctrl->setState("landing", 0);
        } else {
            std::cout << "Please return to hover, before attempting to land. \n";
        }
    } else if(flight_command == "forward_flight") {
        // snap props to forward configuration
        ctrl->tiltProps(90);
    } else if(flight_command == "rotors_forward") {
        ctrl->tiltProps(ctrl->getTiltAngle() + 10);
    } else if(flight_command == "rotors_up") {
        ctrl->tiltProps(ctrl->getTiltAngle() - 10);
    } else if(flight_command == "hover") {
        // snap props to hover configuration
        ctrl->tiltProps(0);
    } else if(flight_command == "elvs_up") {
        // use elevons to gain altitude
        ctrl->setElevons(ctrl->getElevonAngle(0) + 3);
    } else if(flight_command == "elvs_down") {
        // use elevons to shed altitude
        ctrl->setElevons(ctrl->getElevonAngle(0) - 3);
    } else if(flight_command == "elvs_level") {
        // bring elevons to neutral
        ctrl->setElevons(0);
    } else if(flight_command == "throttle_up") {
        // increments motor speed by 250 RPM
        ctrl->updateMotors(ctrl->getMotorSpeed(0) + 250);
    } else if(flight_command == "throttle_down") {
        // decrements motor speed by 250 RPM
        ctrl->updateMotors(ctrl->getMotorSpeed(0) - 250);
    } else if(flight_command == "gear_up") {
        ctrl->setGearSrv(90);
    } else if(flight_command == "gear_down") {
        ctrl->setGearSrv(0);
    } else if(flight_command == "status") {
        std::cout << ctrl->getStatus() << "\n" << sim->getStatus() << "\n";
    } else {
        std::cout << "NO SUCH COMMAND!! " << flight_command << "\n\n";
        printHelp();
    }
    return false;
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
    // this thread manages state transitions
    // and feedback loops within the controller itself.
    std::thread ct(ctrlThread, ctrl);
    while(true) {
        std::cout << "Input command...\n";
        std::getline(std::cin, flight_command);
        std::cout << "Executing Command: " << flight_command << "\n";
        bool abort = carryOutCommand(ctrl, sim, flight_command);
        if(abort)
            break;
    }
    fLog->signalStop();
    sim->signalStop();
    ctrl->signalStop();
    lt.join();
    st.join();
    ct.join();
    delete ctrl;
    std::cout << "Safe shutdown complete.  Exiting. \n\n\n";
    return 0;
}