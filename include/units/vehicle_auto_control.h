#ifndef UNITS_VEHICLE_AUTO_CONTROL_H
#define UNITS_VEHICLE_AUTO_CONTROL_H

#include <stddef.h>

#include "objects/object_types.h"

struct s_vehicle_auto_control {
    long target_object_index;
    long target_time;
    long minimum_range_start_time;
    long target_lost_time;
    s_damage_owner damage_owner;
    bool active;
};

// Auto-turret control extends the base state with a trigger volume and range limits.
struct s_vehicle_auto_turret_control : s_vehicle_auto_control
{
    long m_volume_index;
    float m_min_range;
    float m_max_range;
    float m_lost_seconds;
};

typedef char vehicle_auto_turret_control_size[(sizeof(s_vehicle_auto_turret_control) == 0x30) ? 1 : -1];

typedef char vehicle_auto_control_size[(sizeof(s_vehicle_auto_control) == 0x20) ? 1 : -1];
typedef char vehicle_auto_control_owner_offset[(offsetof(s_vehicle_auto_control, damage_owner) == 0x10) ? 1 : -1];
typedef char vehicle_auto_control_active_offset[(offsetof(s_vehicle_auto_control, active) == 0x1c) ? 1 : -1];

#endif // UNITS_VEHICLE_AUTO_CONTROL_H
