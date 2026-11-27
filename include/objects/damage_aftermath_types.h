#ifndef OBJECTS_DAMAGE_AFTERMATH_TYPES_H
#define OBJECTS_DAMAGE_AFTERMATH_TYPES_H

#include "objects/damage_owner.h"
#include "objects/object_types.h"
#include "math/real_math_types.h"

// Damage aftermath result shared by creatures and units.
struct s_damage_aftermath_result_data
{
    unsigned char reporting_info;
    char spare_before_flags[3];
    unsigned long flags;
    long damage_definition_index;
    union {
        s_damage_owner owner;
        struct {
            long owner_player_index;
            long damage_owner_object_index;
            signed char owner_team;
            char spare_after_owner_team[3];
        };
    };
    real_vector3d direction;
    real_point3d epicenter;
    float scale;
    float shake_scale;
    long body_part;
    unsigned short node_index;
    char spare_before_body_damage[2];
    float body_damage;
    float shield_damage;
    float distance;
    long ping_type;
    long response_metadata;
};

#endif // OBJECTS_DAMAGE_AFTERMATH_TYPES_H
