#ifndef MEMORY_HASHTABLE_TYPES_H
#define MEMORY_HASHTABLE_TYPES_H

#include "memory/static_arrays.h"
#include <stddef.h>

class c_allocation_base;

struct s_hash_table_bucket
{
    void *key;
    unsigned long hash;
    s_hash_table_bucket *next;
};

// March hash_table_new (82c780f0) sets the name at +0 and allocator at +0x34.
struct s_hash_table
{
    c_static_string<32> name;
    unsigned long count;            /* buckets */
    int entry_count;
    int entry_size;
    unsigned long (*hash)(void *);
    unsigned char (*equal)(void *, void *);
    c_allocation_base *allocation;
    char *entries;
    s_hash_table_bucket *last;      /* free list */
    s_hash_table_bucket *buckets[1];
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char hash_table_bucket_size[(sizeof(void *) != 4 || (sizeof(s_hash_table_bucket) == 12)) ? 1 : -1];
typedef char hash_table_size[(sizeof(void *) != 4 || (sizeof(s_hash_table) == 0x44)) ? 1 : -1];
typedef char hash_table_count_offset[(offsetof(s_hash_table, count) == 0x20) ? 1 : -1];
typedef char hash_table_allocation_offset[(sizeof(void *) != 4 || (offsetof(s_hash_table, allocation) == 0x34)) ? 1 : -1];
typedef char hash_table_entries_offset[(sizeof(void *) != 4 || (offsetof(s_hash_table, entries) == 0x38)) ? 1 : -1];
typedef char hash_table_buckets_offset[(sizeof(void *) != 4 || (offsetof(s_hash_table, buckets) == 0x40)) ? 1 : -1];
#endif

#endif // MEMORY_HASHTABLE_TYPES_H
