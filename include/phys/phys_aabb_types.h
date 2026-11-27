#ifndef INCLUDE_PHYS_AABB_TYPES_H
#define INCLUDE_PHYS_AABB_TYPES_H

#include "phys/phys_math.h"
#include "phys/phys_thread_memory.h"

// hkAabb: an axis-aligned box. Methods are the ones the March 2007 debug build emits for it;
// bodies follow that build's code.
class phys_aabb {
public:
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_COLLISION_INFO, phys_aabb);

    // match: 0x25a7d0: the SDK ordinary inline constructor preserves the world-load alias ordering.
    phys_aabb() {}
    __forceinline phys_aabb(const phys_vec4 &min, const phys_vec4 &max) : m_min_limit(min), m_max_limit(max) {}

    // Checks that the box bounds are well-formed.
    phys_bool is_valid_aabb() const;

    __forceinline phys_bool contains(const phys_aabb &other) const {
        phys_vec4_cmp min_cmp = m_min_limit.compare_less_equal4(other.m_min_limit);
        phys_vec4_cmp max_cmp = other.m_max_limit.compare_less_equal4(m_max_limit);
        phys_vec4_cmp both_cmp;
        both_cmp.set_and(min_cmp, max_cmp);
        return both_cmp.all_are_set(phys_vec4_cmp::CMP_MASK_XYZ) != 0;
    }
    __forceinline phys_bool aabb_contains_point(const phys_vec4 &point) const {
        phys_vec4_cmp min_cmp = m_min_limit.compare_less_equal4(point);
        phys_vec4_cmp max_cmp = point.compare_less_equal4(m_max_limit);
        phys_vec4_cmp both_cmp;
        both_cmp.set_and(min_cmp, max_cmp);
        return both_cmp.all_are_set(phys_vec4_cmp::CMP_MASK_XYZ) != 0;
    }

    phys_vec4 m_min_limit;
    phys_vec4 m_max_limit;
};

typedef char phys_aabb_size[(sizeof(phys_aabb) == 0x20) ? 1 : -1];

#endif // INCLUDE_HAVOK_HK_AABB_TYPES_H
