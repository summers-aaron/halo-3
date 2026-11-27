#ifndef BITMAPS_BITMAPS_H
#define BITMAPS_BITMAPS_H

// Functions defined in tu/blamlibxbox/bitmaps.cpp that other units call.

struct bitmap_data;

float convert_xrgb_to_linear(float value);
float half_to_real_all_finite(unsigned short half);
bool bitmap_format_is_compressed(short format);
bool bitmap_format_is_default_srgb(short format);

extern "C" void bitmap_2d_initialize(bitmap_data *bitmap, short width, short height,
    short mipmap_count, short format, unsigned short flags, unsigned char external, unsigned char tiled);
extern "C" void bitmaps_dispose_resource(bitmap_data *bitmap);

#endif // BITMAPS_BITMAPS_H
