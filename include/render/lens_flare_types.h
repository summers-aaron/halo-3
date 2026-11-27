#ifndef RENDER_LENS_FLARE_TYPES_H
#define RENDER_LENS_FLARE_TYPES_H
#include "math/real_math_types.h"
// The value stored before the three keys in a lens-flare hash entry.
struct s_lens_flare
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
};
typedef char lens_flare_value_size[(sizeof(s_lens_flare) == 68) ? 1 : -1];
#endif
