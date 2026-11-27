#ifndef PHYSICS_PHYSICS_MODEL_TYPES_H
#define PHYSICS_PHYSICS_MODEL_TYPES_H

#include <stddef.h>
#include "tag_files/tag_types.h"
#include "math/real_math_types.h"
#include "phys/phys_math_value_types.h"

class phys_shape;

struct s_physics_model_rigid_body
{
    short node;
    short region;
    short permutation;
    short serialized_shapes;
    real_point3d bounding_sphere_offset;
    float bounding_sphere_radius;
    unsigned short flags; // object_early_mover_get_node_and_rigid_body_index tests bit 4
    short motion_type;
    short alternate_rigid_body;
    short size;
    float inertia_tensor_scale;
    float linear_damping;
    float angular_damping;
    real_vector3d center_of_mass_offset;
    real_rectangle3d water_physics_bounds;
    long : 32;
    long : 32;
    TAG_POINTER(phys_shape) shape;
    float mass;
    phys_vec4 center_of_mass;
    phys_mat3 inertia_tensor;
    float margin;
    long : 32;
    long : 32;
    short : 16;
    unsigned short runtime_flags;
};

#include "physics/physics_model_definition_types.h"

typedef char physics_model_rigid_body_flags_offset[(offsetof(s_physics_model_rigid_body, flags) == 0x18) ? 1 : -1];

typedef char physics_model_rigid_body_size[(sizeof(s_physics_model_rigid_body) == 0xb0) ? 1 : -1];

typedef char physics_model_water_bounds_offset[(offsetof(s_physics_model_rigid_body, water_physics_bounds) == 0x38) ? 1 : -1];

typedef char physics_model_rigid_body_shape_offset[(offsetof(s_physics_model_rigid_body, shape) == 0x58) ? 1 : -1];

typedef char physics_model_rigid_body_serialized_shapes_offset[
    (offsetof(s_physics_model_rigid_body, serialized_shapes) == 0x6) ? 1 : -1];
typedef char physics_model_rigid_body_bounding_sphere_offset[
    (offsetof(s_physics_model_rigid_body, bounding_sphere_offset) == 0x8) ? 1 : -1];
typedef char physics_model_rigid_body_bounding_sphere_radius_offset[
    (offsetof(s_physics_model_rigid_body, bounding_sphere_radius) == 0x14) ? 1 : -1];
typedef char physics_model_rigid_body_center_of_mass_offset[
    (offsetof(s_physics_model_rigid_body, center_of_mass_offset) == 0x2c) ? 1 : -1];
typedef char physics_model_rigid_body_motion_offset[(offsetof(s_physics_model_rigid_body, motion_type) == 0x1a) ? 1 : -1];
typedef char physics_model_rigid_body_mass_offset[(offsetof(s_physics_model_rigid_body, mass) == 0x5c) ? 1 : -1];
typedef char physics_model_rigid_body_center_offset[(offsetof(s_physics_model_rigid_body, center_of_mass) == 0x60) ? 1 : -1];
typedef char physics_model_rigid_body_inertia_offset[(offsetof(s_physics_model_rigid_body, inertia_tensor) == 0x70) ? 1 : -1];
typedef char physics_model_rigid_body_margin_offset[(offsetof(s_physics_model_rigid_body, margin) == 0xa0) ? 1 : -1];
typedef char physics_model_rigid_body_runtime_flags_offset[(offsetof(s_physics_model_rigid_body, runtime_flags) == 0xae) ? 1 : -1];

#endif // PHYSICS_PHYSICS_MODEL_TYPES_H
