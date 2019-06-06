#include <mutex>
enum gps_sim_type { stable, flaky, broken };

class Gps
{
private:
    double lat;
    double lng;
    double alt; // altitude is in "feet"
    gps_sim_type sim_t;
    std::mutex mtx;

public:
    Gps(gps_sim_type sim);
    double getLatitude();
    double getLongitude();
    double getAltitude();
    // THESE METHODS ARE FOR THE SIMULATER
    // AIRFRAME SHOULD NOT ACCESS AT ALL!
    void updateAltitude(double delta);
};