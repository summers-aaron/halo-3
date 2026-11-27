#ifndef PHYSICS_HAVOK_RUNTIME_POOL_TYPES_H
#define PHYSICS_HAVOK_RUNTIME_POOL_TYPES_H

#include <stddef.h>
#include "physics/collision_surface_types.h"
#include "physics/collision_bsp_types.h"
#include "physics/havok_runtime_pool_cache_entry_types.h"
#include "physics/havok_collision_surface_shape.h"
#include "physics/havok_collision_geometry_shape.h"
#include "phys/phys_collide.h"

#include "physics/collision_bsp_cache_types.h"

struct s_havok_runtime_pool_cache
{
    short buckets[4096];
    s_havok_runtime_pool_cache_entry entries[512];
    __declspec(align(16)) char shapes[512][sizeof(c_collision_surface_shape)]; // each slot holds one cached surface shape
    long count;
    long next_index;
    bool initialized;
};

struct havok_runtime_pool_instance_collision
{
    // The runtime view of s_collision_bsp_physics_definition: the fixup placement-news a
    // c_collision_geometry_shape into m_geometry_shape and the mopp shape into m_mopp_shape.
    c_collision_geometry_shape geometry_shape;
    phys_bvh_tree_shape shape;
};

typedef char runtime_cached_bsp_size[(sizeof(structure_instanced_geometry_definition) == 184) ? 1 : -1];
typedef char runtime_cached_bsp_surface_block[
    (offsetof(structure_instanced_geometry_definition, bsp) + offsetof(collision_bsp, surfaces) == 0x50) ? 1 : -1];
typedef char runtime_cached_bsp_surfaces[
    (offsetof(structure_instanced_geometry_definition, bsp) + offsetof(collision_bsp, surfaces) +
        offsetof(s_tag_block, address) == 0x54) ? 1 : -1];
typedef char runtime_cache_user_size[(sizeof(void *) != 4 || ((sizeof(s_user_block) == 0x100))) ? 1 : -1];
typedef char runtime_cache_users_offset[(sizeof(void *) != 4 || ((offsetof(s_collision_bsp_cache, users) == 0x44))) ? 1 : -1];
typedef char runtime_instance_geometry_offset[
    (sizeof(void *) != 4 || ((offsetof(havok_runtime_pool_instance_collision, geometry_shape) +
        offsetof(c_collision_geometry_shape, m_geometry_index) == 0x40))) ? 1 : -1];
typedef char runtime_instance_scale_offset[
    (sizeof(void *) != 4 || ((offsetof(havok_runtime_pool_instance_collision, geometry_shape) +
        offsetof(c_collision_geometry_shape, m_radius) == 0x4c))) ? 1 : -1];
typedef char runtime_instance_shape_offset[(sizeof(void *) != 4 || ((offsetof(havok_runtime_pool_instance_collision, shape) == 0x50))) ? 1 : -1];
typedef char runtime_pool_cache_shapes_offset[(offsetof(s_havok_runtime_pool_cache, shapes) == 0x3800) ? 1 : -1];

#endif
