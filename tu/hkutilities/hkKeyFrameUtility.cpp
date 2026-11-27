// hkutilities/hkKeyFrameUtility.obj: from the records 0x612b30 0x612c18 0x612c88 0x612e00 0x6130c8
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x612b30 ?fast_set_from_pos_orient@KeyFrameInfo@phys_keyframe_util@@QAAXABVphys_vec4@@ABVphys_quat@@01M@Z
// @address 0x612c18 ??0accel_info@phys_keyframe_util@@QAA@XZ
// @address 0x612c88 ?set_from_position_orientation@KeyFrameInfo@phys_keyframe_util@@QAAXABVphys_vec4@@ABVphys_quat@@01M@Z
// @address 0x612e00 ?apply_soft_keyframe@phys_keyframe_util@@SAXABUKeyFrameInfo@1@AAUaccel_info@1@MMPAVphys_rigid_body@@@Z
// @address 0x6130c8 ?apply_hard_keyframe@phys_keyframe_util@@SAXABVphys_vec4@@ABVphys_quat@@MPAVphys_rigid_body@@@Z
#define PHYS_CONFIG_SIMD 1
#include "phys/phys_base.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_keyframe_util.h"

void phys_keyframe_util::KeyFrameInfo::fast_set_from_pos_orient(const phys_vec4 &cur_position,
    const phys_quat &cur_orientation, const phys_vec4 &keyframe_next_pos, const phys_quat &keyframe_next_orient, phys_real inv_delta_time)
{
    
    {
        m_position = cur_position;
        m_linear_velocity.set_sub4(keyframe_next_pos, cur_position);
        m_linear_velocity.set_mul4(inv_delta_time, m_linear_velocity);
    }

    {
        m_key_orientation = cur_orientation;
        phys_quat quat_diff;    quat_diff.set_mul_inverse(keyframe_next_orient, cur_orientation);
        m_ang_velocity.set_mul4(2.0f * inv_delta_time, quat_diff.get_imag());

        if ( quat_diff.get_real() < 0.0f )
        {
            m_ang_velocity.set_neg4( m_ang_velocity );
        }
    }    
}

phys_keyframe_util::accel_info::accel_info()
{
    m_lin_pos_factor.flags_set_all( 1.0f );
    m_ang_pos_factor.flags_set_all( 1.0f );
    m_lin_vel_factor.flags_set_all( 1.0f );
    m_ang_vel_factor.flags_set_all( 1.0f );
    m_max_lin_accel = 3.40282e+38f;
    m_max_ang_accel = 3.40282e+38f;
    m_max_allowed_dist = 3.40282e+38f;
}

void phys_keyframe_util::KeyFrameInfo::set_from_position_orientation(const phys_vec4 &cur_position,
    const phys_quat &cur_orientation, const phys_vec4 &keyframe_next_pos, const phys_quat &keyframe_next_orient, phys_real inv_delta_time)
{
    
    {
        m_position = cur_position;
        m_linear_velocity.set_sub4(keyframe_next_pos, cur_position);
        m_linear_velocity.set_mul4(inv_delta_time, m_linear_velocity);
    }

    {
        m_key_orientation = cur_orientation;
        phys_quat quat_diff;    quat_diff.set_mul_inverse(keyframe_next_orient, cur_orientation);

        quat_diff.normalize();

        if ( phys_math::fabs(quat_diff.get_real()) > 1.0f - 1.1920929e-7f )
        {
            m_ang_velocity.set_zero4();

        }
        else
        {
            phys_real angle = quat_diff.get_rotation_angle();
            quat_diff.get_rotation_axis(m_ang_velocity);
            m_ang_velocity.set_mul4(angle * inv_delta_time, m_ang_velocity);        
        }    
    }
}

