#ifndef INCLUDE_PHYS_COLLIDE_CAPSULE_UTIL_H
#define INCLUDE_PHYS_COLLIDE_CAPSULE_UTIL_H

#include "phys/phys_scalar_types.h"

// Functions defined in tu/hkinternal/hkCollideCapsuleUtil.cpp that other units call.

class phys_contact_point;
class phys_vec4;

extern "C" phys_result phys_capsule_util_closest_caps_vs_caps(const phys_vec4 *caps_a, float caps_a_radius,
    const phys_vec4 *caps_b, float caps_b_radius, float capsule_tri_tolerance, phys_contact_point &capsule_point_out);

extern "C" void phys_capsule_util_manifold_caps_vs_caps(const phys_vec4 *caps_a, float caps_a_radius,
    const phys_vec4 *caps_b_points, float caps_b_radius, phys_contact_point *capsule_points_out);

#endif // HAVOK_HK_COLLIDE_CAPSULE_UTIL_H
