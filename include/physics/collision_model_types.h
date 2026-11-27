#ifndef PHYSICS_COLLISION_MODEL_TYPES_H
#define PHYSICS_COLLISION_MODEL_TYPES_H

struct s_model_definition;
struct collision_model_definition;
struct real_matrix4x3;

struct collision_model_instance
{
    long object_index;
    s_model_definition *model;
    collision_model_definition *collision_model;
    char *region_permutations;
    real_matrix4x3 *node_matrices;
};

typedef char collision_model_instance_size[(sizeof(void *) != 4 || sizeof(collision_model_instance) == 0x14) ? 1 : -1];

#endif // PHYSICS_COLLISION_MODEL_TYPES_H
