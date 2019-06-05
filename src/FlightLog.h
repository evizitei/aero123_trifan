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

public:
    FlightLog(FlightController* cPtr, std::string fn);
    /* generally, just use "run", unless you want 
    fine grained contorl over when and
    where logging events happen.*/
    void run();
    void close();
    void logTime();
    void logMessage(std::string message);
    void signalStop();
};