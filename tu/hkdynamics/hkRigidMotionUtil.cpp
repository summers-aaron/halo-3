// hkdynamics/hkRigidMotionUtil.obj: from the records 0x65b810 0x65b850 0x65b948 0x65be10 0x65cbe8 0x65d040
// @flags /TP /O2 /Oc
#include "phys/phys_base_types.h"
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#include "phys/phys_rigid_motion_util.h"
#include "phys/phys_step_info.h"
#include "phys/phys_half.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_vel_accum.h"
#include "phys/phys_math_prefetch.h"
#include "phys/phys_vec4_util.h"
// @address 0x6f49f4 __savevmx_120
// @address 0x6f4c8c __restvmx_120
// @address 0x65b810 ?compute_sleep_velocity@@YAMABVphys_motion@@M@Z

// match: 0x65b948/0x65be10/0x65d040 retain the native scalar helper's rounding boundary.
static __forceinline float phys_rigid_motion_sqrt_inv(float r)
{
    return (1.0f / __fsqrts(r));
}

namespace phys_swept_transform_util {
    __forceinline void compute_center_of_mass_at(const phys_motion_state &ms, float t, phys_vec4 &center_out)
    {
        float iv = ms.get_swept_transform().get_interp_value(t);
        center_out.set_lerp4(ms.get_swept_transform().m_center_of_mass0, ms.get_swept_transform().m_center_of_mass1, iv);
    }
}

// @address 0xfa0 k_phys_rigid_motion_max_vel
const __vector4 k_phys_rigid_motion_max_vel = { 1000000.0f, 1000000.0f, 1000000.0f, 1000000.0f };

namespace phys_swept_transform_util {
    __forceinline void compute_transform_at_t1(const phys_swept_transform &swept_trans, phys_transform &xform_out)
    {
        const phys_quat &qt = swept_trans.m_rotation_1;

        xform_out.set_rotation(qt);

        phys_vec4 center_shift;
        center_shift.set_rotated_dir(xform_out.get_rotation(), swept_trans.m_center_of_mass_local);
        xform_out.get_translation().set_sub4(swept_trans.m_center_of_mass1, center_shift);
    }

    __forceinline void step_motion_state(const phys_step_info &info, phys_vec4 &keyframe_lin_vel, phys_vec4 &ang_velocity, phys_motion_state &motion_state)
    {
        {
            phys_vec4 abs_lin_vel; abs_lin_vel.set_abs4(keyframe_lin_vel);
            phys_vec4 abs_ang_vel; abs_ang_vel.set_abs4(ang_velocity);

            const phys_vec4 &max_vel_vec = reinterpret_cast<const phys_vec4 &>(k_phys_rigid_motion_max_vel);
            int mask = abs_lin_vel.compare_less4(max_vel_vec).get_mask() & abs_ang_vel.compare_less4(max_vel_vec).get_mask();
            mask &= keyframe_lin_vel.compare_equal4(keyframe_lin_vel).get_mask() & ang_velocity.compare_equal4(ang_velocity).get_mask();
            if ((mask & phys_vec4_cmp::CMP_MASK_XYZ) != phys_vec4_cmp::CMP_MASK_XYZ)
            {
                keyframe_lin_vel = phys_transform::get_identity().get_column(0);
                ang_velocity = phys_transform::get_identity().get_column(0);
            }
        }

        motion_state.get_swept_transform().m_center_of_mass0 = motion_state.get_swept_transform().m_center_of_mass1;
        motion_state.get_swept_transform().m_center_of_mass0(3) = info.m_step_start_time;

        float lin_vel_sq = keyframe_lin_vel.length_sq_3d();
        if (lin_vel_sq > motion_state.m_max_lin_vel * motion_state.m_max_lin_vel)
        {
            float f = motion_state.m_max_lin_vel * phys_rigid_motion_sqrt_inv(lin_vel_sq);
            keyframe_lin_vel.mul4(f);
        }

        motion_state.get_swept_transform().m_center_of_mass1.add_scaled4(info.m_delta_time.val(), keyframe_lin_vel);
        motion_state.get_swept_transform().m_center_of_mass1(3) = info.m_inv_dt;

        phys_quat swept_new_rotation = motion_state.get_swept_transform().m_rotation_1;
        motion_state.get_swept_transform().m_rotation_0 = swept_new_rotation;

        float angle;
        {
            phys_quat dq; dq.m_quat_vec.set_mul4(info.m_delta_time * 0.5f, ang_velocity);
            const float pi = 3.14159265358979f;
            float dx_sq = float(dq.m_quat_vec.length_sq_3d()) * (4.0f / (pi * pi));

            const float a = 0.822948f;
            const float b = 0.130529f;
            const float c = 0.044408f;

            const float max_angle_per_step = phys_math::min_of_two(0.9f, info.m_delta_time.val() * motion_state.m_max_ang_vel);

            if (dx_sq <= max_angle_per_step * max_angle_per_step)
            {
                const float dx_pow4 = dx_sq * dx_sq;
                const float w = 1.0f - a * dx_sq - b * dx_pow4 - c * dx_sq * dx_pow4;
                dq.m_quat_vec(3) = w;
            }
            else
            {
                float factor = max_angle_per_step * phys_rigid_motion_sqrt_inv(dx_sq);
                ang_velocity.mul4(factor);
                dq.m_quat_vec.mul4(factor);

                dx_sq = max_angle_per_step * max_angle_per_step;
                const float dx_pow4 = dx_sq * dx_sq;
                const float w = 1.0f - a * dx_sq - b * dx_pow4 - c * dx_sq * dx_pow4;
                dq.m_quat_vec(3) = w;
            }

            swept_new_rotation.set_mul(dq, swept_new_rotation);
            swept_new_rotation.normalize();

            motion_state.m_delta_angle.set_add4(dq.m_quat_vec, dq.m_quat_vec);
            angle = phys_math::sqrt(dx_sq) * pi;
            motion_state.m_delta_angle(3) = angle;
        }
        motion_state.get_swept_transform().m_rotation_1 = swept_new_rotation;
        compute_transform_at_t1(motion_state.get_swept_transform(), motion_state.get_transform());
    }

