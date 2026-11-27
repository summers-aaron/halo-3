#ifndef PHYSICS_BREAKABLE_SURFACE_TYPES_H
#define PHYSICS_BREAKABLE_SURFACE_TYPES_H

#include <stddef.h>
#include "memory/static_arrays.h"

// The runtime set returned by breakable_surface_get_first_set.
struct s_breakable_surface_set
{
    long next_index;
    bool active;
    c_static_flags<256> surface_flags;
    c_static_array<signed char, 16> material_indices;

    __forceinline s_breakable_surface_set()
    {
        surface_flags.set_all();
        material_indices.set_all(-1);
        next_index = -1;
        active = false;
    }
};

typedef char breakable_surface_set_size[(sizeof(s_breakable_surface_set) == 0x38) ? 1 : -1];
typedef char breakable_surface_set_active_offset[(offsetof(s_breakable_surface_set, active) == 4) ? 1 : -1];
typedef char breakable_surface_set_flags_offset[(offsetof(s_breakable_surface_set, surface_flags) == 8) ? 1 : -1];
typedef char breakable_surface_set_materials_offset[(offsetof(s_breakable_surface_set, material_indices) == 0x28) ? 1 : -1];

#endif // PHYSICS_BREAKABLE_SURFACE_TYPES_H
