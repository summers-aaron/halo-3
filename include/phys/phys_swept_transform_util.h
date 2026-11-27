#ifndef INCLUDE_PHYS_SWEPT_TRANSFORM_UTIL_H
#define INCLUDE_PHYS_SWEPT_TRANSFORM_UTIL_H

#include "phys/phys_base.h"
#include "phys/phys_step_info.h"
#include "phys/phys_swept_transform_util_sweep.h"

namespace phys_swept_transform_util
{

        __forceinline void PHYS_CALL lerp_swept_rotation( const phys_swept_transform& swept_trans, phys_real t, phys_quat& lerp_quat_out );

        __forceinline void PHYS_CALL lerp_swept_rotation( const phys_swept_transform& swept_trans, phys_real t, phys_transform& xform_out );

        void PHYS_CALL lerp_swept( const phys_swept_transform& swept_trans, float time, phys_transform& xform_out );

        void PHYS_CALL lerp_swept_add_on( const phys_swept_transform& swept_trans, float t, phys_real t_add_on, phys_transform& xform_out );

        void PHYS_CALL lerp_swept_rel( const phys_swept_transform& swept_trans, phys_real r, phys_transform& xform_out );

        __forceinline void PHYS_CALL compute_transform_at_t0( const phys_swept_transform& swept_trans, phys_transform& xform_out );

        __forceinline void PHYS_CALL compute_transform_at_t1( const phys_swept_transform& swept_trans, phys_transform& xform_out );

        void PHYS_CALL set_centre_of_rotation_local( const phys_vec4& new_rotation_center, phys_motion_state& swept_trans);

        __forceinline void PHYS_CALL step_motion_state( const phys_step_info& step_info,
            phys_vec4& keyframe_lin_vel, phys_vec4& ang_velocity, phys_motion_state& motion_state );

        __forceinline void PHYS_CALL clip_motion_velocities( const phys_motion_state& motion_state, phys_vec4& keyframe_lin_vel, phys_vec4& ang_velocity );

        void PHYS_CALL keyframe_motion_state( const phys_step_info& step_info, const phys_vec4& target_pos,
            const phys_quat& swept_end_rot, phys_motion_state& motion_state );

        __forceinline void PHYS_CALL compute_center_of_mass_at( const phys_motion_state& ms, float t, phys_vec4& center_out );

        __forceinline void PHYS_CALL get_atom_velocity( const phys_motion_state& ms, phys_vec4& lin_vel_out, phys_vec4& ang_vel_out );

        void PHYS_CALL back_step_motion_state( float t, phys_motion_state& motion_state );

        void PHYS_CALL freeze_motion_state( float time, phys_motion_state& motion_state );

        __forceinline void PHYS_CALL deactivate( phys_motion_state& ms );

        void PHYS_CALL set_time_info( float step_start_time, phys_real inv_delta_time, phys_motion_state& motion_state);

        void PHYS_CALL warp_to_position( const phys_vec4& position, phys_motion_state& ms );

        void PHYS_CALL warp_to_rotation( const phys_quat& rotation, phys_motion_state& ms );

        void PHYS_CALL warp_to( const phys_vec4& position, const phys_quat& rotation, phys_motion_state& ms );

        void PHYS_CALL warp_to( const phys_transform& transform, phys_motion_state& ms );



        __forceinline void PHYS_CALL compute_ang_sweep_info( const phys_motion_state& motion_state0, const phys_motion_state& motion_state1,
            phys_real body_delta_time, phys_vec4* delta_ang_out0, phys_vec4* delta_ang_out1 );

}


void PHYS_CALL phys_swept_transform_util::lerp_swept_rotation( const phys_swept_transform& swept_trans, phys_real t, phys_quat& lerp_quat_out )
{
    phys_vec4 swept_start_rot = swept_trans.m_rotation_0.m_quat_vec;
    phys_vec4 swept_end_rot = swept_trans.m_rotation_1.m_quat_vec;
    {
        lerp_quat_out.m_quat_vec.set_add4( swept_start_rot, swept_end_rot );
        phys_real length_sq_quad = lerp_quat_out.m_quat_vec.length_sq_4d();
        phys_real r = 0.75f - 0.125f * length_sq_quad;
        r = r * (1.5f - 0.5f * length_sq_quad * r * r);
        lerp_quat_out.m_quat_vec.mul4(r);
    }

    if ( t < 0.5f )
    {
        lerp_quat_out.m_quat_vec.set_lerp4( swept_start_rot, lerp_quat_out.m_quat_vec, 2.0f * t);
    }
    else
    {
        lerp_quat_out.m_quat_vec.set_lerp4( lerp_quat_out.m_quat_vec, swept_end_rot, 2.0f * t - 1.0f);
    }
    lerp_quat_out.m_quat_vec.normalize_xyzw();
}

