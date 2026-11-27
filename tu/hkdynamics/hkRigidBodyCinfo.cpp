// hkdynamics/hkRigidBodyCinfo.obj: from the records 0x648ec8
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x648ec8 ??0phys_rigid_body_desc@@QAA@XZ
#include "phys/phys_dynamics.h"

// @rdata 0x82014d60
const __vector4 phys_simd_vec_0001 = { 0.0f, 0.0f, 0.0f, 1.0f };

phys_rigid_body_desc::phys_rigid_body_desc()
{
    m_position.set_zero4();
    m_rotation.set_to_identity();
    m_linear_velocity.set_zero4();
    m_ang_velocity.set_zero4();
    m_inertia_tensor.set_diagonal(1.0f, 1.0f, 1.0f);
    m_center_of_mass.set_zero4();
    m_body_mass = 1.0f;
    m_linear_damping = 0.0f;
    m_ang_damping = 0.05f;
    m_contact_friction = 0.5f;
    m_contact_restitution = 0.4f;
    m_max_lin_vel = 200.0f;
    m_max_ang_vel = 200.0f;
    m_allowed_penetration_depth = -1.0f;
    m_motion_type = phys_motion::BODY_MOTION_DYNAMIC;
    m_deactivator_kind = phys_rigid_body_deactivator::sleep_policy_spatial;
    m_solver_sleep = SOLVER_SLEEP_LOW;
    m_collider_quality = PHYS_COLLIDER_QUALITY_INVALID;
    m_collision_response = phys_material::COLLISION_RESPONSE_SIMPLE_CONTACT;
    m_contact_callback_delay = 0xffff;
    m_collision_filter_info = 0;
    m_proxy_shape = 0;
    m_auto_remove_level = 0;
    m_contact_user_data_count = 0;
}
