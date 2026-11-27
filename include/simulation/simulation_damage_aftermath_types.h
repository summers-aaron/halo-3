#ifndef SIMULATION_SIMULATION_DAMAGE_AFTERMATH_TYPES_H
#define SIMULATION_SIMULATION_DAMAGE_AFTERMATH_TYPES_H

#include "math/real_math_types.h"

struct s_simulation_damage_aftermath_event_data
{
    long damage_definition_index;
    long damage_owner_object_index;
    short damage_owner_player_index;
    bool direction_valid;
    real_vector3d direction;
    bool epicenter_valid;
    real_vector3d epicenter_direction;
    float epicenter_distance;
    float scale;
    float shake_scale;
    unsigned long flags;
    float shield_damage;
    float body_damage;
    short body_part;
    short node_index;
    long ping_type;
    long special_death_type;
    unsigned char damage_reporting_info;
};

typedef char s_simulation_damage_aftermath_event_data_size[(sizeof(s_simulation_damage_aftermath_event_data) == 0x50) ? 1 : -1];

#endif // SIMULATION_SIMULATION_DAMAGE_AFTERMATH_TYPES_H
