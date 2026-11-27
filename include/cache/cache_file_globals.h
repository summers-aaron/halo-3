#ifndef CACHE_CACHE_FILE_GLOBALS_H
#define CACHE_CACHE_FILE_GLOBALS_H

#include "cache/cache_file_header.h"
#include "cache/cache_files.h"

// The loaded cache file (cache\cache_files.cpp): g_cache_file_globals, and the accessors the March 2007 build
// defines there.
struct s_cache_file_globals
{
    bool tags_loaded;
    void *tag_cache_base_address;
    void *tag_cache_interop_buffer; // March: the "tag cache interop" allocation; no unit in this tree reads it
    s_cache_file_header header;
    s_cache_file_tags_header *tags_header;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char cache_file_globals_offset_tag_cache_base_address[(sizeof(void *) != 4
    || (offsetof(s_cache_file_globals, tag_cache_base_address) == 0x4)) ? 1 : -1];
typedef char cache_file_globals_offset_tag_cache_interop_buffer[(sizeof(void *) != 4
    || (offsetof(s_cache_file_globals, tag_cache_interop_buffer) == 0x8)) ? 1 : -1];
typedef char cache_file_globals_offset_header[(sizeof(void *) != 4 || (offsetof(s_cache_file_globals, header) == 0x10)) ? 1 : -1];
typedef char cache_file_globals_offset_tags_header[(sizeof(void *) != 4 || (offsetof(s_cache_file_globals, tags_header) == 0x3010)) ? 1 : -1];
typedef char cache_file_globals_size[(sizeof(void *) != 4 || (sizeof(s_cache_file_globals) == 0x3018)) ? 1 : -1];
#endif

static __forceinline const s_cache_file_header *cache_files_get_header()
{
    return &g_cache_file_globals.header;
}

static __forceinline long cache_files_get_total_tags_size()
{
    return g_cache_file_globals.tags_loaded ? cache_files_get_header()->total_tags_size : 0;
}

static __forceinline const void *cache_file_globals_get_tag_cache_base_address()
{
    return g_cache_file_globals.tags_loaded ? g_cache_file_globals.tag_cache_base_address : 0;
}

static __forceinline const s_cache_file_tags_header *cache_file_globals_get_tags_header()
{
    return g_cache_file_globals.tags_header;
}

static __forceinline unsigned long tag_get_group_tag(long tag_index)
{
    return g_cache_file_globals.tags_header->groups[global_tag_instances[(short)tag_index].group_index].group_tags[0];
}

static __forceinline long cache_file_get_global_tag_index(unsigned long group_tag)
{
    long tag_index = -1;
    for (long i = 0; i < g_cache_file_globals.tags_header->global_tag_count; i++)
    {
        if (g_cache_file_globals.tags_header->global_tags[i].group == group_tag)
            tag_index = g_cache_file_globals.tags_header->global_tags[i].index;
    }
    return tag_index;
}

#endif // CACHE_CACHE_FILE_GLOBALS_H