void PHYS_CALL phys_swept_transform_util::lerp_swept_rotation( const phys_swept_transform& swept_trans, phys_real t, phys_transform& xform_out )
{
    phys_quat qt;
    lerp_swept_rotation( swept_trans, t, qt );

    xform_out.set_rotation( qt );
    xform_out.get_translation().set_lerp4( swept_trans.m_center_of_mass0, swept_trans.m_center_of_mass1, t);

    phys_vec4 center_shift;
    center_shift.set_rotated_dir( xform_out.get_rotation(), swept_trans.m_center_of_mass_local);
    xform_out.get_translation().sub4( center_shift );
}

void PHYS_CALL phys_swept_transform_util::compute_transform_at_t0( const phys_swept_transform& swept_trans, phys_transform& xform_out )
{
    const phys_quat& qt = swept_trans.m_rotation_0;

    xform_out.set_rotation( qt );

    phys_vec4 center_shift;
    center_shift.set_rotated_dir( xform_out.get_rotation(), swept_trans.m_center_of_mass_local);
    xform_out.get_translation().set_sub4( swept_trans.m_center_of_mass0, center_shift );
}

void PHYS_CALL phys_swept_transform_util::compute_transform_at_t1( const phys_swept_transform& swept_trans, phys_transform& xform_out )
{
    const phys_quat& qt = swept_trans.m_rotation_1;

    xform_out.set_rotation( qt );

    phys_vec4 center_shift;
    center_shift.set_rotated_dir( xform_out.get_rotation(), swept_trans.m_center_of_mass_local);
    xform_out.get_translation().set_sub4( swept_trans.m_center_of_mass1, center_shift );
}

void PHYS_CALL phys_swept_transform_util::clip_motion_velocities( const phys_motion_state& motion_state, phys_vec4& keyframe_lin_vel, phys_vec4& ang_velocity )
{
    phys_real lin_vel_sq = keyframe_lin_vel.length_sq_3d();
    phys_real ang_vel_sq = ang_velocity.length_sq_3d();
    const phys_real max_lin_sq = motion_state.m_max_lin_vel * motion_state.m_max_lin_vel;
    const phys_real max_ang_sq = motion_state.m_max_ang_vel * motion_state.m_max_ang_vel;
    if ( (lin_vel_sq > max_lin_sq) || (lin_vel_sq!=lin_vel_sq) )
    {
        phys_real f = motion_state.m_max_lin_vel * phys_math::inv_sqrt( lin_vel_sq );
        keyframe_lin_vel.mul4( f );
        if ( lin_vel_sq!=lin_vel_sq )
        {
            keyframe_lin_vel = phys_transform::get_identity().get_column(0);
        }
    }
    if ( (ang_vel_sq > max_ang_sq) || ( ang_vel_sq != ang_vel_sq ))
    {
        phys_real f = motion_state.m_max_ang_vel * phys_math::inv_sqrt( ang_vel_sq );
        ang_velocity.mul4( f );
        if ( ang_vel_sq!=ang_vel_sq )
        {
            ang_velocity = phys_transform::get_identity().get_column(0);
        }
    }
}

