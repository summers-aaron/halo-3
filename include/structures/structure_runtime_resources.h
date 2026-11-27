#ifndef STRUCTURES_STRUCTURE_RUNTIME_RESOURCES_H
#define STRUCTURES_STRUCTURE_RUNTIME_RESOURCES_H

#include "physics/collision_bsp_types.h"
#include "math/real_math_types.h"

struct s_structure_bsp_resources
{
    // Neither slot is read in-tree; only collision_bsp, bounds and instanced_geometry_definitions are.
    long pad0;
    TAG_POINTER(struct collision_bsp) collision_bsp;
    long pad8;
    // Existing cache readers preserve these bytes as bounds; geometry reads a tag block.
    union
    {
        long_point3d bounds;
        s_tag_block instanced_geometry_definitions;
    };
};

typedef char structure_bsp_resources_size[(sizeof(void *) != 4 || (sizeof(s_structure_bsp_resources) == 0x18)) ? 1 : -1];
typedef char structure_bsp_resources_bounds_offset[(sizeof(void *) != 4 || (offsetof(s_structure_bsp_resources, bounds) == 0xc)) ? 1 : -1];

typedef char structure_bsp_resources_definitions_offset[(sizeof(void *) != 4 || (offsetof(s_structure_bsp_resources,
    instanced_geometry_definitions) == 0xc)) ? 1 : -1];

class c_structure_bsp_resource_interface
{
    unsigned int m_datum_index;
public:
    s_structure_bsp_resources *get_resources() const;
    unsigned int get_datum_index() const { return m_datum_index; }
};

#endif
