#ifndef OBJECTS_OBJECT_REFERENCE_TYPES_H
#define OBJECTS_OBJECT_REFERENCE_TYPES_H

#include "objects/cluster_reference.h"

struct s_location {
    union {
        s_cluster_reference cluster_reference;
        short cluster_index;
        // Packed (bsp, cluster) pair as the structure-audibility ABI takes it.
        unsigned short cluster_packed;
    };
    unsigned short leaf_index;  // Leaf index within the cluster; 0xFFFF means none.
};

static __forceinline s_location *location_invalidate(s_location *location)
{
    *(long *)location = -1;
    return location;
}

// March name: object_header_block_reference (object_header_block_get takes one).
struct s_object_block_reference {
    short size;
    short offset;
};

typedef s_object_block_reference object_header_block_reference;

typedef char obj_ref_size_location[(sizeof(s_location) == 4) ? 1 : -1];
typedef char obj_ref_size_block_ref[(sizeof(s_object_block_reference) == 4) ? 1 : -1];

#endif // OBJECTS_OBJECT_REFERENCE_TYPES_H
