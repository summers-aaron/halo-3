#ifndef UNITS_VEHICLE_COUNTERS_H
#define UNITS_VEHICLE_COUNTERS_H

struct c_vehicle_stop_counter
{
    unsigned char m_ticks;
    void update(long vehicle_index, float throttle, bool force);
    float calculate_fraction(long vehicle_index);
};
struct c_vehicle_braking_counter
{
    unsigned char m_ticks;
    bool is_braking(long vehicle_index);
    void update(long vehicle_index);
};

#endif // UNITS_VEHICLE_COUNTERS_H
