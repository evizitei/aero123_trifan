#include <mutex>

class Gyroscope
{
private:
    double roll_angle;
    double pitch_angle;
    std::mutex mtx;

public:
    Gyroscope();
    double getRoll();
    double getPitch();
    // THESE METHODS ARE FOR THE SIMULATER
    // AIRFRAME SHOULD NOT ACCESS AT ALL!
    void updateOrientation(double pitch, double roll);
};