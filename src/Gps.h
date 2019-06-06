
enum gps_sim_type { stable, flaky, broken };

class Gps
{
private:
    double lat;
    double lng;
    double alt;
    gps_sim_type sim_t;    

public:
    Gps(gps_sim_type sim);
    double getLatitude();
    double getLongitude();
    double getAltitude();
};