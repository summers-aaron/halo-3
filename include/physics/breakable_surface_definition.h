#ifndef PHYSICS_BREAKABLE_SURFACE_DEFINITION_H
#define PHYSICS_BREAKABLE_SURFACE_DEFINITION_H
#include "tag_files/tag_types.h"
#include <stddef.h>

// Breakable surface definition: vitality, effect and sound, particle effects, and crack/hole bitmaps.
struct s_breakable_surface_definition
{
    float maximum_vitality;
    s_tag_reference effect;
    s_tag_reference sound;
    s_tag_block particle_effects;
    float particle_density;
    s_tag_reference crack_bitmap;
    s_tag_reference hole_bitmap;
};
typedef char breakable_surface_definition_size[(sizeof(s_breakable_surface_definition) == 0x54) ? 1 : -1];
typedef char breakable_surface_effect_offset[(offsetof(s_breakable_surface_definition, effect) == 4) ? 1 : -1];
typedef char breakable_surface_sound_offset[(offsetof(s_breakable_surface_definition, sound) == 0x14) ? 1 : -1];
typedef char breakable_surface_crack_offset[(offsetof(s_breakable_surface_definition, crack_bitmap) == 0x34) ? 1 : -1];
typedef char breakable_surface_hole_offset[(offsetof(s_breakable_surface_definition, hole_bitmap) == 0x44) ? 1 : -1];
#endif
