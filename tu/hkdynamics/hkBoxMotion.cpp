// hkdynamics/hkBoxMotion.obj: from the records 0x650320 0x650338 0x650358 0x6503c8 0x650418 0x650460 0x6504a0 0x6504d8 0x650558 0x6505d0 0x650608 0x650660 0x6506b0 0x6506d0 0x6506e0 0x6507b8
// @flags /TP /O2 /Oc
// @address 0x650338 ?set_body_mass@phys_box_motion@@UAAXM@Z
// @address 0x6503c8 ?get_inertia_local@phys_box_motion@@UBAXAAVphys_mat3@@@Z
// @address 0x650418 ?set_local_inertia@phys_box_motion@@UAAXABVphys_mat3@@@Z
// @address 0x650558 ?get_inertia_world@phys_box_motion@@UBAXAAVphys_mat3@@@Z
// @address 0x650460 ?get_inv_inertia_local@phys_box_motion@@UBAXAAVphys_mat3@@@Z
// @address 0x6504a0 ?set_local_inv_inertia@phys_box_motion@@UAAXABVphys_mat3@@@Z
// @address 0x6504d8 ?get_inv_inertia_world@phys_box_motion@@UBAXAAVphys_mat3@@@Z
// @address 0x650608 ?apply_force@phys_box_motion@@UAAXMABVphys_vec4@@0@Z
// @address 0x6506b0 ?fixup_loaded_box_motion@@YAXPAX@Z
// @address 0x6506e0 ?apply_point_impulse@phys_box_motion@@UAAXABVphys_vec4@@0@Z
// @address 0x6507b8 ?apply_ang_impulse@phys_box_motion@@UAAXABVphys_vec4@@@Z
// @address 0x650320 ?destroy_loaded_box_motion@@YAXPAX@Z
// @address 0x6505d0 ?apply_force@phys_box_motion@@UAAXMABVphys_vec4@@@Z
// @address 0x6506d0 ?get_box_motion_vtable@@YAPBXXZ
// @address 0x650660 ?apply_torque@phys_box_motion@@UAAXMABVphys_vec4@@@Z
#include <vectorintrinsics.h>
#include <ppcintrinsics.h>
#include <string.h>
#include "phys/phys_math.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_statistics_collector.h"


/* Havok math types: the layouts of tu/blamlibxbox/havok_component.cpp, with the members this unit uses */


void destroy_loaded_box_motion(void *object)
{
    static_cast<phys_box_motion *>(object)->~phys_box_motion();
}

void phys_box_motion::set_body_mass(float mass)
{
    float inv_mass = 1.0f / mass;
    set_inv_mass(inv_mass);
}

void phys_box_motion::get_inertia_local(phys_mat3 &inertia) const
{
    float x = 1.0f / m_inv_inertia_mass(0);
    float y = 1.0f / m_inv_inertia_mass(1);
    float z = 1.0f / m_inv_inertia_mass(2);
    inertia.set_diagonal(x, y, z);
}

void phys_box_motion::set_local_inertia(const phys_mat3 &inertia)
{
    float x = 1.0f / inertia(0, 0);
    float y = 1.0f / inertia(1, 1);
    float z = 1.0f / inertia(2, 2);
    m_inv_inertia_mass.set(x, y, z, m_inv_inertia_mass(3));
}

void phys_box_motion::get_inv_inertia_local(phys_mat3 &out) const
{
    float x = m_inv_inertia_mass(0);
    float y = m_inv_inertia_mass(1);
    float z = m_inv_inertia_mass(2);
    phys_vec4 zero;
    zero.set_zero4();
    out.m_column0 = zero;
    out.m_column1 = zero;
    out.m_column2 = zero;
    out(0, 0) = x;
    out(1, 1) = y;
    out(2, 2) = z;
}

void phys_box_motion::set_local_inv_inertia(const phys_mat3 &inertia)
{
    union { float f[4]; __vector4 q; } u;
    u.f[0] = inertia(0, 0);
    u.f[1] = inertia(1, 1);
    u.f[2] = inertia(2, 2);
    u.f[3] = m_inv_inertia_mass(3);
    m_inv_inertia_mass.m_simd_quad = u.q;
}

