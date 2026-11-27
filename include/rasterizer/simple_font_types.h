#ifndef RASTERIZER_SIMPLE_FONT_TYPES_H
#define RASTERIZER_SIMPLE_FONT_TYPES_H

#include <stddef.h>

struct bitmap_data;
// March simple_font::s_font_data::install and render_text identify the common
// font descriptor. Its texture reference is a 32-bit handle.
namespace simple_font
{
struct s_font_data
{
    bool active;
    long width;
    long height;
    long characters_per_row;
    long first_character;
    long last_character;
    float scale;
    long texture_width;
    long texture_height;
    bitmap_data *resource;
    long handle;
    long pitch;
    long bitmap_width;
    long bitmap_height;
    const char *pixels;
};

// March simple_font::begin_rendering stores its drop-shadow argument in the
// first byte of g_simple_font_globals; render_text reads it back.
struct s_simple_font_globals
{
    bool shadow_enabled;
};
}

typedef char simple_font_data_size[(sizeof(void *) != 4 || sizeof(simple_font::s_font_data) == 0x3c) ? 1 : -1];
typedef char simple_font_resource_offset[(sizeof(void *) != 4 || offsetof(simple_font::s_font_data, resource) == 0x24) ? 1 : -1];
typedef char simple_font_texture_offset[(sizeof(void *) != 4 || offsetof(simple_font::s_font_data, handle) == 0x28) ? 1 : -1];
typedef char simple_font_pixels_offset[(sizeof(void *) != 4 || offsetof(simple_font::s_font_data, pixels) == 0x38) ? 1 : -1];

#endif
