#ifndef PHYSICS_PHYSICS_MODEL_INSTANCE_H
#define PHYSICS_PHYSICS_MODEL_INSTANCE_H

#include <stddef.h>
#include "math/matrix_math_types.h"
#include "models/model_types.h"
#include "physics/physics_model_types.h"

struct s_physics_model_instance
{
    long object_index;
    long component_index;
    real_matrix4x3 world_matrix;
    long model_index;
    long physics_model_index;
    s_model_definition *model;
    s_physics_model_definition *physics_model;
    char *regions;
    real_matrix4x3 *node_matrices;

    // March physics_models.cpp:92 (82c10db8).
    __forceinline const real_matrix4x3 *get_node_matrix(long node) const
    {
        return node == -1 ? &world_matrix : &node_matrices[node];
    }
};

typedef char physics_model_instance_size[(sizeof(void *) != 4 || sizeof(s_physics_model_instance) == 0x54) ? 1 : -1];
typedef char physics_model_instance_physics_offset[(sizeof(void *) != 4 || offsetof(s_physics_model_instance, physics_model) == 0x48) ? 1 : -1];

#endif // PHYSICS_PHYSICS_MODEL_INSTANCE_H
