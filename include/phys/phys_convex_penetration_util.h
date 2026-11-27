#ifndef INCLUDE_PHYS_CONVEX_PENETRATION_UTIL_H
#define INCLUDE_PHYS_CONVEX_PENETRATION_UTIL_H

#include "phys/phys_simplex_status.h"
#include "phys/phys_scalar_types.h"

// Functions defined in tu/hkinternal/hkConvexPenetrationUtil.cpp that other units call.

class phys_hull_vertex;
class phys_convex_shape;
struct phys_simplex_out;
class phys_transform;
class phys_vec4;

phys_simplex_status phys_compute_penetration_depth(const phys_convex_shape *penetration_shape_a, const phys_convex_shape *penetration_shape_b,
    const phys_transform &transform, float tolerance, phys_hull_vertex *vertices_a, phys_hull_vertex *vertices_b,
        phys_vec4 *simplex_verts, int *simplex_size, phys_simplex_out &output);

class phys_contact_point;

phys_result phys_compute_multi_penetration_depth(const phys_transform &xform_a, const phys_convex_shape *penetration_shape_a,
    const phys_convex_shape **shape_b_list, int shape_b_count, const phys_transform &a_from_b, phys_contact_point **capsule_points_out);

#endif // HAVOK_HK_CONVEX_PENETRATION_UTIL_H
