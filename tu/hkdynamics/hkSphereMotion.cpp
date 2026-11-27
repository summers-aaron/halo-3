// hkdynamics/hkSphereMotion.obj: from the records 0x650008 0x650020 0x650060 0x6500a0 0x6500e8 0x650118 0x650150 0x650188 0x6501f0 0x650210 0x650248 0x6502a0 0x6502f0 0x650310
// @flags /TP /O2
// @address 0x650020 ?get_inertia_local@phys_sphere_motion@@UBAXAAVphys_mat3@@@Z
// @address 0x650060 ?get_inertia_world@phys_sphere_motion@@UBAXAAVphys_mat3@@@Z
// @address 0x6500a0 ?set_local_inertia@phys_sphere_motion@@UAAXABVphys_mat3@@@Z
// @address 0x6500e8 ?set_local_inv_inertia@phys_sphere_motion@@UAAXABVphys_mat3@@@Z
// @address 0x650118 ?get_inv_inertia_local@phys_sphere_motion@@UBAXAAVphys_mat3@@@Z
// @address 0x650150 ?get_inv_inertia_world@phys_sphere_motion@@UBAXAAVphys_mat3@@@Z
// @address 0x650188 ?apply_point_impulse@phys_sphere_motion@@UAAXABVphys_vec4@@0@Z
// @address 0x650008 ?destroy_loaded_sphere_motion@@YAXPAX@Z
// @address 0x6501f0 ?apply_ang_impulse@phys_sphere_motion@@UAAXABVphys_vec4@@@Z
// @address 0x650210 ?apply_force@phys_sphere_motion@@UAAXMABVphys_vec4@@@Z
// @address 0x650248 ?apply_force@phys_sphere_motion@@UAAXMABVphys_vec4@@0@Z
// @address 0x6502a0 ?apply_torque@phys_sphere_motion@@UAAXMABVphys_vec4@@@Z
// @address 0x6502f0 ?fixup_loaded_sphere_motion@@YAXPAX@Z
// @address 0x650310 ?get_sphere_motion_vtable@@YAPBXXZ
#include <vectorintrinsics.h>
#include <ppcintrinsics.h>
#include "phys/phys_sphere_motion.h"
#include "phys/phys_math.h"
#include "phys/phys_dynamics.h"


// 0x650008
void destroy_loaded_sphere_motion(void *memory)
{
    static_cast<phys_sphere_motion *>(memory)->~phys_sphere_motion();
}

// 0x650020
void phys_sphere_motion::get_inertia_local(phys_mat3 &inertia) const
{
    float inertia_value = 1.0f / m_inv_inertia_mass(0);
    inertia.set_diagonal(inertia_value, inertia_value, inertia_value);
}

// 0x650060
void phys_sphere_motion::get_inertia_world(phys_mat3 &inertia) const
{
    float inertia_value = 1.0f / m_inv_inertia_mass(0);
    inertia.set_diagonal(inertia_value, inertia_value, inertia_value);
}

// 0x6500a0
void phys_sphere_motion::set_local_inertia(const phys_mat3 &inertia)
{
    float inertia_value = phys_math::max_of_two(phys_math::max_of_two(inertia(0, 0), inertia(1, 1)), inertia(2, 2));
    float sphere_inv_inertia;
    if (inertia_value > 0.0f) {
        sphere_inv_inertia = 1.0f / inertia_value;
    } else {
        sphere_inv_inertia = 0.0f;
    }
    m_inv_inertia_mass(0) = sphere_inv_inertia;
    m_inv_inertia_mass(1) = sphere_inv_inertia;
    m_inv_inertia_mass(2) = sphere_inv_inertia;
}

// 0x6500e8
void phys_sphere_motion::set_local_inv_inertia(const phys_mat3 &inertia)
{
    float sphere_inv_inertia = phys_math::min_of_two(phys_math::min_of_two(inertia(0, 0), inertia(1, 1)), inertia(2, 2));
    m_inv_inertia_mass(0) = sphere_inv_inertia;
    m_inv_inertia_mass(1) = sphere_inv_inertia;
    m_inv_inertia_mass(2) = sphere_inv_inertia;
}

// 0x650118
void phys_sphere_motion::get_inv_inertia_local(phys_mat3 &inertia) const
{
    float sphere_inv_inertia = m_inv_inertia_mass(0);
    inertia.set_diagonal(sphere_inv_inertia, sphere_inv_inertia, sphere_inv_inertia);
}

// 0x650150
void phys_sphere_motion::get_inv_inertia_world(phys_mat3 &inertia) const
{
    float sphere_inv_inertia = m_inv_inertia_mass(0);
    inertia.set_diagonal(sphere_inv_inertia, sphere_inv_inertia, sphere_inv_inertia);
}

// 0x650188
void phys_sphere_motion::apply_point_impulse(const phys_vec4 &impulse, const phys_vec4 &point)
{
    m_linear_velocity.add_scaled4(get_inv_mass(), impulse);
    phys_vec4 arm;
    arm.set_sub4(point, get_center_of_mass_in_world());
    phys_vec4 ang_impulse;
    ang_impulse.set_cross(arm, impulse);
    m_ang_velocity.add_scaled4(m_inv_inertia_mass, ang_impulse);
}

// 0x6501f0
void phys_sphere_motion::apply_ang_impulse(const phys_vec4 &impulse)
{
    m_ang_velocity.add_scaled4(m_inv_inertia_mass, impulse);
}

// 0x650210
void phys_sphere_motion::apply_force(float body_delta_time, const phys_vec4 &force)
{
    phys_simd_scalar scale(body_delta_time);
    phys_vec4 scaled_force;
    scaled_force.set_mul4(scale, force);
    m_linear_velocity.add_scaled4(get_inv_mass(), scaled_force);
}

void phys_sphere_motion::apply_force(float delta_time,
                  const phys_vec4 &force, const phys_vec4 &point)
{
    __vector4 scalar = __vspltw(__lvlx(&delta_time, 0), 0);
    phys_vec4 scaled_impulse;
    scaled_impulse.m_simd_quad = __vmulfp(force.m_simd_quad, scalar);
    apply_point_impulse(scaled_impulse, point);
}

void fixup_loaded_sphere_motion(void *memory)
{
    phys_fixup_tag flag;
    new (memory) phys_sphere_motion(flag);
}

// 0x650310
const void *get_sphere_motion_vtable()
{
    __declspec(align(16)) char storage[sizeof(phys_sphere_motion)];
    phys_fixup_tag flag;
    phys_sphere_motion *object = new (storage) phys_sphere_motion(flag);
    return *reinterpret_cast<const void *const *>(object);
}

void phys_sphere_motion::apply_torque(float body_delta_time, const phys_vec4 &torque)
{
    phys_simd_scalar scale(body_delta_time);
    phys_vec4 impulse;
    impulse.set_mul4(scale, torque);
    apply_ang_impulse(impulse);
}

