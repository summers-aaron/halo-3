// hkdynamics/hkKeyframedRigidMotion.obj: from the records 0x64fcb8 0x64fcd0 0x64fce8 0x64fcf0 0x64fcf8 0x64fd00 0x64fd08 0x64fd10 0x64fd18 0x64fd20 0x64fd28 0x64fd30 0x64fd38 0x64fd40 0x64fdf0 0x64fe58 0x64ff08 0x64ff30 0x64ff58 0x64ff80 0x64ffa8 0x64ffc8 0x64ffd8 0x64fff8
// @flags /TP /O2
// @address 0x64ff08 ?get_inertia_local@phys_keyframed_motion@@UBAXAAVphys_mat3@@@Z
// @address 0x64ff30 ?get_inertia_world@phys_keyframed_motion@@UBAXAAVphys_mat3@@@Z
// @address 0x64ff58 ?get_inv_inertia_local@phys_keyframed_motion@@UBAXAAVphys_mat3@@@Z
// @address 0x64ff80 ?get_inv_inertia_world@phys_keyframed_motion@@UBAXAAVphys_mat3@@@Z
// @address 0x64ffa8 ?fixup_loaded_keyframed_motion@@YAXPAX@Z
// @address 0x64ffd8 ?fixup_loaded_max_size_motion@@YAXPAX@Z
// @address 0x64fd40 ?set_stored_motion@phys_keyframed_motion@@UAAXPAVphys_max_size_motion@@@Z
// @address 0x64ffc8 ?get_keyframed_motion_vtable@@YAPBXXZ
// @address 0x64fff8 ?get_max_size_motion_vtable@@YAPBXXZ
#include <vectorintrinsics.h>
#include "physics/havok_halo_external.h"
#include "phys/phys_dynamics.h"


// @address 0x64fce8 ?set_body_mass@phys_keyframed_motion@@UAAXM@Z
void phys_keyframed_motion::set_body_mass(float m)
{
}

// @address 0x64fcf0 ?set_inv_mass@phys_keyframed_motion@@UAAXM@Z
void phys_keyframed_motion::set_inv_mass(float inv_mass_arg)
{
}

// @address 0x64fcf8 ?set_local_inertia@phys_keyframed_motion@@UAAXABVphys_mat3@@@Z
void phys_keyframed_motion::set_local_inertia(const phys_mat3 &inertia)
{
}

// @address 0x64fd00 ?set_local_inv_inertia@phys_keyframed_motion@@UAAXABVphys_mat3@@@Z
void phys_keyframed_motion::set_local_inv_inertia(const phys_mat3 &inv_inertia)
{
}

// @address 0x64fd08 ?apply_lin_impulse@phys_keyframed_motion@@UAAXABVphys_vec4@@@Z
void phys_keyframed_motion::apply_lin_impulse(const phys_vec4 &body_linear_impulse)
{
}

// @address 0x64fd10 ?apply_point_impulse@phys_keyframed_motion@@UAAXABVphys_vec4@@0@Z
void phys_keyframed_motion::apply_point_impulse(const phys_vec4 &body_linear_impulse, const phys_vec4 &p)
{
}

// @address 0x64fd18 ?apply_ang_impulse@phys_keyframed_motion@@UAAXABVphys_vec4@@@Z
void phys_keyframed_motion::apply_ang_impulse(const phys_vec4 &body_linear_impulse)
{
}

// @address 0x64fd20 ?apply_force@phys_keyframed_motion@@UAAXMABVphys_vec4@@@Z
void phys_keyframed_motion::apply_force(const float body_delta_time, const phys_vec4 &force)
{
}

// @address 0x64fd28 ?apply_force@phys_keyframed_motion@@UAAXMABVphys_vec4@@0@Z
void phys_keyframed_motion::apply_force(const float body_delta_time, const phys_vec4 &force, const phys_vec4 &p)
{
}

// @address 0x64fd30 ?apply_torque@phys_keyframed_motion@@UAAXMABVphys_vec4@@@Z
void phys_keyframed_motion::apply_torque(const float body_delta_time, const phys_vec4 &torque)
{
}

// @address 0x64fd38 ?set_step_position@phys_keyframed_motion@@UAAXMM@Z
void phys_keyframed_motion::set_step_position(float position, float timestep)
{
}

