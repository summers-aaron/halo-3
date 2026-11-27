// hkdynamics/hkMotion.obj: from the records 0x64c060 0x64c098 0x64c0a8 0x64c0b8 0x64c0c8 0x64c0e0 0x64c0f0 0x64c0f8 0x64c130 0x64c138 0x64c140 0x64c150 0x64c160 0x64c188 0x64c260
// @flags /TP /O2
// @address 0x64c060 ?set_body_mass@phys_motion@@UAAXM@Z
// @address 0x64c0f8 ?get_mass@phys_motion@@QBAMXZ
// @address 0x64c188 ??0phys_motion@@QAA@ABVphys_vec4@@ABVphys_quat@@_N@Z
// @address 0x64c098 ?set_center_of_mass_in_local@phys_motion@@UAAXABVphys_vec4@@@Z
// @address 0x64c0a8 ?contact_set_position@phys_motion@@UAAXABVphys_vec4@@@Z
// @address 0x64c0b8 ?set_rotation@phys_motion@@UAAXABVphys_quat@@@Z
// @address 0x64c0c8 ?set_position_and_rotation@phys_motion@@UAAXABVphys_vec4@@ABVphys_quat@@@Z
// @address 0x64c0e0 ?set_transform@phys_motion@@UAAXABVphys_transform@@@Z
// @address 0x64c0f0 ?set_deactivation_class@phys_motion@@QAAXH@Z
// @address 0x64c130 ?set_inv_mass@phys_motion@@UAAXM@Z
// @address 0x64c138 ?approx_body_transform_at@phys_motion@@QAAXMAAVphys_transform@@@Z
// @address 0x64c140 ?set_linear_velocity@phys_motion@@UAAXABVphys_vec4@@@Z
// @address 0x64c150 ?set_angular_velocity@phys_motion@@UAAXABVphys_vec4@@@Z
// @address 0x64c160 ?apply_lin_impulse@phys_motion@@UAAXABVphys_vec4@@@Z
// @address 0x64c260 ?get_motion_snapshot@phys_motion@@UAAXPAV1@@Z
#include <vectorintrinsics.h>
#include "phys/phys_math.h"
#include "phys/phys_dynamics.h"

#include "phys/phys_swept_transform_util.h"
#include "phys/phys_statistics_collector.h"

// 0x64c060
void phys_motion::set_body_mass(float mass)
{
    set_inv_mass(mass == 0.0f ? 0.0f : 1.0f / mass);
}

// 0x64c098: hkMotion::setCenterOfMassInLocal
void phys_motion::set_center_of_mass_in_local(const phys_vec4 &shape_centre)
{
    phys_swept_transform_util::set_centre_of_rotation_local(shape_centre, m_motion_state);
}

// 0x64c0a8: hkMotion::setPosition
void phys_motion::contact_set_position(const phys_vec4 &position)
{
    phys_swept_transform_util::warp_to_position(position, m_motion_state);
}

// 0x64c0b8: hkMotion::setRotation
void phys_motion::set_rotation(const phys_quat &rotation)
{
    phys_swept_transform_util::warp_to_rotation(rotation, m_motion_state);
}

// 0x64c0c8
void phys_motion::set_position_and_rotation(const phys_vec4 &position, const phys_quat &rotation)
{
    phys_swept_transform_util::warp_to(position, rotation, m_motion_state);
}

// 0x64c0e0: hkMotion::setTransform
void phys_motion::set_transform(const phys_transform &transform)
{
    phys_swept_transform_util::warp_to(transform, m_motion_state);
}

// 0x64c0f0
void phys_motion::set_deactivation_class(int body_sleep_class)
{
    m_motion_state.m_deactivation_class = (unsigned short)body_sleep_class;
}

// 0x64c0f8
float phys_motion::get_mass() const
{
    float inv_mass = get_inv_mass();
    if (inv_mass == 0.0f) return 0.0f;
    else return 1.0f / inv_mass;
}

// 0x64c130
void phys_motion::set_inv_mass(float inv_mass)
{
    m_inv_inertia_mass(3) = inv_mass;
}

// 0x64c138
void phys_motion::approx_body_transform_at(float time, phys_transform &transform)
{
    get_motion_state()->get_swept_transform().approx_body_transform_at(time, transform);
}

// 0x64c140
void phys_motion::set_linear_velocity(const phys_vec4 &velocity)
{
    m_linear_velocity = velocity;
}

// 0x64c150
void phys_motion::set_angular_velocity(const phys_vec4 &velocity)
{
    m_ang_velocity = velocity;
}

// 0x64c160
void phys_motion::apply_lin_impulse(const phys_vec4 &impulse)
{
    m_linear_velocity.add_scaled4(get_inv_mass(), impulse);
}

// 0x64c188
phys_motion::phys_motion(const phys_vec4 &position, const phys_quat &rotation, bool want_deactivation)
{
    m_linear_velocity.set_zero4();
    m_ang_velocity.set_zero4();
    m_motion_state.init_motion_state(position, rotation);
    m_motion_state.m_linear_damping = 0.0f;
    m_motion_state.m_ang_damping = 0.0f;
    m_type = BODY_MOTION_INVALID;
    if (want_deactivation)
        m_integrate_sleep_counter = (int)position(0) & 15;
    else
        m_integrate_sleep_counter = 255;
    m_inactive_frame_counts[0] = 0;
    m_inactive_frame_counts[1] = 0;
    m_sleep_ref_position[0].set_zero4();
    m_sleep_ref_position[1].set_zero4();
    m_motion_state.m_sleep_ref_orientation[0] = 0;
    m_motion_state.m_sleep_ref_orientation[1] = 0;
}

// 0x64c260
void phys_motion::get_motion_snapshot(phys_motion *motion)
{
    motion->m_motion_state = m_motion_state;
    motion->m_linear_velocity = m_linear_velocity;
    motion->m_ang_velocity = m_ang_velocity;
    motion->m_integrate_sleep_counter = m_integrate_sleep_counter;
}
