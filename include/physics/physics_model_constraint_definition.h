#ifndef PHYSICS_PHYSICS_MODEL_CONSTRAINT_DEFINITION_H
#define PHYSICS_PHYSICS_MODEL_CONSTRAINT_DEFINITION_H
#include "math/matrix_math_types.h"
#include <stddef.h>

// Constraint records share two scaled frames and their node-edge reference.
struct s_physics_model_constraint_bodies
{
    long name;
    short node_a;
    short node_b;
    real_matrix4x3 transform_a;
    real_matrix4x3 transform_b;
    short node_edge_index;
};
typedef char physics_constraint_bodies_size[(sizeof(s_physics_model_constraint_bodies) == 0x74) ? 1 : -1];
typedef char physics_constraint_bodies_transform_a_offset[(offsetof(s_physics_model_constraint_bodies, transform_a) == 8) ? 1 : -1];
typedef char physics_constraint_bodies_transform_b_offset[(offsetof(s_physics_model_constraint_bodies, transform_b) == 0x3c) ? 1 : -1];
typedef char physics_constraint_bodies_node_edge_offset[(offsetof(s_physics_model_constraint_bodies, node_edge_index) == 0x70) ? 1 : -1];

typedef s_physics_model_constraint_bodies s_physics_model_constraint_definition;

#endif
