#ifndef PHYSICS_COLLISION_MODEL_DEFINITION_TYPES_H
#define PHYSICS_COLLISION_MODEL_DEFINITION_TYPES_H
#include "physics/collision_bsp_types.h"
#include "tag_files/tag_types.h"
#include "math/real_math_types.h"

struct pathfinding_sphere
{
    short node;
    unsigned short flags;
    real_point3d center;
    float radius;
};
typedef char pathfinding_sphere_size[(sizeof(pathfinding_sphere) == 0x14) ? 1 : -1];
struct s_collision_model_bsp {
    short node_index;
    short unused;
    collision_bsp bsp;
};
struct s_collision_model_permutation {
    long name;
    s_tag_block bsps;
    s_tag_block physics;
    s_tag_block mopp_codes;
};
struct s_collision_model_region {
    long name;
    s_tag_block permutations;
};
struct collision_model_definition {
    long : 32;
    long : 32;
    long : 32;
    long : 32;
    long flags;
    long : 32;
    long : 32;
    long : 32;
    s_tag_block regions;
    s_tag_block pathfinding_spheres;
};
typedef char collision_model_bsp_size[(sizeof(void *) != 4 || sizeof(s_collision_model_bsp) == 100) ? 1 : -1];
typedef char collision_model_permutation_size[(sizeof(void *) != 4 || sizeof(s_collision_model_permutation) == 40) ? 1 : -1];
typedef char collision_model_region_size[(sizeof(void *) != 4 || sizeof(s_collision_model_region) == 16) ? 1 : -1];
typedef char collision_model_definition_size[(sizeof(void *) != 4 || sizeof(collision_model_definition) == 56) ? 1 : -1];
typedef char collision_model_pathfinding_spheres_offset[(offsetof(collision_model_definition, pathfinding_spheres) == 0x2c) ? 1 : -1];
typedef char collision_model_bsp_bsp_offset[(offsetof(s_collision_model_bsp, bsp) == 4) ? 1 : -1];
typedef char collision_model_permutation_physics_offset[(offsetof(s_collision_model_permutation, physics) == 16) ? 1 : -1];
typedef char collision_model_permutation_mopp_codes_offset[(offsetof(s_collision_model_permutation, mopp_codes) == 28) ? 1 : -1];
typedef char collision_model_definition_flags_offset[(offsetof(collision_model_definition, flags) == 16) ? 1 : -1];
typedef char collision_model_definition_regions_offset[(offsetof(collision_model_definition, regions) == 32) ? 1 : -1];

#endif