    __forceinline void clip_motion_velocities(const phys_motion_state &motion_state, phys_vec4 &keyframe_lin_vel, phys_vec4 &ang_velocity)
    {
        float lin_vel_sq = keyframe_lin_vel.length_sq_3d();
        float ang_vel_sq = ang_velocity.length_sq_3d();
        const float max_lin_sq = motion_state.m_max_lin_vel * motion_state.m_max_lin_vel;
        const float max_ang_sq = motion_state.m_max_ang_vel * motion_state.m_max_ang_vel;
        if ((lin_vel_sq > max_lin_sq) || (lin_vel_sq != lin_vel_sq))
        {
            float f = motion_state.m_max_lin_vel * phys_rigid_motion_sqrt_inv(lin_vel_sq);
            keyframe_lin_vel.mul4(f);
            if (lin_vel_sq != lin_vel_sq)
            {
                keyframe_lin_vel = phys_transform::get_identity().get_column(0);
            }
        }
        if ((ang_vel_sq > max_ang_sq) || (ang_vel_sq != ang_vel_sq))
        {
            float f = motion_state.m_max_ang_vel * phys_rigid_motion_sqrt_inv(ang_vel_sq);
            ang_velocity.mul4(f);
            if (ang_vel_sq != ang_vel_sq)
            {
                ang_velocity = phys_transform::get_identity().get_column(0);
            }
        }
    }
}

// Preserve the accepted havok_component layout and virtual slots.
// Byte/float storage keeps vector alignment from shifting the vfptr padding.

float compute_sleep_velocity(const phys_motion &motion, float radius)
{
    float lin_speed_sq = motion.get_linear_velocity().length_sq_3d();
    float ang_speed_sq = motion.get_angular_velocity().length_sq_3d();
    return ang_speed_sq * radius * radius + lin_speed_sq;
}

