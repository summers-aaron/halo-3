#ifndef INCLUDE_PHYS_VEC8_H
#define INCLUDE_PHYS_VEC8_H

#include "phys/phys_base.h"

class phys_mat6;

class phys_vec8 {
public:
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_MATH, phys_vec8);

    phys_vec4 m_lin_block;
    phys_vec4 m_ang_half;

    __forceinline void flags_set_all(phys_real x) { m_lin_block.flags_set_all(x); m_ang_half.flags_set_all(x); }
    __forceinline void set_all6(phys_real x) { m_lin_block.set_all3(x); m_ang_half.set_all3(x); }
    __forceinline void set_zero8() { m_lin_block.set_zero4(); m_ang_half.set_zero4(); }
    __forceinline void add_vec8(const phys_vec8 &a) { m_lin_block.add4(a.m_lin_block); m_ang_half.add4(a.m_ang_half); }
    __forceinline void add6_clobber_w(const phys_vec8 &a) { m_lin_block.add3_clobber_w(a.m_lin_block); m_ang_half.add3_clobber_w(a.m_ang_half); }
    __forceinline void set_sub8(const phys_vec8 &a, const phys_vec8 &b) { m_lin_block.set_sub4(a.m_lin_block,
        b.m_lin_block); m_ang_half.set_sub4(a.m_ang_half, b.m_ang_half); }
    __forceinline void mul_vec8(phys_simd_scalar_param a) { m_lin_block.mul4(a); m_ang_half.mul4(a); }
    __forceinline void set_neg8() { m_lin_block.set_neg4(m_lin_block); m_ang_half.set_neg4(m_ang_half); }
    __forceinline void add_scaled8(phys_simd_scalar_param r, const phys_vec8 &a) { m_lin_block.add_scaled4(r,
        a.m_lin_block); m_ang_half.add_scaled4(r, a.m_ang_half); }

    // Defined with hkMatrix6 in its owning unit; March 0x83527588.
    __forceinline void set_mul_mat6(const phys_mat6 &a, const phys_vec8 &b);
};

typedef char phys_vec8_size[(sizeof(phys_vec8) == 32) ? 1 : -1];
typedef char phys_vec8_ang_offset[(offsetof(phys_vec8, m_ang_half) == 16) ? 1 : -1];

#endif
