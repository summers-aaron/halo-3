#ifndef PHYSICS_COLLISION_MODEL_BSP_ITERATOR_TYPES_H
#define PHYSICS_COLLISION_MODEL_BSP_ITERATOR_TYPES_H

#include "memory/data_types.h"

struct collision_model_definition;

struct collision_model_bsp_iterator {collision_model_definition *model;signed char permutations[16];unsigned long index;void *bsp;};

#endif // PHYSICS_COLLISION_MODEL_BSP_ITERATOR_TYPES_H
