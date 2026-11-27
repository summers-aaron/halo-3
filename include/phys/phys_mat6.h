#ifndef INCLUDE_PHYS_MAT6_H
#define INCLUDE_PHYS_MAT6_H

// Functions defined in tu/hkmath/hkMatrix6.cpp.

class phys_mat6;
class phys_vec8;

extern "C" void phys_mat6_sub(phys_mat6& mat_a_out, const phys_mat6& b);
extern "C" void phys_mat6_set_mul(phys_mat6& out, const phys_mat6&a, const phys_mat6&b);
extern "C" void phys_mat6_set_transpose(phys_mat6& out, const phys_mat6&in);
extern "C" void phys_mat6_set_invert(phys_mat6& out, const phys_mat6& in );
extern "C" void phys_mat6_set_mul_vec(phys_vec8& out, const phys_mat6& a, const phys_vec8& b);
extern "C" void phys_mat6_add(phys_mat6& mat_a_out, const phys_mat6& b);

#endif // HAVOK_HK_MATRIX6_H
