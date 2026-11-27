#ifndef INCLUDE_PHYS_SOLVER_VELOCITY_H
#define INCLUDE_PHYS_SOLVER_VELOCITY_H

#include <vectorintrinsics.h>
#include "phys/phys_vel_accum.h"

namespace phys_solver {
    static __forceinline void load_vel_accums(const phys_vel_accum &a, const phys_vel_accum &b) {
        __VMXSetReg(86, a.m_lin_vel.m_simd_quad);
        __VMXSetReg(87, a.m_ang_vel.m_simd_quad);
        __VMXSetReg(88, a.m_inv_masses.m_simd_quad);
        __VMXSetReg(89, b.m_lin_vel.m_simd_quad);
        __VMXSetReg(90, b.m_ang_vel.m_simd_quad);
        __VMXSetReg(91, b.m_inv_masses.m_simd_quad);
    }
}

#endif
