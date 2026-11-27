#ifndef TEXT_FONT_CACHE_CHARACTER_TYPES_H
#define TEXT_FONT_CACHE_CHARACTER_TYPES_H

#include <stddef.h>

struct s_font_cache_character_metrics {
    short advance;
    unsigned short pixel_count;
    short width;
    short height;
    short origin_x;
    short origin_y;
};

struct s_font_cache_range {
    unsigned int begin;
    unsigned int end;
};

struct s_font_cache_page_header
{
    unsigned short entries_offset;
    unsigned short entry_count;
    unsigned short data_offset;
    unsigned short data_size;
};
struct s_font_cache_page_entry
{
    union
    {
        unsigned long character;
#if defined(_XBOX) && !defined(__clang__)
        struct
        {
            unsigned short font_index;
            unsigned short character_index;
        };
#else
        struct
        {
            unsigned short character_index;
            unsigned short font_index;
        };
#endif
    };
    unsigned short offset;
    unsigned short reserved;
};

struct font_cache_character_data
{
    long key;
    long frame_index;
    long hardware_block_index;
    s_font_cache_character_metrics metrics;
};


typedef char font_cache_metrics_size[(sizeof(s_font_cache_character_metrics) == 12) ? 1 : -1];
typedef char font_cache_character_data_size[(sizeof(font_cache_character_data) == 24) ? 1 : -1];
typedef char font_cache_character_hardware_offset[(offsetof(font_cache_character_data, hardware_block_index) == 8) ? 1 : -1];
typedef char font_cache_character_metrics_offset[(offsetof(font_cache_character_data, metrics) == 12) ? 1 : -1];
typedef char font_cache_page_header_size[(sizeof(s_font_cache_page_header) == 8) ? 1 : -1];
typedef char font_cache_page_entry_size[(sizeof(s_font_cache_page_entry) == 8) ? 1 : -1];

#endif
