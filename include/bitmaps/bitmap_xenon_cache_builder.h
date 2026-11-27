#ifndef BITMAPS_BITMAP_XENON_CACHE_BUILDER_H
#define BITMAPS_BITMAP_XENON_CACHE_BUILDER_H

#include "math/real_math_types.h"

// Functions defined in tu/blamlibxbox/bitmap_xenon_cache_builder.cpp that other units call.

struct bitmap_data;

real_argb_color bitmap_2d_xenon_get_real_pixel(const bitmap_data *bitmap,void *texture_data,const real_point2d *point,long slice,long level,bool clamp);

#endif // BITMAPS_BITMAP_XENON_CACHE_BUILDER_H
