#ifndef UNITS_VEHICLE_TYPE_TURRET_H
#define UNITS_VEHICLE_TYPE_TURRET_H

#include "units/vehicle_auto_control.h"
#include <stddef.h>

// Auto-turret parameters: shared control record, trigger volume, range limits, and lost-target timeout.
class c_vehicle_type_turret
{
public:
    s_vehicle_auto_control m_auto_control;
    long m_volume_index;
    float m_min_range;
    float m_max_range;
    float m_lost_seconds;

    bool init_auto_turret(long object_index, long trigger, float minimum_range,
        float maximum_range, float angle, long owner_index);
    bool auto_control(long vehicle_index);
    __forceinline void update_control(long vehicle_index) {}
    __forceinline bool compute_function_value(long vehicle_index, long function, float *value, bool *active) { return false; }
    __forceinline bool kills_riders_at_terminal_velocity(long object_index) { return true; }
    __forceinline bool should_override_deactivation(long object_index) { return false; }

private:
    void update_auto_target(long vehicle_index);
    void track_auto_target(long vehicle_index);
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char vehicle_type_turret_size[(sizeof(c_vehicle_type_turret) == 0x30) ? 1 : -1];
typedef char vehicle_type_turret_volume_offset[(offsetof(c_vehicle_type_turret, m_volume_index) == 0x20) ? 1 : -1];
#endif

extern "C" float vehicle_type_turret_compute_target_weight(c_vehicle_type_turret *turret, int a, int b);

// Five-degree tracking threshold for auto-turret target tracking.
extern "C" const double k_turret_track_angle;

#endif // UNITS_VEHICLE_TYPE_TURRET_H
