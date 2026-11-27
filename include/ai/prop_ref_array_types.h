#ifndef AI_PROP_REF_ARRAY_TYPES_H
#define AI_PROP_REF_ARRAY_TYPES_H

#include "memory/data_types.h"

struct prop_ref_datum;

// Typed s_data_array over prop_ref_datum elements (mirrors actor_array): the
// data pointer sits at +0x44 exactly as in s_data_array.
struct prop_ref_array {
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
    prop_ref_datum *data;
};

typedef char prop_ref_array_size[(sizeof(void *) != 4 || ((sizeof(prop_ref_array) == 0x48))) ? 1 : -1];

#endif // AI_PROP_REF_ARRAY_TYPES_H
