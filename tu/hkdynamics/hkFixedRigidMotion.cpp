// hkdynamics/hkFixedRigidMotion.obj: from the records 0x650838 0x650850 0x650858 0x6508a0 0x6508c0 0x6508d0
// @flags /TP /O2
// @address 0x650838 ?destroy_loaded_fixed_motion@@YAXPAX@Z
// @address 0x6508a0 ?fixup_loaded_fixed_motion@@YAXPAX@Z
// @address 0x6508c0 ?get_fixed_motion_vtable@@YAPBXXZ
// @address 0x650850 ?set_step_position@phys_fixed_motion@@UAAXMM@Z
// @address 0x650858 ??0phys_fixed_motion@@QAA@ABVphys_vec4@@ABVphys_quat@@@Z
// @address 0x6508d0 ?get_position_and_velocities@phys_fixed_motion@@UAAXPAVphys_motion@@@Z
#include <vectorintrinsics.h>
#include "phys/phys_dynamics.h"
#include "phys/phys_fixed_motion.h"


void destroy_loaded_fixed_motion(void *p0)
{
    static_cast<phys_fixed_motion *>(p0)->~phys_fixed_motion();
}

// hkFixedRigidMotion::setStepPosition
void phys_fixed_motion::set_step_position(float step_dt, float factor)
{
}

// hkFixedRigidMotion::hkFixedRigidMotion
phys_fixed_motion::phys_fixed_motion(const phys_vec4 &position, const phys_quat &rotation)
    : phys_keyframed_motion(position, rotation)
{
    m_type = MOTION_FIXED;
}

void fixup_loaded_fixed_motion(void *storage)
{
    phys_fixup_tag flag;
    new (storage) phys_fixed_motion(flag);
}

const void *get_fixed_motion_vtable()
{
    __declspec(align(16)) char storage[sizeof(phys_fixed_motion)];
    phys_fixup_tag flag;
    phys_fixed_motion *object = new (storage) phys_fixed_motion(flag);
    return *reinterpret_cast<const void *const *>(object);
}

// hkFixedRigidMotion::getPositionAndVelocities
void phys_fixed_motion::get_position_and_velocities(phys_motion *motion)
{
    motion->m_motion_state = m_motion_state;
    motion->m_linear_velocity.set_zero4();
    motion->m_ang_velocity.set_zero4();
}
