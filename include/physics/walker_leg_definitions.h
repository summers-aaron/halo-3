#ifndef PHYSICS_WALKER_LEG_DEFINITIONS_H
#define PHYSICS_WALKER_LEG_DEFINITIONS_H

#include <stddef.h>
#include "math/real_math_types.h"
#include "math/matrix_math_types.h"
#include "tag_files/tag_types.h"

struct s_walker_leg_definition
{
    signed char group;
    signed char leg_side;
    signed char height;
    bool enabled;
    long hip_node_a_marker;
    long hip_node_b_marker;
    long ankle_node_a_marker;
    long ankle_node_b_marker;
    long knee_marker_names[16];
    unsigned long leg_flags;
    real_vector3d vector;
    real_vector3d knee_axis;
    float bone_length_a;
    float bone_length_b;
    float ankle_offset;
    float lift_height;
    short runtime_marker_index;
    short node;
    short node_a;
    short node_b;
    real_point3d planted_position;
    real_vector3d planted_normal;
};

typedef char walker_leg_vector_offset[(offsetof(s_walker_leg_definition, vector) == 0x58) ? 1 : -1];
typedef char walker_leg_bone_length_a_offset[(offsetof(s_walker_leg_definition, bone_length_a) == 0x70) ? 1 : -1];
typedef char walker_leg_node_offset[(offsetof(s_walker_leg_definition, node) == 0x82) ? 1 : -1];
typedef char walker_leg_definition_size[(sizeof(s_walker_leg_definition) == 0xa0) ? 1 : -1];

struct s_walker_constraint_bodies
{
    long name;
    short rigid_body_a;
    short rigid_body_b;
    real_matrix4x3 bodies[2];
};

#endif // PHYSICS_WALKER_LEG_DEFINITIONS_H