// @address 0x65b850 phys_rigid_motion_can_sleep_final
extern "C" unsigned char phys_rigid_motion_can_sleep_final(
    const phys_solver_info *solver_info, void *step_info, phys_motion *const *motion_list, int motion_count, int motion_offset)
{
    for (int i = motion_count - 1; i >= 0; i--, motion_list++)
    {
        phys_motion &motion = *(phys_motion *)((char *)motion_list[0] + motion_offset);
        float radius = motion.m_motion_state.m_object_radius;
        radius = (1.0f - radius >= 0.0f) ? radius : 1.0f;
        float lin_speed_sq = motion.get_linear_velocity().length_sq_3d();
        float ang_speed_sq = motion.get_angular_velocity().length_sq_3d();
        float vel_sq_metric = (ang_speed_sq * radius * radius + lin_speed_sq) * 0.25f - 0.010000001f;
        if ((motion.m_inactive_frame_counts[0] & 127) > (motion.m_inactive_frame_counts[1] & 127))
        {
            if (vel_sq_metric > motion.m_sleep_ref_position[0](3))
            {
                unsigned int packed = ((motion.m_inactive_frame_counts[0] & 127)
                    | (solver_info->m_inactive_frame_select[0] << 7)) << 7;
                motion.m_inactive_frame_counts[0] = (unsigned short)packed;
                return 0;
            }
        }
        else if (vel_sq_metric > motion.m_sleep_ref_position[1](3))
        {
            unsigned int packed = ((motion.m_inactive_frame_counts[1] & 127) | (solver_info->m_inactive_frame_select[1] << 7)) << 7;
            motion.m_inactive_frame_counts[1] = (unsigned short)packed;
            return 0;
        }
    }
    return 1;
}

// @address 0x65b948 phys_rigid_motion_step
extern "C" void phys_rigid_motion_step(const phys_step_info &info, phys_motion *const *motion_list, int motion_count, int motion_offset)
{
    for (int i = motion_count - 1; i >= 0; motion_list++, i--)
    {
        phys_motion *motion = phys_add_byte_offset(motion_list[0], motion_offset);
        phys_swept_transform_util::step_motion_state(info, motion->m_linear_velocity, motion->m_ang_velocity, motion->m_motion_state);
    }
}

static __forceinline void phys_rigid_motion_sleep_counters(const phys_solver_info &si, phys_motion &motion)
{
    unsigned int c = motion.m_integrate_sleep_counter;
    c++;
    motion.m_integrate_sleep_counter = (unsigned char)c;
    if ((c & 3) == 0)
    {
        int select;
        if ((c & 15) != 0)
        {
            select = 0;
        }
        else
        {
            if (c == 0x100)
            {
                motion.m_integrate_sleep_counter = 0xff;
                return;
            }
            motion.m_integrate_sleep_counter = 0;
            select = 1;
        }

        float radius = phys_math::min_of_two(1.0f, motion.m_motion_state.m_object_radius);
        float vel_sq_metric = compute_sleep_velocity(motion, radius);
        motion.m_sleep_ref_position[select](3) = phys_math::max_of_two(motion.m_sleep_ref_position[select](3), vel_sq_metric);

        phys_vec4 &ref_position = motion.m_sleep_ref_position[select];
        unsigned int &ref_orientation = motion.m_motion_state.m_sleep_ref_orientation[select];

        const phys_swept_transform &swept_transform = motion.m_motion_state.get_swept_transform();
        const phys_vec4 &cur_position = swept_transform.m_center_of_mass1;
        const phys_quat &cur_rotation = swept_transform.m_rotation_1;

        while (1)
        {
            const phys_solver_info::sleep_info &di = si.m_deactivation_info[motion.m_motion_state.m_deactivation_class];
            {
                const float max_dist_sq = di.m_max_dist_sq[select];
                phys_vec4 trans_dist; trans_dist.set_sub4(ref_position, cur_position);
                const float trans_dist_sq = trans_dist.length_sq_3d();
                if (trans_dist_sq > max_dist_sq)
                {
                    break;
                }
            }
            {
                phys_quat ref_quat; phys_vec4_util::unpack_i32_to_quat(ref_orientation, ref_quat.m_quat_vec);
                const float max_dist_sq = di.m_max_rot_sq[select];
                phys_vec4 dist; dist.set_sub4(ref_quat.m_quat_vec, cur_rotation.m_quat_vec);
                const float trans_dist_sq = dist.length_sq_4d();
                if (trans_dist_sq > max_dist_sq)
                {
                    break;
                }
            }
            {
                motion.bump_inactive_frames_mt(select, si.m_inactive_frame_select[select]);
                return;
            }
        }

        motion.zero_inactive_frames_mt(select, si.m_inactive_frame_select[select]);
        {
            phys_vec4 pos = cur_position;
            pos.zero_element(3);
            ref_position = pos;
            ref_orientation = phys_vec4_util::pack_quat_to_u32(cur_rotation.m_quat_vec);
        }
    }
}

static __forceinline int phys_rigid_motion_check_sleep(const phys_solver_info &si, phys_motion &motion)
{
    phys_rigid_motion_sleep_counters(si, motion);
    return phys_math::max_of_two(motion.get_inactive_frame_count(0), motion.get_inactive_frame_count(1));
}

