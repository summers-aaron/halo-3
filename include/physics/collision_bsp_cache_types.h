#ifndef PHYSICS_COLLISION_BSP_CACHE_TYPES_H
#define PHYSICS_COLLISION_BSP_CACHE_TYPES_H

#include <stddef.h>
#include "tag_files/tag_types.h"
#include "structures/structure_instanced_geometry_definition.h"
#include "math/real_math_types.h"

struct collision_bsp;

struct s_shifted_instanced_geometry_block
{
    TAG_POINTER(structure_instanced_geometry_definition) data;
    TAG_POINTER(void) definition;
    union
    {
        long next_instanced_geometry_count;
        TAG_POINTER(const collision_bsp) next_thread_first_collision_bsp;
    };
};

struct s_user_block
{
    s_shifted_instanced_geometry_block entries[16];
    TAG_POINTER(const collision_bsp) next_thread_remaining_collision_bsps[15];
    long next_thread_first_instanced_geometry_count;
};

struct s_collision_bsp_cache_user
{
    TAG_POINTER(collision_bsp) collision_bsps[16];
    s_tag_block instanced_geometry_definitions[16];
};

// Per-thread lock view: the resident bsps plus their bounds, over the same
// 256 bytes as one s_collision_bsp_cache_user.
struct s_tag_resource_lock_cache_block
{
    TAG_POINTER(collision_bsp) collision_bsps[16];
    long_point3d bounds[16];
};

struct s_collision_bsp_cache
{
    union
    {
        struct
        {
            // The shifted geometry view begins at thread zero's first tag block address.
            TAG_POINTER(const collision_bsp) first_thread_collision_bsps[16];
            long first_instanced_geometry_count;
            s_user_block users[8];
        };
        s_collision_bsp_cache_user thread_users[9];
        s_tag_resource_lock_cache_block lock_blocks[9];
    };
};

typedef char collision_cache_bsp_size[(sizeof(structure_instanced_geometry_definition) == 0xb8) ? 1 : -1];
typedef char collision_cache_mesh_offset[(offsetof(structure_instanced_geometry_definition, mesh_index) == 0xb0) ? 1 : -1];
typedef char collision_cache_users_offset[(sizeof(void *) != 4 || ((offsetof(s_collision_bsp_cache, users) == 0x44))) ? 1 : -1];
typedef char collision_cache_geometry_address_offset[
    (sizeof(void *) != 4 || ((offsetof(s_collision_bsp_cache, users) ==
    offsetof(s_collision_bsp_cache_user, instanced_geometry_definitions) + offsetof(s_tag_block, address)))) ? 1 : -1];
typedef char collision_cache_geometry_stride[(sizeof(void *) != 4 || ((sizeof(s_user_block) == sizeof(s_collision_bsp_cache_user)))) ? 1 : -1];

typedef char collision_bsp_cache_size[(sizeof(void *) != 4 || ((sizeof(s_collision_bsp_cache) == 0x900))) ? 1 : -1];

typedef char collision_cache_thread_stride[(sizeof(void *) != 4 || ((sizeof(s_collision_bsp_cache_user) == 0x100))) ? 1 : -1];
typedef char collision_cache_thread_offset[(offsetof(s_collision_bsp_cache, thread_users) == 0) ? 1 : -1];
typedef char collision_cache_lock_stride[(sizeof(void *) != 4 || ((sizeof(s_tag_resource_lock_cache_block) == 0x100))) ? 1 : -1];
typedef char collision_cache_lock_offset[(offsetof(s_collision_bsp_cache, lock_blocks) == 0) ? 1 : -1];

#endif
