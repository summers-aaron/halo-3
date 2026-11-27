#ifndef GPU_PARTICLE_LOCATION_ARRAY_TYPES_H
#define GPU_PARTICLE_LOCATION_ARRAY_TYPES_H

#include "memory/data_types.h"

struct s_location;

struct s_location_array {
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
    s_location *data;
};

typedef char location_array_size[(sizeof(s_location_array) == 0x48) ? 1 : -1];

#endif // GPU_PARTICLE_LOCATION_ARRAY_TYPES_H
