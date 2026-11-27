// hkmath/hkSweptTransformUtil.obj: from the records 0x701b28 0x701b38 0x701bb8 0x701da8 0x701fa0 0x702180 0x7023b0 0x7025d0 0x702680 0x702758 0x7027e0 0x7027f0
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x701b28 ?set_time_info@phys_swept_transform_util@@YAXMMAAVphys_motion_state@@@Z
// @address 0x701b38 ?set_centre_of_rotation_local@phys_swept_transform_util@@YAXABVphys_vec4@@AAVphys_motion_state@@@Z
// @address 0x701bb8 ?lerp_swept@phys_swept_transform_util@@YAXABVphys_swept_transform@@MAAVphys_transform@@@Z
// @address 0x701da8 ?lerp_swept_add_on@phys_swept_transform_util@@YAXABVphys_swept_transform@@MMAAVphys_transform@@@Z
// @address 0x701fa0 ?lerp_swept_rel@phys_swept_transform_util@@YAXABVphys_swept_transform@@MAAVphys_transform@@@Z
// @address 0x702180 ?back_step_motion_state@phys_swept_transform_util@@YAXMAAVphys_motion_state@@@Z
// @address 0x7023b0 ?freeze_motion_state@phys_swept_transform_util@@YAXMAAVphys_motion_state@@@Z
// @address 0x7025d0 ?warp_to@phys_swept_transform_util@@YAXABVphys_vec4@@ABVphys_quat@@AAVphys_motion_state@@@Z
// @address 0x702680 ?warp_to@phys_swept_transform_util@@YAXABVphys_transform@@AAVphys_motion_state@@@Z
// @address 0x702758 ?warp_to_position@phys_swept_transform_util@@YAXABVphys_vec4@@AAVphys_motion_state@@@Z
// @address 0x7027e0 ?warp_to_rotation@phys_swept_transform_util@@YAXABVphys_quat@@AAVphys_motion_state@@@Z
// @address 0x7027f0 ?keyframe_motion_state@phys_swept_transform_util@@YAXABVphys_step_info@@ABVphys_vec4@@ABVphys_quat@@AAVphys_motion_state@@@Z
#define PHYS_CONFIG_SIMD 1


#include "phys/phys_base.h"
#include <float.h>


#define PHYS_MATH_SWEPT_TRANSFORM_UTIL_H 





