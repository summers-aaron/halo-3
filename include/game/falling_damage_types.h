#ifndef GAME_FALLING_DAMAGE_TYPES_H
#define GAME_FALLING_DAMAGE_TYPES_H

#include <stddef.h>
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"

// Halo 3's 0x78-byte falling-damage tag block. March hs_damage_object reads
// the script_damage and distance_damage tag references.
struct s_falling_damage_definition
{
    real_bounds harmful_falling_distance;
    s_tag_reference falling_damage;
    s_tag_reference jumping_damage;
    s_tag_reference soft_landing_damage;
    s_tag_reference hard_landing_damage;
    s_tag_reference script_damage;
    float maximum_falling_distance;
    s_tag_reference distance_damage;
    float runtime_maximum_falling_velocity;
    real_bounds runtime_damage_velocity;
};

typedef char falling_damage_definition_size[(sizeof(s_falling_damage_definition) == 0x78) ? 1 : -1];
typedef char falling_damage_script_index_offset[(offsetof(s_falling_damage_definition, script_damage) + offsetof(s_tag_reference, index) == 0x54) ? 1 : -1];
typedef char falling_damage_distance_index_offset[(offsetof(s_falling_damage_definition, distance_damage) + offsetof(s_tag_reference, index) == 0x68) ? 1 : -1];

#endif // GAME_FALLING_DAMAGE_TYPES_H
