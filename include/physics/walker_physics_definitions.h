#ifndef PHYSICS_WALKER_PHYSICS_DEFINITIONS_H
#define PHYSICS_WALKER_PHYSICS_DEFINITIONS_H

#include <stddef.h>

#include "math/real_math_types.h"
#include "tag_files/tag_types.h"

struct s_walker_leg_definition;

// Shared by walker_physics and vehicle_type_mantis. Middle tuning fields named from
// TagTool Vehicle.WalkerPhysicsDefinition (0x78 total; mantis holds the 0x38 tail).
struct s_walker_physics_definition
{
    float forward_stride;
    float lateral_stride;
    float leg_lift;
    float maximum_turn;
    long leg_count;
    TAG_POINTER(s_walker_leg_definition) legs;
    TAG_POINTER(void) leg_block_definition;
    float leg_apex_draction;
    float lift_exponent;
    float drop_exponent;
    real_vector3d object_space_pivot_position;
    float overtravel;
};

typedef char walker_definition_legs_offset[(offsetof(s_walker_physics_definition, legs) == 0x14) ? 1 : -1];
typedef char walker_definition_overtravel_offset[(offsetof(s_walker_physics_definition, overtravel) == 0x34) ? 1 : -1];
typedef char walker_definition_size[(sizeof(s_walker_physics_definition) == 0x38) ? 1 : -1];

#endif
