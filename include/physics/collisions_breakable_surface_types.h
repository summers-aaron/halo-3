#ifndef PHYSICS_COLLISIONS_BREAKABLE_SURFACE_TYPES_H
#define PHYSICS_COLLISIONS_BREAKABLE_SURFACE_TYPES_H
#include "physics/breakable_surface_types.h"
#include "math/real_math_types.h"

// Full cache prefix, corroborated by breakable_surfaces.cpp's s_cache_globals.
struct collisions_breakable_surface_slot
{
    long time;
    short surface_b;
    short set_index;
    short slot_index;
    short variant_index;
    short record_index;
    real_point3d position;
};
struct collisions_breakable_surface_globals
{
    char active;
    char reset_pending;
    char dirty;
    char pad3;
    long next_set_index;
    unsigned long slot_mask;
    long next_slot;
    long oldest_slot;
    collisions_breakable_surface_slot slots[127];
    c_static_array<s_breakable_surface_set, 2048> entries;
    short default_entry_indices[16];
    short entry_indices[16][4096];
};
typedef char collisions_breakable_slot_size[(sizeof(collisions_breakable_surface_slot) == 28) ? 1 : -1];
typedef char collisions_breakable_entries_offset[(offsetof(collisions_breakable_surface_globals, entries) == 0xdf8) ? 1 : -1];
#endif
