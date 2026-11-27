#ifndef PHYSICS_BREAKABLE_SURFACE_KEY_H
#define PHYSICS_BREAKABLE_SURFACE_KEY_H
#include "math/real_math_types.h"
#include <stddef.h>

// Structure-physics key table; this header names
// the trailing six floats as axis bounds, rather than unused bytes.
struct s_breakable_surfaces_mopp_record
{
    short variant_index;
    unsigned char set_index;
    unsigned char surface_index;
    long record_index;
    real_rectangle3d bounds;
};
typedef char breakable_surface_key_size[(sizeof(s_breakable_surfaces_mopp_record) == 0x20) ? 1 : -1];
typedef char breakable_surface_key_bounds_offset[(offsetof(s_breakable_surfaces_mopp_record, bounds) == 8) ? 1 : -1];
#endif
