#ifndef INCLUDE_PHYS_COLLIDE_CAPSULE_TRIANGLE_UTIL_H
#define INCLUDE_PHYS_COLLIDE_CAPSULE_TRIANGLE_UTIL_H

#include "phys/phys_collide_triangle_util.h"

class phys_contact_point;
class phys_vec4;

extern "C" void phys_capsule_util_caps_vs_tri(const phys_vec4 *caps_a_points, float caps_a_radius, const phys_vec4 *tri_vertices,
    float triangle_b_radius, const phys_collide_triangle_util::point_tri_distance_cache &cache, float capsule_tri_tolerance, int search_manifold,
    phys_contact_point *points);

#endif
