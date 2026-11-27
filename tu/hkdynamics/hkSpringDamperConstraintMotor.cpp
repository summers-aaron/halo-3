// hkdynamics/hkSpringDamperConstraintMotor.obj: from the records 0x64c518 0x64c530 0x64c550 0x64c560 0x64c5b0 0x64c5f0 0x64d650
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x64c518 ?destroy_loaded_spring_damper_motor@@YAXPAX@Z
// @address 0x64c530 ?fixup_loaded_spring_damper_motor@@YAXPAX@Z
// @address 0x64c550 ?get_spring_damper_motor_vtable@@YAPBXXZ
// @address 0x64c560 ??0phys_spring_damper_motor@@QAA@XZ
// @address 0x64c5b0 ??0phys_spring_damper_motor@@QAA@MMM@Z
// @address 0x64c5f0 ?clone@phys_spring_damper_motor@@UBAPAVphys_motor@@XZ
// @address 0x64d650 ??_Gphys_motor@@UAAPAXI@Z
#include "phys/phys_dynamics.h"

phys_spring_damper_motor::phys_spring_damper_motor()
    : phys_limited_force_motor()
{
    m_motor_kind = motor_kind_spring_damper;
    set_max_force(1e6f);
    m_spring_stiffness = 0.0f;
    m_spring_damp = 0.0f;
}

phys_spring_damper_motor::phys_spring_damper_motor(float spring_constant, float spring_damping, float maximum_force)
    : phys_limited_force_motor()
{
    m_motor_kind = motor_kind_spring_damper;
    set_max_force(maximum_force);
    m_spring_stiffness = spring_constant;
    m_spring_damp = spring_damping;
}

phys_motor *phys_spring_damper_motor::clone() const
{
    phys_spring_damper_motor *motor = new phys_spring_damper_motor(*this);
    return motor;
}

void fixup_loaded_spring_damper_motor(void *p)
{
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 1;
    new (p) phys_spring_damper_motor(flag);
}

void destroy_loaded_spring_damper_motor(void *p)
{
    static_cast<phys_spring_damper_motor *>(p)->~phys_spring_damper_motor();
}

const void *get_spring_damper_motor_vtable()
{
    union {
        __declspec(align(16)) void *ptr;
        char buf[sizeof(phys_spring_damper_motor)];
    } u;
    phys_fixup_tag flag;
    new (u.buf) phys_spring_damper_motor(flag);
    return u.ptr;
}
