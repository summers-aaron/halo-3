#ifndef PHYSICS_HAVOK_COLLISION_FILTER_H
#define PHYSICS_HAVOK_COLLISION_FILTER_H

#include "physics/havok_group_types.h"
enum e_soft_surface_sink_class;

e_soft_surface_sink_class havok_group_soft_surface_sink_class_get(e_havok_group group);

// Functions defined in tu/blamlibxbox/havok_collision_filter.cpp.

class phys_group_filter;

phys_group_filter *havok_initialize_collision_filter();

class phys_collision_proxy;
class phys_collider;
class phys_shape;
class phys_shape_container;
class c_group_filter;

extern "C" bool havok_collision_filter_test_phantom_collision(const phys_collider *a, const phys_collider *b, const phys_collision_proxy *body_a,
    const phys_collision_proxy *body_b, const phys_shape_container *container, unsigned long key);
extern "C" unsigned char havok_collision_filter_try_and_get_primitive_collision(const phys_group_filter *filter,
    const phys_collision_proxy *a, const phys_collision_proxy *b, const phys_shape *shape_b, bool *collision);
extern "C" {
inline bool group_filter_is_phantom_shape_a_versus_phantom_b_collision(const phys_collider *, const phys_collider *, bool *);
bool group_filter_is_phantom_shape_versus_phantom_collision(const c_group_filter *, const phys_collider *, const phys_collider *, bool *);
bool group_filter_is_phantom_shape_versus_environment_collision(const phys_collision_proxy *, const phys_collision_proxy *,
    const phys_shape_container *, unsigned long);
}

#endif // PHYSICS_HAVOK_COLLISION_FILTER_H