// 0x64fd40
void phys_keyframed_motion::set_stored_motion(phys_max_size_motion *motion)
{
    if (motion) {
        motion->add_object_reference();
    }
    if (m_saved_motion) {
        m_saved_motion->object_remove_ref();
    }
    m_saved_motion = motion;
}

// @address 0x64fdf0 ??0phys_keyframed_motion@@QAA@ABVphys_vec4@@ABVphys_quat@@@Z
phys_keyframed_motion::phys_keyframed_motion(const phys_vec4 &position, const phys_quat &rotation)
    : phys_motion(position, rotation, false)
{
    m_saved_motion = 0;
    m_saved_quality_idx = 0;
    m_inv_inertia_mass.set_zero4();
    m_type = MOTION_KEYFRAMED;
}
// @address 0x64fe58 ??1phys_keyframed_motion@@UAA@XZ
phys_keyframed_motion::~phys_keyframed_motion()
{
    if (m_saved_motion) {
        m_saved_motion->object_remove_ref();
    }
}

void phys_keyframed_motion::get_inertia_local(phys_mat3 &inertia) const
{
    __vector4 zero;
#if defined(_XBOX) && !defined(__clang__)
    zero = __vxor(zero, zero);
#else
    zero = __vzero();
#endif
    inertia.m_column0.m_simd_quad = zero;
    inertia.m_column1.m_simd_quad = zero;
    inertia.m_column2.m_simd_quad = zero;
}

void phys_keyframed_motion::get_inertia_world(phys_mat3 &inertia) const
{
    __vector4 zero;
#if defined(_XBOX) && !defined(__clang__)
    zero = __vxor(zero, zero);
#else
    zero = __vzero();
#endif
    inertia.m_column0.m_simd_quad = zero;
    inertia.m_column1.m_simd_quad = zero;
    inertia.m_column2.m_simd_quad = zero;
}

void phys_keyframed_motion::get_inv_inertia_local(phys_mat3 &inertia) const
{
    __vector4 zero;
#if defined(_XBOX) && !defined(__clang__)
    zero = __vxor(zero, zero);
#else
    zero = __vzero();
#endif
    inertia.m_column0.m_simd_quad = zero;
    inertia.m_column1.m_simd_quad = zero;
    inertia.m_column2.m_simd_quad = zero;
}

void phys_keyframed_motion::get_inv_inertia_world(phys_mat3 &inertia) const
{
    __vector4 zero;
#if defined(_XBOX) && !defined(__clang__)
    zero = __vxor(zero, zero);
#else
    zero = __vzero();
#endif
    inertia.m_column0.m_simd_quad = zero;
    inertia.m_column1.m_simd_quad = zero;
    inertia.m_column2.m_simd_quad = zero;
}

void fixup_loaded_keyframed_motion(void *p)
{
    phys_fixup_tag flag;
    new (p) phys_keyframed_motion(flag);
}

// 0x64ffc8
const void *get_keyframed_motion_vtable()
{
    __declspec(align(16)) char storage[sizeof(phys_keyframed_motion)];
    phys_fixup_tag flag;
    phys_keyframed_motion *object = new (storage) phys_keyframed_motion(flag);
    return *reinterpret_cast<const void *const *>(object);
}

void fixup_loaded_max_size_motion(void *p)
{
    phys_fixup_tag flag;
    new (p) phys_max_size_motion(flag);
}

// 0x64fff8
const void *get_max_size_motion_vtable()
{
    __declspec(align(16)) char storage[sizeof(phys_max_size_motion)];
    phys_fixup_tag flag;
    phys_max_size_motion *object = new (storage) phys_max_size_motion(flag);
    return *reinterpret_cast<const void *const *>(object);
}

// @address 0x64fcb8 ?destroy_loaded_keyframed_motion@@YAXPAX@Z
void destroy_loaded_keyframed_motion(void *p)
{
    static_cast<phys_keyframed_motion *>(p)->~phys_keyframed_motion();
}

// @address 0x64fcd0 ?destroy_loaded_max_size_motion@@YAXPAX@Z
void destroy_loaded_max_size_motion(void *p)
{
    static_cast<phys_max_size_motion *>(p)->~phys_max_size_motion();
}
