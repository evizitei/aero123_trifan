#include <iostream>
#include <fstream>

//just a forward declaration
class FlightController;

class FlightLog
{
private:
    FlightController* ctrl;
    std::ofstream logStream;
    std::string logFileName;
    bool shouldClose;
    std::mutex mtx;
    void logTime();
    void logMessage(std::string message);

public:
    FlightLog(FlightController* cPtr, std::string fn);
    void run();
    void signalStop();
};