// @address 0x65be10 phys_rigid_motion_force_step
extern "C" int phys_rigid_motion_force_step(const phys_solver_info &solver_info, const phys_step_info &info, const phys_vec4 &delta_vel,
    phys_motion *const *motion_list, int motion_count, int motion_offset)
{
    int inactive_frame_count = 0x7fffffff;
    for (int i = motion_count - 1; i >= 0; motion_list++, i--)
    {
        phys_motion *motion = phys_add_byte_offset(motion_list[0], motion_offset);

        switch (motion->m_type)
        {
            case phys_motion::MOTION_FIXED:
            {
                continue;
            }
            case phys_motion::BODY_MOTION_THIN_BOX_INERTIA:
            {
                phys_motion *motion_view = motion;
                motion_view->m_linear_velocity.add4(delta_vel);
                const float one = 1.0f;
                const float body_delta_time = info.m_delta_time;
                motion_view->m_linear_velocity.mul4(phys_math::max_of_two(0.0f, one - body_delta_time * motion_view->m_motion_state.m_linear_damping));
                motion_view->m_ang_velocity.mul4(phys_math::max_of_two(0.0f, one - body_delta_time * motion_view->m_motion_state.m_ang_damping));

                phys_vec4 ang_momentum;
                {
                    phys_vec4 h; h.vec_set_rotated_inverse_dir(motion_view->get_transform().get_rotation(), motion_view->m_ang_velocity);
                    phys_vec4 motion_inv_inertia; motion_inv_inertia.set_reciprocal4(motion_view->m_inv_inertia_mass);
                    h.mul4(motion_inv_inertia);
                    ang_momentum.vec_set_rotated_dir(motion_view->get_transform().get_rotation(), h);
                }

                phys_swept_transform_util::step_motion_state(info, motion_view->m_linear_velocity, motion_view->m_ang_velocity, motion_view->m_motion_state);

                {
                    phys_vec4 h; h.vec_set_rotated_inverse_dir(motion_view->get_transform().get_rotation(), ang_momentum);
                    h.mul4(motion_view->m_inv_inertia_mass);
                    motion_view->m_ang_velocity.vec_set_rotated_dir(motion_view->get_transform().get_rotation(), h);
                }

                phys_swept_transform_util::clip_motion_velocities(motion->m_motion_state, motion->m_linear_velocity, motion->m_ang_velocity);
                goto check_deactivation_label;
            }
            case phys_motion::MOTION_KEYFRAMED:
            {
                break;
            }
            default:
            {
                motion->m_linear_velocity.add4(delta_vel);
                const float one = 1.0f;
                const float body_delta_time = info.m_delta_time;
                motion->m_linear_velocity.mul4(phys_math::max_of_two(0.0f, one - body_delta_time * motion->m_motion_state.m_linear_damping));
                motion->m_ang_velocity.mul4(phys_math::max_of_two(0.0f, one - body_delta_time * motion->m_motion_state.m_ang_damping));
                break;
            }
        }
        phys_swept_transform_util::step_motion_state(info, motion->m_linear_velocity, motion->m_ang_velocity, motion->m_motion_state);
check_deactivation_label:
        inactive_frame_count = phys_math::min_of_two(inactive_frame_count, phys_rigid_motion_check_sleep(solver_info, *motion));
    }
    return inactive_frame_count;
}

