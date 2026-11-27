#ifndef OBJECTS_CRATE_DEFINITIONS_H
#define OBJECTS_CRATE_DEFINITIONS_H

#include <stddef.h>
#include "math/real_math_types.h"
#include "objects/object_definitions.h"
#include "tag_files/tag_types.h"

struct s_crate_definition
{
    s_object_definition object;
    unsigned short crate_flags;
    short tight_bounds_to_collision_region;
    s_tag_block campaign_metagame_bucket;
    long self_destruction_timer;
};

typedef char crate_definition_flags_offset[(offsetof(s_crate_definition, crate_flags) == 0xf8) ? 1 : -1];
typedef char crate_definition_timer_offset[(offsetof(s_crate_definition, self_destruction_timer) == 0x108) ? 1 : -1];

struct s_crate_rigid_body_definition
{
    short node_index;
    short region_index;
    short permutation_index;
    short serialized_shapes_offset;
    real_point3d bounding_sphere_offset;
    float bounding_sphere_radius;
    unsigned short flags;
    short dynamic_motion_type;
    short alternate_rigid_body_index;
    short motion_type;
    float inertia_tensor_scale;
    float linear_damping;
    float angular_damping;
    real_vector3d center_of_mass_offset;
    long shape_storage[30];
};

typedef char crate_rigid_body_motion_offset[(offsetof(s_crate_rigid_body_definition, motion_type) == 30) ? 1 : -1];
typedef char crate_rigid_body_size[(sizeof(s_crate_rigid_body_definition) == 176) ? 1 : -1];

#endif // OBJECTS_CRATE_DEFINITIONS_H
