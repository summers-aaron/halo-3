#ifndef PHYSICS_COLLISION_TYPES_H
#define PHYSICS_COLLISION_TYPES_H

#include <stddef.h>
#include "physics/collision_test_flags.h"
#include "math/real_math_types.h"
#include "objects/object_reference_types.h"
#include "game/materials.h"

struct collision_bsp;


// Physics collision query result. March 82447ff8 constructs only global_material;
// the BSP pointer identifies which BSP the hit was found in.
// Kept in this leaf while private copies in collisions.h's held users are retired.
struct collision_result {
    long type;
    union { float fraction; float t; };
    real_point3d point;
    s_location start_location;
    s_location location;
    const collision_bsp *bsp;
    c_global_material_type global_material;
    real_plane3d plane;
    long geometry_index;
    long instance_index;
    long object_index;
    short region_index;
    short node_index;
    unsigned long bsp_reference;
    long structure_bsp_index;
    long leaf_index;
    long surface_index;
    unsigned long plane_index;
    unsigned char flags;
    unsigned char breakable_surface_bit;
    unsigned short material_index;
    unsigned char breakable_surface;

    // Copies another collision result.
    collision_result &operator=(const collision_result &other);
};

#if !defined(_WIN64) && !defined(__LP64__)
typedef char collision_result_size[(sizeof(collision_result) == 0x60) ? 1 : -1];
typedef char collision_result_offset_00[(offsetof(collision_result, type) == 0x00) ? 1 : -1];
typedef char collision_result_offset_04[(offsetof(collision_result, fraction) == 0x04) ? 1 : -1];
typedef char collision_result_offset_08[(offsetof(collision_result, point) == 0x08) ? 1 : -1];
typedef char collision_result_offset_14[(offsetof(collision_result, start_location) == 0x14) ? 1 : -1];
typedef char collision_result_offset_18[(offsetof(collision_result, location) == 0x18) ? 1 : -1];
typedef char collision_result_offset_1c[(offsetof(collision_result, bsp) == 0x1c) ? 1 : -1];
typedef char collision_result_offset_20[(offsetof(collision_result, global_material) == 0x20) ? 1 : -1];
typedef char collision_result_offset_24[(offsetof(collision_result, plane) == 0x24) ? 1 : -1];
typedef char collision_result_offset_34[(offsetof(collision_result, geometry_index) == 0x34) ? 1 : -1];
typedef char collision_result_offset_38[(offsetof(collision_result, instance_index) == 0x38) ? 1 : -1];
typedef char collision_result_offset_3c[(offsetof(collision_result, object_index) == 0x3c) ? 1 : -1];
typedef char collision_result_offset_40[(offsetof(collision_result, region_index) == 0x40) ? 1 : -1];
typedef char collision_result_offset_42[(offsetof(collision_result, node_index) == 0x42) ? 1 : -1];
typedef char collision_result_offset_44[(offsetof(collision_result, bsp_reference) == 0x44) ? 1 : -1];
typedef char collision_result_offset_48[(offsetof(collision_result, structure_bsp_index) == 0x48) ? 1 : -1];
typedef char collision_result_offset_4c[(offsetof(collision_result, leaf_index) == 0x4c) ? 1 : -1];
typedef char collision_result_offset_50[(offsetof(collision_result, surface_index) == 0x50) ? 1 : -1];
typedef char collision_result_offset_54[(offsetof(collision_result, plane_index) == 0x54) ? 1 : -1];
typedef char collision_result_offset_58[(offsetof(collision_result, flags) == 0x58) ? 1 : -1];
typedef char collision_result_offset_59[(offsetof(collision_result, breakable_surface_bit) == 0x59) ? 1 : -1];
typedef char collision_result_offset_5a[(offsetof(collision_result, material_index) == 0x5a) ? 1 : -1];
typedef char collision_result_offset_5c[(offsetof(collision_result, breakable_surface) == 0x5c) ? 1 : -1];
#endif

#endif // PHYSICS_COLLISION_TYPES_H