// @address 0x65cbe8 phys_rigid_motion_build_accums
extern "C" phys_vel_accum *phys_rigid_motion_build_accums(
    const phys_step_info &info, phys_motion *const *motion_list, int motion_count, int motion_offset, phys_vel_accum *accums_out)
{
    for (int i = motion_count - 1; i >= 0; motion_list++, accums_out++, i--)
    {
        phys_motion *motion = phys_add_byte_offset(motion_list[0], motion_offset);
        if (i > 1)
        {
            phys_motion *next = phys_add_byte_offset(motion_list[2], motion_offset);
            void *p2 = phys_add_byte_offset(accums_out, 256);
            phys_math::force_prefetch<256>(next);
            phys_math::prefetch_128(p2);
        }

        switch (motion->m_type)
        {
            case phys_motion::BODY_MOTION_INVALID:
            {
            }
            case phys_motion::BODY_MOTION_BOX_INERTIA:
            case phys_motion::BODY_MOTION_STABLE_BOX_INERTIA:
            case phys_motion::BODY_MOTION_THIN_BOX_INERTIA:
            {
                const float one = 1.0f;
                const float body_delta_time = info.m_delta_time;
                motion->m_linear_velocity.mul4(phys_math::max_of_two(0.0f, one - body_delta_time * motion->m_motion_state.m_linear_damping));
                motion->m_ang_velocity.mul4(phys_math::max_of_two(0.0f, one - body_delta_time * motion->m_motion_state.m_ang_damping));

                phys_vel_accum *accumulator = accums_out;
                accumulator->get_core_from_world().set_transpose(motion->get_transform().get_rotation());
                accumulator->m_type = phys_vel_accum::PHYS_RIGID_BODY;
                const phys_motion *base_motion = motion;
                accumulator->m_inv_masses = motion->m_inv_inertia_mass;
                accumulator->m_lin_vel = motion->m_linear_velocity;
                phys_motion_state &ms = motion->m_motion_state;
                phys_swept_transform_util::compute_center_of_mass_at(ms, info.m_step_start_time, accumulator->get_center_of_mass_in_world());
                accumulator->m_ang_vel.set_rotated_dir(accumulator->get_core_from_world(), motion->m_ang_velocity);
                accumulator->m_mat_is_identity = false;
                accumulator->m_deactivation_class = motion->m_motion_state.m_deactivation_class;
                accumulator->m_deactivation_counter = motion->m_motion_state.m_deactivation_counter;
                break;
            }
            case phys_motion::BODY_MOTION_SPHERE_INERTIA:
            case phys_motion::BODY_MOTION_STABLE_SPHERE_INERTIA:
            {
                const float one = 1.0f;
                const float body_delta_time = info.m_delta_time;
                motion->m_linear_velocity.mul4(phys_math::max_of_two(0.0f, one - body_delta_time * motion->m_motion_state.m_linear_damping));
                motion->m_ang_velocity.mul4(phys_math::max_of_two(0.0f, one - body_delta_time * motion->m_motion_state.m_ang_damping));

                phys_vel_accum *accumulator = accums_out;
                accumulator->m_type = phys_vel_accum::PHYS_RIGID_BODY;
                accumulator->m_inv_masses = motion->m_inv_inertia_mass;
                phys_motion_state &ms = motion->m_motion_state;
                phys_swept_transform_util::compute_center_of_mass_at(ms, info.m_step_start_time, accumulator->get_center_of_mass_in_world());
                accumulator->m_ang_vel = motion->m_ang_velocity;
                accumulator->m_lin_vel = motion->m_linear_velocity;
                accumulator->get_core_from_world().set_to_identity();
                accumulator->m_mat_is_identity = true;
                accumulator->m_deactivation_class = motion->m_motion_state.m_deactivation_class;
                accumulator->m_deactivation_counter = motion->m_motion_state.m_deactivation_counter;
                break;
            }
            case phys_motion::MOTION_FIXED:
            {
                phys_vel_accum *accumulator = accums_out;
                accumulator->make_fixed_accumulator();
                break;
            }
            case phys_motion::MOTION_KEYFRAMED:
            {
                phys_vel_accum *accumulator = accums_out;
                accumulator->m_type = phys_vel_accum::PHYS_VEL_ACCUM_KEYFRAMED_BODY;
                accumulator->m_inv_masses.set_zero4();
                accumulator->get_center_of_mass_in_world() = motion->get_center_of_mass_in_world();
                accumulator->m_ang_vel = motion->m_ang_velocity;
                accumulator->m_lin_vel = motion->m_linear_velocity;
                accumulator->get_core_from_world().set_to_identity();
                accumulator->m_mat_is_identity = true;
                break;
            }
            default:
            {
            }
        }
    }
    return accums_out;
}

