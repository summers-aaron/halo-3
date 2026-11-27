#ifndef STRUCTURES_STRUCTURE_MARKER_TYPES_H
#define STRUCTURES_STRUCTURE_MARKER_TYPES_H

#include <stddef.h>
#include "math/real_math_types.h"

struct structure_marker
{
    char name[32];
    real_quaternion rotation;
    real_point3d position;
};

typedef char structure_marker_size[(sizeof(structure_marker) == 0x3c) ? 1 : -1];
typedef char structure_marker_rotation_offset[(offsetof(structure_marker, rotation) == 0x20) ? 1 : -1];
typedef char structure_marker_position_offset[(offsetof(structure_marker, position) == 0x30) ? 1 : -1];

#endif // STRUCTURES_STRUCTURE_MARKER_TYPES_H
