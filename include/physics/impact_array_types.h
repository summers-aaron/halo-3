#ifndef PHYSICS_IMPACT_ARRAY_TYPES_H
#define PHYSICS_IMPACT_ARRAY_TYPES_H

#include "memory/data_types.h"

struct s_impact_array {
    unsigned short identifier;
    short impact_count;
    long impact_indices[32];
    long object_index;
};

#endif // PHYSICS_IMPACT_ARRAY_TYPES_H
