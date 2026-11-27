#ifndef PHYSICS_HAVOK_SHAPES_H
#define PHYSICS_HAVOK_SHAPES_H

// Functions defined in tu/blamlibxbox/havok_shapes.cpp.

void __tls_set_g_havok_shape_globals_allocator(void *allocator);

struct collision_bsp;

// Allocation hooks for Havok shapes.
void *havok_aligned_malloc(int size, int alignment);
void havok_aligned_free(void *block);

bool surface_has_sink(collision_bsp *bsp, long surface_index);



extern "C" long get_bsp_surface_count(const collision_bsp *list);

// Data defined in tu/blamlibxbox/havok_shapes.cpp that other units use.
extern bool g_havok_shapes_environment_null_shapes_locked;

#include <vectorintrinsics.h>
extern "C" __vector4 k_negative_flt_max_vector;
#endif // PHYSICS_HAVOK_SHAPES_H
