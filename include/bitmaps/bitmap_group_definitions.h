#ifndef BITMAPS_BITMAP_GROUP_DEFINITIONS_H
#define BITMAPS_BITMAP_GROUP_DEFINITIONS_H

#include <stddef.h>
#include "cseries/cseries_types.h"
#include "tag_files/tag_types.h"
#include "math/real_math_types.h"

struct bitmap_sprite
{
    short bitmap_index;
    short bitmap_pad;
    long unused;
    real_rectangle2d bounds;
    real_point2d registration_point;
};

struct bitmap_sequence
{
    char name[32];
    short first_bitmap_index;
    short bitmap_count;
    long unused[4];
    s_tag_block sprites;
};

typedef char bitmap_sprite_size[(sizeof(bitmap_sprite) == 32) ? 1 : -1];
typedef char bitmap_sequence_size[(sizeof(void *) != 4 || sizeof(bitmap_sequence) == 64) ? 1 : -1];
typedef char bitmap_sprite_bounds_offset[(offsetof(bitmap_sprite, bounds) == 8) ? 1 : -1];

// Bitmap tag containing source pixels, sprite sequences, and platform texture resources.
struct bitmap_group
{
    enum
    {
        _using_interop_resource_bit = 4,
    };

    long usage;
    short flags;
    short sprite_spacing;
    real bump_map_height;
    real fade_factor;
    char curve_mode;
    char max_mipmap_level;
    short max_resolution;
    s_tag_block usage_overrides;
    s_tag_block manual_sequences;
    s_tag_data source_data;
    s_tag_data processed_pixel_data;
    s_tag_block sequences;
    s_tag_block bitmaps;
    s_tag_data xenon_processed_pixel_data;
    s_tag_block xenon_bitmaps;
    s_tag_block hardware_textures;
    s_tag_block interleaved_hardware_textures;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char bitmap_group_size[(sizeof(bitmap_group) == 0xa4) ? 1 : -1];
typedef char bitmap_group_sequences_offset[(offsetof(bitmap_group, sequences) == 0x54) ? 1 : -1];
typedef char bitmap_group_bitmaps_offset[(offsetof(bitmap_group, bitmaps) == 0x60) ? 1 : -1];
#endif

#endif // BITMAPS_BITMAP_GROUP_DEFINITIONS_H
