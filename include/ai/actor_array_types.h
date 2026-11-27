#ifndef AI_ACTOR_ARRAY_TYPES_H
#define AI_ACTOR_ARRAY_TYPES_H

#include "memory/data_types.h"

struct actor_datum;

struct actor_array {
    char name[32];
    long maximum_count;
    union {
        long size;
        long element_size;
    };
    unsigned char alignment_bits;
    unsigned char valid;
    unsigned short flags;
    unsigned long signature;
    c_allocation_base *allocator;
    long next_index;
    long first_unallocated;
    long actual_count;
    short next_identifier;
    short isolated_next_identifier;
    actor_datum *data;
};

typedef char actor_array_size[(sizeof(void *) != 4 || ((sizeof(actor_array) == 0x48))) ? 1 : -1];

#endif // AI_ACTOR_ARRAY_TYPES_H
