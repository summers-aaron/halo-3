#ifndef STRUCTURES_PREPLACED_DECAL_TYPES_H
#define STRUCTURES_PREPLACED_DECAL_TYPES_H

#include <stddef.h>
#include "math/real_math_types.h"

struct s_preplaced_decal
{
    short palette_index;
    real_orientation orientation;
};

typedef char preplaced_decal_orientation_offset[(offsetof(s_preplaced_decal, orientation) == 4) ? 1 : -1];
typedef char preplaced_decal_size[(sizeof(s_preplaced_decal) == 0x24) ? 1 : -1];

#endif
