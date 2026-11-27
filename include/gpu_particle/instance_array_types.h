#ifndef GPU_PARTICLE_INSTANCE_ARRAY_TYPES_H
#define GPU_PARTICLE_INSTANCE_ARRAY_TYPES_H

#include "effects/particle_instance_types.h"
#include "memory/data_types.h"

struct s_instance_array {
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
    s_particle_instance *data;
};

typedef char instance_array_size[(sizeof(s_instance_array) == 0x48) ? 1 : -1];

#endif // GPU_PARTICLE_INSTANCE_ARRAY_TYPES_H
