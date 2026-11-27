#ifndef BITMAPS_BITMAP_GROUP_H
#define BITMAPS_BITMAP_GROUP_H

// Functions defined in tu/blamlibxbox/bitmap_group.cpp that other units call.

struct bitmap_data;
union real_argb_color;
union real_point2d;

extern "C" real_argb_color *bitmap_group_2d_get_real_pixel(real_argb_color *result, long group_index, long bitmap_index,
    const real_point2d *point, long mip, long layer, bool linear);

long bitmap_group_get_bitmap_index_from_sequence(long bitmap_group_index, short sequence_index, short frame_index);
bitmap_data *bitmap_group_get_bitmap_from_sequence(long bitmap_group_index, short sequence_index, short frame_index);

union real_rectangle2d;

extern "C" real_rectangle2d *bitmap_group_get_sprite_bounds_from_sequence(long bitmap_group_index, short sequence_index, short frame_index);

class c_rasterizer_texture_ref;

unsigned char bitmap_group_get_xenon_hardware_format(long tag_index, long index, c_rasterizer_texture_ref *out_element,
    const bitmap_data **out_entry, __int64 *out_datum);

#endif // BITMAPS_BITMAP_GROUP_H
