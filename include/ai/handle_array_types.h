#ifndef AI_HANDLE_ARRAY_TYPES_H
#define AI_HANDLE_ARRAY_TYPES_H

#include "memory/data_types.h"

struct s_handle;

// Typed s_data_array over s_handle elements (mirrors actor_array): the
// data pointer sits at +0x44 exactly as in s_data_array.
struct s_handle_array {
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
    s_handle *data;
};

typedef char s_handle_array_size[(sizeof(void *) != 4 || ((sizeof(s_handle_array) == 0x48))) ? 1 : -1];

#endif // AI_HANDLE_ARRAY_TYPES_H
