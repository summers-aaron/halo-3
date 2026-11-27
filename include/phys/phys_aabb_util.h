#ifndef INCLUDE_PHYS_AABB_UTIL_H
#define INCLUDE_PHYS_AABB_UTIL_H

// hkAabbUtil: calcAabb over a vertex array is defined in tu/hkcollide/hkAabbUtil.cpp.
// The two box forms are inline. The box-with-centre form sits in
// havok_component.obj (0x250438) and is defined in tu/blamlibxbox/havok_component.cpp;
// the box form is defined in tu/hkcollide/hkBoxShape.cpp.

class phys_aabb;
class phys_transform;
class phys_vec4;

class phys_aabb_util {
public:
    static void compute_bounds(const float *vertices, int vertex_count, int vertex_stride, phys_aabb &aabb);
    static void compute_bounds(const phys_transform &transform, const phys_vec4 &box_half_extents, float radius, phys_aabb &aabb);
    static void compute_bounds(const phys_transform &local_to_world, const phys_vec4 &box_half_extents, const phys_vec4 &center,
        float extra_radius, phys_aabb &aabb_out);
};

#endif // HAVOK_HK_AABB_UTIL_H
