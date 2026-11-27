#ifndef PHYSICS_COLLISION_BSP_H
#define PHYSICS_COLLISION_BSP_H

#include <stddef.h>
#include "cseries/cseries_wrapped.h"

struct collision_leaf
{
    unsigned char flags;
    unsigned short reference_count;
    unsigned long first_reference_index;
};

struct collision_reference
{
    short plane_index;
    short node_index;
};

#if !defined(_WIN64) && !defined(__LP64__)
typedef char collision_leaf_size[(sizeof(collision_leaf) == 8) ? 1 : -1];
typedef char collision_leaf_count_offset[(offsetof(collision_leaf, reference_count) == 2) ? 1 : -1];
typedef char collision_leaf_first_offset[(offsetof(collision_leaf, first_reference_index) == 4) ? 1 : -1];
#endif
typedef char collision_reference_size[(sizeof(collision_reference) == 4) ? 1 : -1];

// March's collision_materials_extant_flags_test asserts a 512-bit range.
struct s_collision_materials_extant_flags
{
    unsigned long flags[16];
};

typedef char collision_materials_extant_flags_size[(sizeof(s_collision_materials_extant_flags) == 64) ? 1 : -1];

enum e_collision_bsp_test_sphere_flag
{
    _collision_bsp_test_sphere_stop_at_first_hit_bit = 0,
    _collision_bsp_test_sphere_leaves_bit,
    _collision_bsp_test_sphere_surfaces_bit,
    k_collision_bsp_test_sphere_flag_count
};

typedef c_flags<e_collision_bsp_test_sphere_flag, unsigned long, k_collision_bsp_test_sphere_flag_count> c_collision_bsp_test_sphere_flags;

// Functions defined in tu/blamlibxbox/collision_bsp.cpp.

struct collision_bsp;
struct collision_bsp_test_vector_result;
struct collision_bsp_test_sphere_result;
union real_point3d;
union real_vector3d;
struct s_breakable_surface_set;

bool collision_bsp_test_vector(unsigned long flags, const collision_bsp *bsp, const s_collision_materials_extant_flags *materials,
    short breakable_surface_set_index, const s_breakable_surface_set *breakable_surface_set, const real_point3d *point,
    const real_vector3d *vector, float max_t, collision_bsp_test_vector_result *result);
bool collision_bsp_test_sphere(const collision_bsp *bsp, c_collision_bsp_test_sphere_flags flags, long breakable_surface_set_count,
    const s_breakable_surface_set *breakable_surface_sets, const real_point3d *point, float radius,
    collision_bsp_test_sphere_result *result);

union real_point2d;
struct test_sphere_data;
struct test_vector_data;

void bsp3d_test_sphere_recursive(test_sphere_data *data, long node_index);
void bsp2d_test_sphere_recursive(test_sphere_data *data, long bsp2d_node_index, long leaf_index);
void collision_surface_test_sphere(test_sphere_data *data, long surface_index, long plane_index);
bool collision_bsp_test_vector_recursive(test_vector_data *data, long node_index, float t0, float t1);
bool collision_surface_test_point(const collision_bsp *bsp, short breakable_surface_set_count,
    const s_breakable_surface_set *breakable_surface_sets, long surface_index, short projection, bool sign,
    const real_point2d *point);
long collision_leaf_test_vector(const collision_bsp *bsp, const s_collision_materials_extant_flags *materials,
    short breakable_surface_set_count, const s_breakable_surface_set *breakable_surface_sets, const real_point3d *point,
    const real_vector3d *vector, long leaf_index, long plane_index, float t, bool test_surface);

#endif // PHYSICS_COLLISION_BSP_H
