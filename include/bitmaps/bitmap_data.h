#ifndef BITMAPS_BITMAP_DATA_H
#define BITMAPS_BITMAP_DATA_H

#include "math/real_math_types.h"
#include "tag_files/tag_types.h"

// 48-byte bitmap descriptor shared by the tag and rasterizer.
struct bitmap_data
{
    unsigned long signature;
    short width, height;
    signed char depth;
    unsigned char runtime_flags;
    short type, format;
    unsigned short flags;
    point2d registration_point;
    signed char mipmap_count;
    unsigned char curve;
    signed char interleaved_resource_index;
    signed char interleaved_texture_index;
    int pixel_data_offset;
    int pixel_data_size;
    int high_res_pixels_offset;
    int high_res_pixels_size;
    long texture_ref;
    TAG_POINTER(void) base_address;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char bitmap_data_size[(sizeof(void *) != 4 || sizeof(bitmap_data) == 48) ? 1 : -1];
#endif

#endif // BITMAPS_BITMAP_DATA_H