void PHYS_CALL phys_swept_transform_util::step_motion_state( const phys_step_info& info,
    phys_vec4& keyframe_lin_vel, phys_vec4& ang_velocity,
    phys_motion_state& motion_state )
{

    {
        phys_vec4 abs_lin_vel; abs_lin_vel.set_abs4( keyframe_lin_vel );
        phys_vec4 abs_ang_vel; abs_ang_vel.set_abs4( ang_velocity );

        phys_vec4 max_vel_vec;
        max_vel_vec.set(1e6f, 1e6f, 1e6f, 1e6f);
        int mask = abs_lin_vel.compare_less4( max_vel_vec ).get_mask() & abs_ang_vel.compare_less4( max_vel_vec ).get_mask();
        mask    &= keyframe_lin_vel.compare_equal4( keyframe_lin_vel ).get_mask() & ang_velocity.compare_equal4( ang_velocity ).get_mask();
        if ( (mask & phys_vec4_cmp::CMP_MASK_XYZ) != phys_vec4_cmp::CMP_MASK_XYZ )
        {

            keyframe_lin_vel  = phys_transform::get_identity().get_column(0);
            ang_velocity = phys_transform::get_identity().get_column(0);
        }
    }

    motion_state.get_swept_transform().m_center_of_mass0 = motion_state.get_swept_transform().m_center_of_mass1;
    motion_state.get_swept_transform().m_center_of_mass0(3) = info.m_step_start_time;

    phys_real lin_vel_sq = keyframe_lin_vel.length_sq_3d();
    if ( lin_vel_sq > motion_state.m_max_lin_vel * motion_state.m_max_lin_vel )
    {

        phys_real f = motion_state.m_max_lin_vel * phys_math::inv_sqrt( lin_vel_sq );
        keyframe_lin_vel.mul4( f );
    }

    motion_state.get_swept_transform().m_center_of_mass1.add_scaled4(info.m_delta_time.val(), keyframe_lin_vel);
    motion_state.get_swept_transform().m_center_of_mass1(3) = info.m_inv_dt;

    phys_quat swept_new_rotation = motion_state.get_swept_transform().m_rotation_1;
    motion_state.get_swept_transform().m_rotation_0 = swept_new_rotation;

    phys_real angle;
    {
        phys_quat dq;    dq.m_quat_vec.set_mul4( info.m_delta_time * 0.5f, ang_velocity );
        const phys_real pi = 3.14159265358979f;
        phys_real dx_sq = phys_real(dq.m_quat_vec.length_sq_3d()) * (4.0f/ (pi*pi));

        const phys_real a = 0.822948f;
        const phys_real b = 0.130529f;
        const phys_real c = 0.044408f;

        const phys_real max_angle_per_step = phys_math::min_of_two( 0.9f, info.m_delta_time * motion_state.m_max_ang_vel );

        if ( dx_sq <= max_angle_per_step * max_angle_per_step )
        {
            const phys_real dx_pow4 = dx_sq * dx_sq;
            const phys_real w = 1.0f - a*dx_sq - b*dx_pow4 - c*dx_sq*dx_pow4;
            dq.m_quat_vec(3) = w;
        }
        else
        {
            phys_real factor = max_angle_per_step * phys_math::inv_sqrt( dx_sq );
            ang_velocity.mul4( factor );
            dq.m_quat_vec.mul4( factor );

                         dx_sq  = max_angle_per_step*max_angle_per_step;
            const phys_real dx_pow4  = dx_sq * dx_sq;
            const phys_real w    = 1.0f - a*dx_sq - b*dx_pow4 - c*dx_sq*dx_pow4;
            dq.m_quat_vec(3) = w;
        }

        swept_new_rotation.set_mul( dq, swept_new_rotation );
        swept_new_rotation.normalize();

        motion_state.m_delta_angle.set_add4( dq.m_quat_vec, dq.m_quat_vec );
        angle = phys_math::sqrt(dx_sq) * pi;
        motion_state.m_delta_angle(3) = angle;
    }
    motion_state.get_swept_transform().m_rotation_1 = swept_new_rotation;
    compute_transform_at_t1( motion_state.get_swept_transform(), motion_state.get_transform());
}

void PHYS_CALL phys_swept_transform_util::deactivate( phys_motion_state& ms )
{
    phys_swept_transform& swept_transform = ms.get_swept_transform();
    ms.m_delta_angle.set_zero4();

    swept_transform.m_rotation_0 = swept_transform.m_rotation_1;
    swept_transform.m_center_of_mass0 = swept_transform.m_center_of_mass1;
    swept_transform.m_center_of_mass1(3) = 0.0f;
}

void PHYS_CALL phys_swept_transform_util::compute_ang_sweep_info( const phys_motion_state& motion_state0, const phys_motion_state& motion_state1,
    phys_real body_delta_time, phys_vec4* delta_ang_out0, phys_vec4* delta_ang_out1)
{
    const phys_swept_transform& swept_a = motion_state0.get_swept_transform();
    const phys_swept_transform& swept_b = motion_state1.get_swept_transform();
    phys_real f0 = body_delta_time * swept_a.get_inv_delta_time();
    phys_real f1 = body_delta_time * swept_b.get_inv_delta_time();

    phys_vec4 ang_sq0; ang_sq0.set_mul4( f0, motion_state0.m_delta_angle );
    phys_vec4 ang_sq1; ang_sq1.set_mul4( f1, motion_state1.m_delta_angle );

    delta_ang_out0[0] = ang_sq0;
    delta_ang_out1[0] = ang_sq1;
}

void PHYS_CALL phys_swept_transform_util::compute_center_of_mass_at( const phys_motion_state& ms, float t, phys_vec4& center_out )
{
    phys_real iv = ms.get_swept_transform().get_interp_value( t );
    center_out.set_lerp4( ms.get_swept_transform().m_center_of_mass0, ms.get_swept_transform().m_center_of_mass1, iv );
}

void PHYS_CALL phys_swept_transform_util::get_atom_velocity( const phys_motion_state& ms, phys_vec4& lin_vel_out, phys_vec4& ang_vel_out )
{
    lin_vel_out.set_sub4 (ms.get_swept_transform().m_center_of_mass1, ms.get_swept_transform().m_center_of_mass0);
    lin_vel_out.mul4( ms.get_swept_transform().get_inv_delta_time() );
    ang_vel_out.set_mul4( ms.get_swept_transform().get_inv_delta_time(), ms.m_delta_angle );
}


#endif // INCLUDE_HAVOK_HK_SWEPT_TRANSFORM_UTIL_H
