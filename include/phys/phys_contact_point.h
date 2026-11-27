#ifndef INCLUDE_PHYS_CONTACT_POINT_H
#define INCLUDE_PHYS_CONTACT_POINT_H

#include "phys/phys_math_value_types.h"

class phys_contact_point {
public:
    // March 0x82ba74e8, 0x8349ba98 and 0x83499030.
    __forceinline const phys_vec4 &get_contact_normal() const { return m_separating_normal; }
    __forceinline void set_contact_normal(const phys_vec4 &normal) { m_separating_normal.set_xyz(normal); }
    __forceinline void set_separating_normal(const phys_vec4 &normal) { m_separating_normal = normal; }
    __forceinline const phys_vec4 &get_contact_position() const { return m_position; }
    __forceinline phys_vec4 &get_contact_position() { return m_position; }
    __forceinline const phys_vec4 &get_separating_normal() const { return m_separating_normal; }
    __forceinline phys_vec4 &get_separating_normal() { return m_separating_normal; }
    __forceinline float get_distance() const { return m_separating_normal(3); }
    __forceinline phys_simd_scalar get_distance_simd() const { return m_separating_normal.get_simd_at(3); }
    __forceinline void contact_set_position(const phys_vec4 &pos) { m_position = pos; }
    __forceinline void set_separating_normal(const phys_vec4 &normal, float dist) {
        m_separating_normal = normal;
        m_separating_normal(3) = dist;
    }
    __forceinline void contact_set_distance(float d) { m_separating_normal(3) = d; }
    // March body (hkBvTreeAgent.obj emits it): the contact as seen from the other body.
    __forceinline void set_flipped_contact(const phys_contact_point &other) {
        phys_vec4 shift;
        shift.set_broadcast(other.m_separating_normal, 3);
        m_position.set_add_mul4(other.m_position, other.m_separating_normal, shift);
        m_separating_normal.set_neg3(other.m_separating_normal);
    }
    // March 0x8355cb40.
    __forceinline void flip() { set_flipped_contact(*this); }

    phys_vec4 m_position;
    phys_vec4 m_separating_normal;
};

typedef char phys_contact_point_size[(sizeof(phys_contact_point) == 0x20) ? 1 : -1];

#endif // INCLUDE_HAVOK_HK_CONTACT_POINT_H
