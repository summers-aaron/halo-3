#ifndef INCLUDE_PHYS_SPHERE_H
#define INCLUDE_PHYS_SPHERE_H

#include "phys/phys_math.h"

class phys_sphere {
public:
    __forceinline phys_sphere() {}
    __forceinline phys_sphere(const phys_vec4 &pt, float radius) { m_sphere_center = pt; m_sphere_center(3) = radius; }
    __forceinline float get_convex_radius() const { return m_sphere_center(3); }
    __forceinline void set_shape_radius(float sphere_new_radius) { m_sphere_center(3) = sphere_new_radius; }
    __forceinline const phys_vec4 &get_contact_position() const { return m_sphere_center; }
    __forceinline const phys_vec4 &get_position_and_radius() const { return m_sphere_center; }
    __forceinline phys_vec4 &get_position_and_radius() { return m_sphere_center; }
    // March 0x82c97c20 preserves the radius component.
    __forceinline void contact_set_position(const phys_vec4 &pos) { m_sphere_center.set_xyz(pos); }
    __forceinline void set_position_and_radius(const phys_vec4 &pos) { m_sphere_center = pos; }

    phys_vec4 m_sphere_center;
};

typedef char phys_sphere_size[(sizeof(phys_sphere) == 0x10) ? 1 : -1];

#endif // INCLUDE_HAVOK_HK_SPHERE_H
