#ifndef TEXT_FONT_CACHE_GLOBALS_TYPES_H
#define TEXT_FONT_CACHE_GLOBALS_TYPES_H

#include "memory/data_types.h"
#include "memory/hashtable.h"
#include "text/character_data_index_types.h"

struct s_lruv_cache;
struct s_character_key;

struct s_font_cache_globals
{
    ~s_font_cache_globals();
    long flags;
    s_data_array *characters;
    c_hash_table<e_character_data_index, s_character_key> hash_table;
    s_lruv_cache *pixel_lru;
    char *pixels;
};

extern "C" {
extern __declspec(thread) s_font_cache_globals *g_font_cache_globals;
}

#endif // TEXT_FONT_CACHE_GLOBALS_TYPES_H
