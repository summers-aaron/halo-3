#ifndef RASTERIZER_RASTERIZER_TEXT_GLYPH_CACHE_TYPES_H
#define RASTERIZER_RASTERIZER_TEXT_GLYPH_CACHE_TYPES_H

struct bitmap_data;
struct s_font_character;

// One hardware glyph slot; slots sharing a hash bucket form a doubly linked chain.
struct s_hardware_character
{
    unsigned long key;
    long character_data_index;
    // The cached character pointer, smuggled as a long through
    // rasterizer_text_try_and_get_glyph's out_value (precache_character
    // passes (long *)out_character for that slot).
    union
    {
        unsigned long font_character;
        const s_font_character *font_character_pointer;
    };
    short previous;
    short next;
    short x;
    short y;
};

struct s_hardware_character_cache_globals
{
    bool initialized;
    long head_index;
    long tail_index;
    long cursor_x;
    long cursor_y;
    long row_height;
    bitmap_data *bitmap;
    long handle;
    float texture_scale_x;
    float texture_scale_y;
    char map[1024];
    s_hardware_character entries[512];
};

extern s_hardware_character_cache_globals g_rasterizer_text_glyph_cache;

#endif // RASTERIZER_RASTERIZER_TEXT_GLYPH_CACHE_TYPES_H