// @address 0x65d040 phys_rigid_motion_apply_accums
extern "C" int phys_rigid_motion_apply_accums(const phys_solver_info &solver_info, const phys_step_info &info,
    const phys_vel_accum *accum_array, phys_motion *const *motion_list, int motion_count, int motion_offset)
{
    const phys_vel_accum *vel_accum = accum_array;

    int inactive_frame_count = 0x7fffffff;
    phys_vec4 integ_lin_vel;
    phys_vec4 integ_ang_vel;
    phys_vec4 ang_momentum;

    for (int i = motion_count - 1; i >= 0; motion_list++, vel_accum++, i--)
    {
        phys_motion *motion = phys_add_byte_offset(motion_list[0], motion_offset);
        if (i > 1)
        {
            phys_motion *next = phys_add_byte_offset(motion_list[2], motion_offset);
            const void *p2 = phys_add_byte_offset_const(vel_accum, 256);
            phys_math::force_prefetch<256>(next);
            phys_math::prefetch_128(p2);
        }

        switch (motion->m_type)
        {
            case phys_motion::BODY_MOTION_INVALID:
            {
            }
            case phys_motion::BODY_MOTION_THIN_BOX_INERTIA:
            {
                phys_motion *motion_view = motion;
                phys_vec4 ang_vel_local = vel_accum->m_ang_vel;
                phys_vec4 motion_inv_inertia; motion_inv_inertia.set_reciprocal4(motion_view->m_inv_inertia_mass);
                ang_vel_local.mul4(motion_inv_inertia);
                ang_momentum.vec_set_rotated_dir(motion_view->get_transform().get_rotation(), ang_vel_local);
            }
            case phys_motion::BODY_MOTION_BOX_INERTIA:
            {
                motion->m_linear_velocity = vel_accum->m_lin_vel;
                motion->m_ang_velocity.set_rotated_dir(motion->get_transform().get_rotation(), vel_accum->m_ang_vel);
                integ_lin_vel = vel_accum->get_sum_lin_vel();
                integ_ang_vel.vec_set_rotated_dir(motion->get_transform().get_rotation(), vel_accum->get_sum_ang_vel());
                goto sphere_inertia_label;
            }
            case phys_motion::BODY_MOTION_STABLE_BOX_INERTIA:
            {
                motion->m_linear_velocity = vel_accum->get_sum_lin_vel();
                motion->m_ang_velocity = vel_accum->get_sum_ang_vel();
                integ_lin_vel = vel_accum->get_sum_lin_vel();
                integ_ang_vel.vec_set_rotated_dir(motion->get_transform().get_rotation(), vel_accum->get_sum_ang_vel());
                goto sphere_inertia_label;
            }
            case phys_motion::BODY_MOTION_STABLE_SPHERE_INERTIA:
            {
                motion->m_linear_velocity = vel_accum->get_sum_lin_vel();
                motion->m_ang_velocity = vel_accum->get_sum_ang_vel();
                integ_lin_vel = vel_accum->get_sum_lin_vel();
                integ_ang_vel = vel_accum->get_sum_ang_vel();
                goto sphere_inertia_label;
            }
            case phys_motion::MOTION_KEYFRAMED:
            {
                motion->m_linear_velocity = motion->m_linear_velocity;
                motion->m_ang_velocity = motion->m_ang_velocity;
                integ_lin_vel = motion->m_linear_velocity;
                integ_ang_vel = motion->m_ang_velocity;
                goto sphere_inertia_no_sleep_count;
            }
            case phys_motion::BODY_MOTION_SPHERE_INERTIA:
            {
                motion->m_linear_velocity = vel_accum->m_lin_vel;
                motion->m_ang_velocity = vel_accum->m_ang_vel;
                integ_lin_vel = vel_accum->get_sum_lin_vel();
                integ_ang_vel = vel_accum->get_sum_ang_vel();
sphere_inertia_label:
                motion->m_motion_state.m_deactivation_counter = (unsigned short)(vel_accum->m_deactivation_counter);
sphere_inertia_no_sleep_count:
                phys_swept_transform_util::step_motion_state(info, integ_lin_vel, integ_ang_vel, motion->m_motion_state);
                break;
            }
            case phys_motion::MOTION_FIXED:
            default:
            {
                break;
            }
        }

        if (motion->m_type == phys_motion::BODY_MOTION_THIN_BOX_INERTIA)
        {
            phys_motion *motion_view = motion;
            motion_view->m_linear_velocity = vel_accum->m_lin_vel;
            {
                phys_vec4 h; h.vec_set_rotated_inverse_dir(motion_view->get_transform().get_rotation(), ang_momentum);
                h.mul4(motion_view->m_inv_inertia_mass);
                motion_view->m_ang_velocity.vec_set_rotated_dir(motion_view->get_transform().get_rotation(), h);
            }
        }
        phys_swept_transform_util::clip_motion_velocities(motion->m_motion_state, motion->m_linear_velocity, motion->m_ang_velocity);
        inactive_frame_count = phys_math::min_of_two(inactive_frame_count, phys_rigid_motion_check_sleep(solver_info, *motion));
    }
    return inactive_frame_count;
}
