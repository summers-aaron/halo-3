#ifndef INCLUDE_TEXT_FONT_CACHE_INTERNAL_TYPES_H
#define INCLUDE_TEXT_FONT_CACHE_INTERNAL_TYPES_H

#include "multithreading/synchronized_value.h"
#include "memory/byte_swapping_types.h"
struct s_font_cache_channel
{
    int handle;
    long creation_frame;
    long read_complete_frame;
    int state;
    int pending;
    c_synchronized_long read_count;
    c_synchronized_long write_count;
    int size;
    char data[32768];
};
struct s_font_cache_channel_block { c_synchronized_long active; long reserved; s_font_cache_channel channels[8]; };

struct font_cache_swap_definition
{
    element_type_t type;
    unsigned long signature;
    long initialized;
};
#endif
