#ifndef MEMORY_LRUV_CACHE_TYPES_H
#define MEMORY_LRUV_CACHE_TYPES_H

#include <stddef.h>
#include "memory/lruv_block_header_types.h"

class c_allocation_base;

struct s_lruv_procs
{
    void *client_data;
    void (*delete_proc)(void *, long, bool);
    bool (*locked_proc)(void *, long);
    unsigned char (*usage_proc)(void *, long);
};

// The paging cache installs and invokes the move callback at +0x30.
struct s_lruv_cache
{
    char name[32];
    s_lruv_procs procs;
    void (*move_proc)(void *, long, unsigned long, unsigned long);
    unsigned long hole_algorithm;
    unsigned long maximum_page_count;
    long page_size_bits;
    long pages_per_block_ratio;          // (maximum_page_count / block capacity) << 3; -1 without blocks
    long frame;
    long first_block_index;
    long last_block_index;
    long smallest_failed_page_counts[8]; // per minimum age, the smallest request that found no hole since the last idle
    s_lruv_block_header header;
    c_allocation_base *allocator;
    long critical_section;
    long : 32;                           // the March 2007 cache ends at critical_section; no code reads these bytes
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char lruv_cache_move_proc_offset[(sizeof(void *) != 4 || (offsetof(s_lruv_cache, move_proc) == 0x30)) ? 1 : -1];
typedef char lruv_cache_size[(sizeof(void *) != 4 || (sizeof(s_lruv_cache) == 0x84)) ? 1 : -1];
typedef char lruv_cache_page_size_offset[(sizeof(void *) != 4 || (offsetof(s_lruv_cache, page_size_bits) == 0x3c)) ? 1 : -1];
typedef char lruv_cache_block_header_offset[(sizeof(void *) != 4 || (offsetof(s_lruv_cache, header) == 0x70)) ? 1 : -1];
#endif

#endif // MEMORY_LRUV_CACHE_TYPES_H
