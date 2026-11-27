#ifndef PHYSICS_COLLISION_MODEL_BSP_ITERATOR_INTERNALS_H
#define PHYSICS_COLLISION_MODEL_BSP_ITERATOR_INTERNALS_H

#include "models/model_types.h"

struct s_collision_model_region_cursor
{
    signed char collision_region_index;
    signed char physics_region_index;
    char alignment_bytes[2];
    long permutation_count;
    s_model_region_permutation *permutations;
};

#endif // PHYSICS_COLLISION_MODEL_BSP_ITERATOR_INTERNALS_H