#include "phys/phys_dynamics.h"
#include "phys/phys_swept_transform_util.h"

 void phys_swept_transform_util::lerp_swept( const phys_swept_transform& swept_trans, float t, phys_transform& xform_out )
 {
    phys_real r = swept_trans.get_interp_value( t );
    lerp_swept_rotation(swept_trans, r, xform_out);
 }

 void phys_swept_transform_util::lerp_swept_add_on( const phys_swept_transform& swept_trans, float t, phys_real t_add_on, phys_transform& xform_out )
 {
    phys_real r = swept_trans.get_interp_value_precise( t, t_add_on );
    lerp_swept_rotation(swept_trans, r, xform_out);
 }

 void phys_swept_transform_util::lerp_swept_rel( const phys_swept_transform& swept_trans, phys_real r, phys_transform& xform_out )
 {
    lerp_swept_rotation(swept_trans, r, xform_out);
 }

 void phys_swept_transform_util::back_step_motion_state( float time, phys_motion_state& motion_state )
 {
    phys_swept_transform& st = motion_state.get_swept_transform();
    phys_real t = phys_math::max_of_two( st.get_interp_value( time ), FLT_EPSILON);

    lerp_swept_rotation( st, t, st.m_rotation_1 );
    phys_real new_inv_dt = st.get_inv_delta_time() / t;

    st.m_center_of_mass1.set_lerp4( st.m_center_of_mass0, st.m_center_of_mass1, t );
    st.m_center_of_mass1(3) = new_inv_dt;
    motion_state.m_delta_angle.mul4( t );

    compute_transform_at_t1( st, motion_state.get_transform());
 }

 void phys_swept_transform_util::freeze_motion_state( float time, phys_motion_state& motion_state )
 {
    phys_swept_transform& st = motion_state.get_swept_transform();

    time = phys_math::max_of_two(time, st.get_base_time());
    phys_real t = st.get_interp_value( time );

    lerp_swept_rotation( st, t, st.m_rotation_1 );
    st.m_rotation_0 = st.m_rotation_1;

    st.m_center_of_mass1.set_lerp4( st.m_center_of_mass0, st.m_center_of_mass1, t );
    st.m_center_of_mass0 = st.m_center_of_mass1;

    st.m_center_of_mass0(3) = time;
    st.m_center_of_mass1(3) = 0.0f;

    compute_transform_at_t1( st, motion_state.get_transform());
 }

 void phys_swept_transform_util::set_time_info( float step_start_time, phys_real inv_delta_time, phys_motion_state& motion_state)
 {
    motion_state.get_swept_transform().m_center_of_mass0(3) = step_start_time;
    motion_state.get_swept_transform().m_center_of_mass1(3) = inv_delta_time;
 }

 void phys_swept_transform_util::warp_to( const phys_vec4& position, const phys_quat& rotation, phys_motion_state& ms )
 {
    phys_swept_transform& swept_transform = ms.get_swept_transform();
    ms.m_delta_angle.set_zero4();

    swept_transform.m_rotation_0 = rotation;
    swept_transform.m_rotation_1 = rotation;

    ms.get_transform().set_rotation( rotation );
    ms.get_transform().set_translation( position );
    
    phys_vec4 center_shift;
    center_shift.set_rotated_dir( ms.get_transform().get_rotation(), swept_transform.m_center_of_mass_local );

    phys_real swept_base_t = swept_transform.m_center_of_mass0(3);

    swept_transform.m_center_of_mass0.set_add4( position, center_shift );
    swept_transform.m_center_of_mass1 = swept_transform.m_center_of_mass0;

    swept_transform.m_center_of_mass0(3) = swept_base_t;	
    swept_transform.m_center_of_mass1(3) = 0;	
 }

 void phys_swept_transform_util::warp_to( const phys_transform& transform, phys_motion_state& ms )
 {
    phys_swept_transform& swept_transform = ms.get_swept_transform();
    ms.m_delta_angle.set_zero4();

    phys_quat rotation; rotation.set( transform.get_rotation() );
    ms.get_transform() = transform;

    swept_transform.m_rotation_0 = rotation;
    swept_transform.m_rotation_1 = rotation;
    
    phys_vec4 center_shift;
    center_shift.set_rotated_dir( transform.get_rotation(), swept_transform.m_center_of_mass_local );

    phys_real swept_base_t = swept_transform.m_center_of_mass0(3);

    swept_transform.m_center_of_mass0.set_add4( transform.get_translation(), center_shift );
    swept_transform.m_center_of_mass1 = swept_transform.m_center_of_mass0;

    swept_transform.m_center_of_mass0(3) = swept_base_t;	
    swept_transform.m_center_of_mass1(3) = 0;	
 }

 void phys_swept_transform_util::warp_to_position( const phys_vec4& position, phys_motion_state& ms )
 {
    const phys_rotation& cur_rotation = ms.get_transform().get_rotation();
    phys_swept_transform& swept_transform = ms.get_swept_transform();

    ms.m_delta_angle.set_zero4();
    ms.get_transform().set_translation( position );

    phys_vec4 center_shift;
    center_shift.set_rotated_dir( cur_rotation, swept_transform.m_center_of_mass_local );

    phys_real swept_base_t = swept_transform.m_center_of_mass0(3);

    swept_transform.m_center_of_mass0.set_add4( position, center_shift );
    swept_transform.m_center_of_mass1 = swept_transform.m_center_of_mass0;

    swept_transform.m_rotation_0 = swept_transform.m_rotation_1;

    swept_transform.m_center_of_mass0(3) = swept_base_t;	
    swept_transform.m_center_of_mass1(3) = 0.0f; 
 }

 void phys_swept_transform_util::warp_to_rotation( const phys_quat& rotation, phys_motion_state& ms )
 {
    warp_to( ms.get_transform().get_translation(), rotation, ms );
 }

 void phys_swept_transform_util::keyframe_motion_state( const phys_step_info& step_info, const phys_vec4& target_pos,
     const phys_quat& swept_end_rot, phys_motion_state& ms )
 {
    phys_swept_transform& swept_transform = ms.get_swept_transform();

    swept_transform.m_center_of_mass0 = swept_transform.m_center_of_mass1;
    swept_transform.m_rotation_0 = swept_transform.m_rotation_1;

    swept_transform.m_center_of_mass1 = target_pos;
    swept_transform.m_rotation_1 = swept_end_rot;

    phys_quat diff; diff.set_mul_inverse( swept_end_rot, swept_transform.m_rotation_0 );

    phys_real angle =  diff.get_rotation_angle_inline();
    phys_vec4 axis;
    if ( angle )
    {
        diff.get_rotation_axis(axis);
    }
    else
    {
        axis.set_zero4();
    }

    ms.m_delta_angle.set_mul4( angle, axis );
    ms.m_delta_angle(3) = angle;
    
    swept_transform.m_center_of_mass0(3) = step_info.m_step_start_time;	
    swept_transform.m_center_of_mass1(3) = step_info.m_inv_dt;

    compute_transform_at_t1( swept_transform, ms.get_transform() );
 }

 void phys_swept_transform_util::set_centre_of_rotation_local( const phys_vec4& new_rotation_center, phys_motion_state& motion_state)
 {
    phys_vec4 offset; offset.set_sub4(new_rotation_center, motion_state.get_swept_transform().m_center_of_mass_local);
    motion_state.get_swept_transform().m_center_of_mass_local = new_rotation_center;
    
    phys_vec4 offset_world; offset_world.vec_set_rotated_dir(motion_state.get_transform().get_rotation(), offset);
    offset_world.zero_element(3);
    motion_state.get_swept_transform().m_center_of_mass0.add3_clobber_w(offset_world);
    motion_state.get_swept_transform().m_center_of_mass1.add3_clobber_w(offset_world);
 }

