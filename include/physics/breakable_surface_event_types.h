#ifndef PHYSICS_BREAKABLE_SURFACE_EVENT_TYPES_H
#define PHYSICS_BREAKABLE_SURFACE_EVENT_TYPES_H

#include <stddef.h>
#include "math/real_math_types.h"

// The 4 material bytes always view an s_location (damage_data.location at the
// 4 cast sites, and the 32-bit wire "location" in simulation_game_damage.cpp).
struct s_breakable_surfaces_event_material { unsigned char type; unsigned char cluster_index; unsigned short leaf_index; };
struct s_breakable_surfaces_list {
    long count;
    short indices[512];
};
struct s_breakable_surfaces_broken_event {
    short identifier;
    short pad;
    unsigned long surface_bits[8];
    s_breakable_surfaces_list surface_list;
    union { s_breakable_surfaces_event_material material; long material_index; };
    real_point3d damage_position;
    real_point3d position;
    real_vector3d direction;
    long previous_event;
    long cause;
    long time;
    long set_index;
    short variant_index;
    char bsp_index;
    char effects_created;
};

typedef char breakable_event_size[(sizeof(s_breakable_surfaces_broken_event) == 0x464) ? 1 : -1];
typedef char breakable_event_material_offset[(offsetof(s_breakable_surfaces_broken_event, material) == 0x428) ? 1 : -1];
typedef char breakable_event_variant_offset[(offsetof(s_breakable_surfaces_broken_event, variant_index) == 0x460) ? 1 : -1];

#endif
