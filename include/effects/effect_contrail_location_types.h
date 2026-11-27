#ifndef EFFECTS_EFFECT_CONTRAIL_LOCATION_TYPES_H
#define EFFECTS_EFFECT_CONTRAIL_LOCATION_TYPES_H

#include <stddef.h>
#include "memory/data_types.h"
#include "math/real_math_types.h"

// Locations of effect contrail profiles, allocated by particle_new_location_from_spawn.
// New locations copy position and forward direction from the spawn matrix;
// effect_update_contrail_profile and both GPU submit paths use the same 44-byte datum.
struct s_effect_contrail_location
{
    s_datum_header header;
    short pad2;
    long next_index;
    long owner_index;
    long first_instance_index;
    long pad10;
    real_point3d position;
    real_vector3d direction;
};

typedef char effect_contrail_location_size[(sizeof(s_effect_contrail_location) == 44) ? 1 : -1];
typedef char effect_contrail_location_owner_offset[(offsetof(s_effect_contrail_location, owner_index) == 8) ? 1 : -1];
typedef char effect_contrail_location_instance_offset[(offsetof(s_effect_contrail_location, first_instance_index) == 12) ? 1 : -1];
typedef char effect_contrail_location_position_offset[(offsetof(s_effect_contrail_location, position) == 20) ? 1 : -1];
typedef char effect_contrail_location_direction_offset[(offsetof(s_effect_contrail_location, direction) == 32) ? 1 : -1];

// The data array is installed by effects.cpp and relocated by particle.cpp.
#include "effects/effect_runtime_storage.h"

#endif // EFFECTS_EFFECT_CONTRAIL_LOCATION_TYPES_H
