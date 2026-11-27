// hkdynamics/hkPositionConstraintMotor.obj: from the records 0x64c690 0x64c6a8 0x64c6c8 0x64c6d8 0x64c740
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x64c690 ?destroy_loaded_position_motor@@YAXPAX@Z
// @address 0x64c6a8 ?fixup_loaded_position_motor@@YAXPAX@Z
// @address 0x64c6c8 ?get_position_motor_vtable@@YAPBXXZ
// @address 0x64c6d8 ??0phys_position_motor@@QAA@M@Z
// @address 0x64c740 ?clone@phys_position_motor@@UBAPAVphys_motor@@XZ
#include "phys/phys_dynamics.h"

phys_position_motor::phys_position_motor(float current_position)
    : phys_limited_force_motor()
{
    m_motor_kind = motor_kind_position;
    set_max_force(1e6f);
    m_tau_scale = 0.8f;
    m_motor_damping = 1.0f;
    m_const_recovery_vel = 1.0f;
    m_proportional_recovery_vel = 2.0f;
}

phys_motor *phys_position_motor::clone() const
{
    phys_position_motor *motor = new phys_position_motor(*this);
    return motor;
}

void fixup_loaded_position_motor(void *p)
{
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 1;
    new (p) phys_position_motor(flag);
}

void destroy_loaded_position_motor(void *p)
{
    static_cast<phys_position_motor *>(p)->~phys_position_motor();
}

const void *get_position_motor_vtable()
{
    union {
        __declspec(align(16)) void *ptr;
        char buf[sizeof(phys_position_motor)];
    } u;
    phys_fixup_tag flag;
    new (u.buf) phys_position_motor(flag);
    return u.ptr;
}
