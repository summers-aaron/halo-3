#ifndef RASTERIZER_LENS_FLARE_ENTRY_TYPES_H
#define RASTERIZER_LENS_FLARE_ENTRY_TYPES_H

#include "math/matrix_math_types.h"
#include "math/real_math_types.h"
#include "memory/data_types.h"

struct lens_flare_entry
{
    int tag_index;
    real_point3d position;
    real_vector3d direction;
    unsigned long color;
    float scale, intensity, rotation;
    int frame;
    int age;
    int element_key1;
    int element_type;
    signed char flags;
    int element_key2;
    int key0, key1, key2;
};

#endif // RASTERIZER_LENS_FLARE_ENTRY_TYPES_H
