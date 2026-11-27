#ifndef STRUCTURES_STRUCTURE_BACKGROUND_SOUND_TYPES_H
#define STRUCTURES_STRUCTURE_BACKGROUND_SOUND_TYPES_H

#include <stddef.h>
#include "tag_files/tag_types.h"

struct structure_background_sound_palette_entry
{
    char name[32];
    s_tag_reference background_sound;
    s_tag_reference inside_cluster_sound;
    // Filler between the sound references and the cutoff/scale cluster; no in-tree reader uses these bytes.
    char cutoff_distance_padding[20];
    float cutoff_distance;
    unsigned long scale_flags;
    float interior_scale;
    float portal_scale;
    float exterior_scale;
    float interpolation_speed;
    // Trailing filler after the interpolation speed; no in-tree reader uses these bytes.
    char interpolation_speed_padding[8];
};
typedef char structure_background_sound_palette_entry_size[(sizeof(structure_background_sound_palette_entry) == 0x74) ? 1 : -1];
typedef char structure_background_sound_palette_entry_sound_offset[(offsetof(structure_background_sound_palette_entry, background_sound) == 0x20) ? 1 : -1];

#endif
