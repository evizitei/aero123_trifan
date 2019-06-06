
enum gps_sim_type { stable, flaky, broken };

class Gps
{
private:
    gps_sim_type sim_t;    

public:
    Gps(gps_sim_type sim);
};