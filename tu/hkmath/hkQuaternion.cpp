// hkmath/hkQuaternion.obj: from the records 0x610ee0 0x610f68 0x611008 0x6111e8 0x6111f0 0x611250 0x6113a8 0x611488 0x611670
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x610ee0 ?set_axis_angle@phys_quat@@QAAXABVphys_vec4@@M@Z
// @address 0x610f68 ?is_stream_ok@phys_quat@@QBA?AVphys_bool@@XZ
// @address 0x611008 ?quat_from_rotation@@YAXAAU__vector4@@ABVphys_rotation@@@Z
// @address 0x6111e8 ?set@phys_quat@@QAAXABVphys_rotation@@@Z
// @address 0x6111f0 ?set_and_normalize@phys_quat@@QAAXABVphys_rotation@@@Z
// @address 0x611250 ?set_slerp@phys_quat@@QAAXABV1@0M@Z
// @address 0x6113a8 ?set_flipped_rotation@phys_quat@@IAAXABVphys_vec4@@@Z
// @address 0x611488 ?remove_axis_component@phys_quat@@QAAXABVphys_vec4@@@Z
// @address 0x611670 ?decompose_rest_axis@phys_quat@@QBAXABVphys_vec4@@AAV1@AAM@Z
#define PHYS_CONFIG_SIMD 1

#include "phys/phys_base.h"
#include <float.h>


#include "phys/phys_vec4_util.h"
#include "phys/phys_quat.h"
 static const phys_real PHYS_QUAT_DELTA = 1e-3f;

 void phys_quat::set_axis_angle(const phys_vec4& axis, phys_real angle)
 {
    phys_real quat_half_angle = 0.5f * angle;
    phys_simd_scalar sin_half = phys_math::sin(quat_half_angle);
    m_quat_vec.set_mul4(sin_half, axis);
    m_quat_vec(3) = phys_math::cos(quat_half_angle);
 }

 phys_bool phys_quat::is_stream_ok() const
 {
    phys_bool ok = m_quat_vec.is_ok4();
    phys_real length = m_quat_vec.length_sq_4d();
    phys_real error = length - 1.0f;
    phys_bool length_ok = (phys_math::fabs(error) < 1e-3f);
    ok = ok && length_ok;
    return ok;
 }

 void quat_from_rotation( phys_simd_vec& lerp_quat_out, const phys_rotation& r )
 {
    phys_real trace = r(0,0) + r(1,1) + r(2,2);
    union {
        float r[4];
        __vector4 q;
    } u;

    if( trace > 0 )
    {
        
        phys_real s = phys_math::sqrt( trace + 1.0f );
        phys_real t = 0.5f / s;
        u.r[0] = (r(2,1)-r(1,2)) * t;
        u.r[1] = (r(0,2)-r(2,0)) * t;
        u.r[2] = (r(1,0)-r(0,1)) * t;
        u.r[3] = 0.5f*s;
    }
    else
    {
        const int next[] = {1,2,0};
        int i=0;

        if(r(1,1) > r(0,0)) i=1;
        if(r(2,2) > r(i,i)) i=2;

        int j = next[i];
        int k = next[j];

        phys_real s = phys_math::sqrt(r(i,i) - (r(j,j)+r(k,k)) + 1.0f);
        phys_real t = 0.5f / s;

        u.r[i] = 0.5f * s;
        u.r[3] = (r(k,j)-r(j,k)) * t;
        u.r[j] = (r(j,i)+r(i,j)) * t;
        u.r[k] = (r(k,i)+r(i,k)) * t;
    }
    lerp_quat_out = u.q;
 }

 void phys_quat::set(const phys_rotation& r)
 {
    quat_from_rotation(m_quat_vec.get_quad(),r);
    
 }

 void phys_quat::set_and_normalize(const phys_rotation& r)
 {
    quat_from_rotation(m_quat_vec.get_quad(),r);
    normalize();
 }

 void phys_quat::set_slerp(const phys_quat& q0, const phys_quat& q1, phys_real t)
 {

    phys_real cos_theta = q0.m_quat_vec.dot4_simd(q1.m_quat_vec);

    phys_real t1_sign = 1.0f;
    if (cos_theta < 0.0f)
    {
        cos_theta = -cos_theta;
        t1_sign = -1.0f;
    }

    phys_vec4 slerp_vec;

    if (cos_theta < 1.0f - PHYS_QUAT_DELTA)
    {
        phys_real theta = phys_math::acos(cos_theta);
        
        phys_real inv_sin_theta = phys_math::inv_sqrt(1.0f - cos_theta*cos_theta);
        phys_real t_theta = t*theta;

        phys_real t0 = phys_math::sin(theta-t_theta) * inv_sin_theta;
        phys_real t1 = phys_math::sin(t_theta)       * inv_sin_theta;

        slerp_vec.set_mul4( t0,			q0.m_quat_vec);
        slerp_vec.add_scaled4( t1*t1_sign,	q1.m_quat_vec);
    }
    else
    {
        
        phys_real t0 = 1.0f - t;
        phys_real t1 = t;

        slerp_vec.set_mul4( t0,          q0.m_quat_vec);
        slerp_vec.add_scaled4( t1*t1_sign, q1.m_quat_vec);
    }	
    slerp_vec.normalize_xyzw();
    m_quat_vec = slerp_vec;
 }

 void phys_quat::remove_axis_component (const phys_vec4& axis)
 {
    
    phys_vec4 rotated_axis;
    rotated_axis.vec_set_rotated_dir(*this, axis);

    const phys_real axis_dot = axis.dot3(rotated_axis);

    if ( (axis_dot-1.0f) > -1e-3f )
    {
        set_to_identity();
        return;
    }

    if ( (axis_dot+1.0f) < 1e-3f )
    {
        phys_vec4 perp_vector;
        phys_vec4_util::compute_perpendicular_vec(axis, perp_vector);
        this->set_axis_angle(perp_vector, 3.14159265358979f);
        return;

    }

    {
        const phys_real rot_angle = phys_math::acos(axis_dot);

        phys_vec4 rot_axis;
        rot_axis.set_cross(axis, rotated_axis);
        rot_axis.normalize_xyz();

        set_axis_angle(rot_axis, rot_angle);

    }
 }

 void phys_quat::decompose_rest_axis(const phys_vec4& axis, phys_quat& rest_out, phys_real& angle_out) const
 {
    phys_quat axis_rot;
    {
        rest_out = *this;
        rest_out.remove_axis_component(axis);

        phys_quat rest_inverse;
        rest_inverse.set_to_inverse(rest_out);
        axis_rot.set_mul(rest_inverse, *this);
    }

    angle_out = axis_rot.get_rotation_angle_inline();

    const phys_bool reverse = ( phys_simd_scalar(axis_rot.get_real()) * (axis_rot.get_imag().dot3(axis))) < phys_simd_scalar(0.0f);

    if (reverse)
    {

        angle_out = -angle_out;
    }
 }

 void phys_quat::set_flipped_rotation(const phys_vec4& from)
 {
    phys_vec4 vec;
    phys_vec4_util::compute_perpendicular_vec(from, vec);
    vec.normalize_xyz();
    vec(3) = 0.0f;
    m_quat_vec = vec;
 }



const phys_quat phys_quat_identity = phys_quat(0.0f, 0.0f, 0.0f, 1.0f);
