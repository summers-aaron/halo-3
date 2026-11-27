#ifndef INCLUDE_PHYS_RIGID_MOTION_UTIL_H
#define INCLUDE_PHYS_RIGID_MOTION_UTIL_H

#include <vectorintrinsics.h>

// Functions defined in tu/hkdynamics/hkRigidMotionUtil.cpp that other units call.

class phys_motion;
struct phys_solver_info;
struct phys_step_info;
class phys_vec4;
class phys_vel_accum;

extern "C" void phys_rigid_motion_step(const phys_step_info &info, phys_motion *const *motion_list, int motion_count, int motion_offset);
extern "C" int phys_rigid_motion_force_step(const phys_solver_info &solver_info, const phys_step_info &info,
    const phys_vec4 &delta_vel, phys_motion *const *motion_list, int motion_count, int motion_offset);
extern "C" phys_vel_accum *phys_rigid_motion_build_accums(const phys_step_info &info, phys_motion *const *motion_list,
    int motion_count, int motion_offset, phys_vel_accum *accums_out);
extern "C" int phys_rigid_motion_apply_accums(const phys_solver_info &solver_info, const phys_step_info &info,
    const phys_vel_accum *accum_array, phys_motion *const *motion_list, int motion_count, int motion_offset);

extern "C" unsigned char phys_rigid_motion_can_sleep_final(const phys_solver_info *solver_info, void *step_info,
    phys_motion *const *motion_list, int motion_count, int motion_offset);

float compute_sleep_velocity(const phys_motion &motion, float radius);

extern "C" const __vector4 k_phys_rigid_motion_max_vel;

#endif // HAVOK_HK_RIGID_MOTION_UTIL_H
