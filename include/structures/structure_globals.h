#ifndef STRUCTURES_STRUCTURE_GLOBALS_H
#define STRUCTURES_STRUCTURE_GLOBALS_H

#include <stddef.h>
#include "rasterizer/simple_hash_table.h"

// Shared marker storage used by structures and visibility_collection_structure.
struct s_structure_globals
{
    bool initialized;
    bool cluster_marker_initialized;
    long cluster_marker_sequence;
    long cluster_markers[16][256];
    bool instanced_geometry_marker_initialized;
    long instanced_geometry_marker_sequence;
    c_additive_hash<512, 4096, 3307, 3> marker;
};

typedef char structure_globals_size[(sizeof(s_structure_globals) == 0xc414) ? 1 : -1];
typedef char structure_globals_marker_offset[(offsetof(s_structure_globals, marker) == 0x4010) ? 1 : -1];

extern s_structure_globals thread_structure_globals[2];

#endif // STRUCTURES_STRUCTURE_GLOBALS_H