void phys_box_motion::get_inv_inertia_world(phys_mat3 &out) const
{
    phys_mat3 scaled;
    __vector4 inv = m_inv_inertia_mass.m_simd_quad;
    scaled.m_column0.m_simd_quad = __vmulfp(__vspltw(inv, 0), m_motion_state.m_body_transform.m_rotation.m_column0.m_simd_quad);
    scaled.m_column1.m_simd_quad = __vmulfp(__vspltw(inv, 1), m_motion_state.m_body_transform.m_rotation.m_column1.m_simd_quad);
    scaled.m_column2.m_simd_quad = __vmulfp(__vspltw(inv, 2), m_motion_state.m_body_transform.m_rotation.m_column2.m_simd_quad);
    out.set_mul_inverse(scaled, m_motion_state.m_body_transform.m_rotation);
}

void phys_box_motion::get_inertia_world(phys_mat3 &inertia) const
{
    float x = 1.0f / m_inv_inertia_mass(0);
    float y = 1.0f / m_inv_inertia_mass(1);
    float z = 1.0f / m_inv_inertia_mass(2);
    inertia.set_diagonal(x, y, z);
    inertia.change_basis(m_motion_state.m_body_transform.m_rotation);
}

void phys_box_motion::apply_force(float body_delta_time, const phys_vec4 &force)
{
    phys_simd_scalar scale(body_delta_time);
    phys_vec4 impulse;
    impulse.set_mul4(scale, force);
    m_linear_velocity.add_scaled4(get_inv_mass(), impulse);
}

void phys_box_motion::apply_force(float factor, const phys_vec4 &force, const phys_vec4 &point)
{
    phys_simd_scalar scale(factor);
    phys_vec4 scaled;
    scaled.set_mul4(scale, force);
    apply_point_impulse(scaled, point);
}

void fixup_loaded_box_motion(void *p)
{
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 1;
    new (p) phys_box_motion(flag);
}

const void *get_box_motion_vtable()
{
    __declspec(align(16)) char storage[sizeof(phys_box_motion)];
    phys_fixup_tag flag;
    new (storage) phys_box_motion(flag);
    const void *result;
    memcpy(&result, storage, sizeof(result));
    return result;
}

void phys_box_motion::apply_point_impulse(const phys_vec4 &impulse, const phys_vec4 &position)
{
    phys_simd_scalar inv_mass = get_inv_mass();
    m_linear_velocity.add_scaled4(inv_mass, impulse);

    phys_vec4 box_rel_pos;
    box_rel_pos.set_sub4(position, m_motion_state.m_swept_transform.m_center_of_mass1);

    phys_vec4 torque;
    torque.set_cross(box_rel_pos, impulse);

    phys_vec4 local_torque;
    local_torque.set_rotated_inverse_dir(m_motion_state.m_body_transform.m_rotation, torque);

    phys_vec4 local_ang_delta;
    local_ang_delta.set_mul4(m_inv_inertia_mass, local_torque);

    phys_vec4 ang_delta;
    ang_delta.set_rotated_dir(m_motion_state.m_body_transform.m_rotation, local_ang_delta);
    m_ang_velocity.add4(ang_delta);
}

void phys_box_motion::apply_ang_impulse(const phys_vec4 &impulse)
{
    phys_vec4 h;
    h.set_rotated_inverse_dir(m_motion_state.m_body_transform.m_rotation, impulse);
    h.m_simd_quad = __vmulfp(m_inv_inertia_mass.m_simd_quad, h.m_simd_quad);
    phys_vec4 w;
    w.set_rotated_dir(m_motion_state.m_body_transform.m_rotation, h);
    m_ang_velocity.add4(w);
}

void phys_box_motion::apply_torque(float body_delta_time, const phys_vec4 &torque)
{
    phys_vec4 scaled;
    scaled.set_mul4(phys_simd_scalar(body_delta_time), torque);
    apply_ang_impulse(scaled);
}

// @address 0x650358 ??0phys_box_motion@@QAA@ABVphys_vec4@@ABVphys_quat@@@Z

phys_box_motion::phys_box_motion(const phys_vec4 &position, const phys_quat &rotation)
    : phys_motion(position, rotation, false)
{
    m_inv_inertia_mass.set(1.0f, 1.0f, 1.0f, 1.0f);
    m_type = BODY_MOTION_BOX_INERTIA;
}