void PHYS_CALL phys_keyframe_util::apply_soft_keyframe(
    const KeyFrameInfo &keyframe_info, accel_info &keyframe_accel_info, phys_real body_delta_time, phys_real inv_delta_time, phys_rigid_body *body)
{


    phys_vec4 delta_time_vec; delta_time_vec.flags_set_all( body_delta_time );

    phys_vec4 delta_position;    delta_position.set_sub4(    keyframe_info.m_position, body->get_contact_position() );
    
    if ( delta_position.length_sq_3d() > keyframe_accel_info.m_max_allowed_dist * keyframe_accel_info.m_max_allowed_dist )
    {
        delta_position.set_zero4();
        body->set_position_and_rotation( keyframe_info.m_position, keyframe_info.m_key_orientation );
        body->set_angular_velocity( keyframe_info.m_ang_velocity );
        body->set_linear_velocity( keyframe_info.m_linear_velocity );
    }

    phys_quat quat_diff;        quat_diff.set_mul_inverse(keyframe_info.m_key_orientation, body->get_rotation());

    phys_vec4 delta_orientation; delta_orientation.set_mul4( 2.0f , quat_diff.get_imag() );
    if ( quat_diff.get_real() < 0 )
    {
        delta_orientation.set_neg4( delta_orientation );
    }

    phys_vec4 scaled_delta_position;    scaled_delta_position.set_mul4( keyframe_accel_info.m_lin_pos_factor, delta_position );
    phys_vec4 scaled_delta_orientation; scaled_delta_orientation.set_mul4( keyframe_accel_info.m_ang_pos_factor, delta_orientation );

    phys_vec4 lin_vel_factor;  lin_vel_factor.set_mul4( delta_time_vec, keyframe_accel_info.m_lin_vel_factor );
    phys_vec4 ang_vel_factor;  ang_vel_factor.set_mul4( delta_time_vec, keyframe_accel_info.m_ang_vel_factor );

    phys_vec4 delta_lin_vel; delta_lin_vel.set_sub4( keyframe_info.m_linear_velocity, body->get_linear_velocity() );
    phys_vec4 delta_ang_vel; delta_ang_vel.set_sub4( keyframe_info.m_ang_velocity, body->get_angular_velocity() );

    delta_lin_vel.mul4( lin_vel_factor );
    delta_ang_vel.mul4( ang_vel_factor );

    delta_lin_vel.add4( scaled_delta_position );
    delta_ang_vel.add4( scaled_delta_orientation );

    {
        
        phys_real max_lin_delta = body_delta_time * keyframe_accel_info.m_max_lin_accel;
        phys_real delta_lin_vel_len_sq = delta_lin_vel.length_sq_3d();

        if ( delta_lin_vel_len_sq > max_lin_delta*max_lin_delta )
        {
            phys_real f = max_lin_delta * (1.0f / __fsqrts(delta_lin_vel_len_sq));
            delta_lin_vel.mul4( f );
        }

        phys_real max_ang_delta = body_delta_time * keyframe_accel_info.m_max_ang_accel;
        phys_real delta_ang_vel_len_sq = delta_ang_vel.length_sq_3d();

        if ( delta_ang_vel_len_sq > max_ang_delta*max_ang_delta )
        {
            phys_real f = max_ang_delta * (1.0f / __fsqrts(delta_ang_vel_len_sq));
            delta_ang_vel.mul4( f );
        }
    }
        
    {
        phys_vec4 new_ang_vel; new_ang_vel.set_add4( body->get_angular_velocity(), delta_ang_vel );
        phys_vec4 new_lin_vel; new_lin_vel.set_add4( body->get_linear_velocity(),  delta_lin_vel );

        body->set_angular_velocity( new_ang_vel );
        body->set_linear_velocity( new_lin_vel );
    }
}

void PHYS_CALL phys_keyframe_util::apply_hard_keyframe( const phys_vec4& keyframe_next_pos, const phys_quat& keyframe_next_orient,
    phys_real inv_delta_time, phys_rigid_body* body)
{
    
    {
        phys_vec4 keyframe_lin_vel;

        phys_vec4 new_center_of_mass;
        new_center_of_mass.vec_set_rotated_dir( keyframe_next_orient, body->get_center_of_mass_local() );
        new_center_of_mass.add4( keyframe_next_pos );
        keyframe_lin_vel.set_sub4( new_center_of_mass, body->get_center_of_mass_in_world() );

        keyframe_lin_vel.set_mul4(inv_delta_time, keyframe_lin_vel);
        
        body->set_linear_velocity(keyframe_lin_vel);
    }

    {
        phys_vec4 ang_velocity;
        phys_quat quat_diff;
        quat_diff.set_mul_inverse(keyframe_next_orient, body->get_rotation());
        quat_diff.normalize();

        phys_real angle = quat_diff.get_rotation_angle_inline();
        if(angle < 1e-3f)
        {
            ang_velocity.set_zero4();
        }
        else
        {
            quat_diff.get_rotation_axis(ang_velocity);
            ang_velocity.set_mul4(angle * inv_delta_time, ang_velocity);        
        }    
        body->set_angular_velocity(ang_velocity);
    }
}

