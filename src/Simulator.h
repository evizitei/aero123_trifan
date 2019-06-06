#include <mutex>
class FlightController;
class Gps;

class Simulator
{
private:
    FlightController* ctrl;
    Gps* gps;
    bool shouldStop;
    std::mutex mtx;

public:
    Simulator(FlightController* cPtr, Gps* gpsPtr);
    void run();
    void signalStop();
};