#ifndef TEXT_FONT_PACKAGE_TYPES_H
#define TEXT_FONT_PACKAGE_TYPES_H

#include <stddef.h>
#include "text/font_cache_file_types.h"

struct s_kerning_pair
{
    unsigned char second_character;
    signed char offset;
};

struct s_font_package_entry
{
    long header_offset;
    long header_size;
    short first_character;
    short character_count;
};

struct s_font_package_header
{
    unsigned long version;
    long font_count;
    s_font_package_entry fonts[16];
    long font_indices[16];
    unsigned long data_offset;
    long data_size;
    long package_size;
    long character_count;
};

typedef s_font_package_header s_font_package;

struct s_font_globals {
    signed char initialized;
    bool use_cached_fonts;
    signed char disabled;
    bool has_failed;
    long language;
    long failure_count;
    c_synchronized_long async_error;
    s_font_cache_file file;
    s_font_package_header *package;
    union
    {
        char package_buffer[0x8000];
        s_font_package_header package_header;
    };
};

struct s_font_header
{
    unsigned long version;
    char name[32];
    short ascending_height;
    short descending_height;
    short leading_height;
    union
    {
        short maximum_kerning;
        short leading_width;
    };
    unsigned long kerning_pairs_offset;
    long kerning_pair_count;
    unsigned char character_first_kerning_pair_index[256];
    unsigned long character_data_offset;
    long character_count;
    long character_data_count;
    long character_data_size;
    long packed_data_size;
    unsigned long fallback_character;
    long maximum_character_width;
    long maximum_character_height;
    long maximum_character_data_size;
    long reserved;
};

typedef char font_header_size[(sizeof(s_font_header) == 0x15c) ? 1 : -1];
typedef char font_globals_package_offset[(sizeof(void *) != 4 || offsetof(s_font_globals, package) == 0x158) ? 1 : -1];

#endif
