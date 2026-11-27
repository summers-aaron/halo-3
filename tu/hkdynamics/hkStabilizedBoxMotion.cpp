// hkdynamics/hkStabilizedBoxMotion.obj: from the records 0x65db20 0x65db38 0x65db80 0x65dba0
// @flags /TP /O2
#include "phys/phys_dynamics.h"
#include "phys/phys_type_desc.h"


// @address 0x65db20 ?destroy_loaded_stabilized_box_motion@@YAXPAX@Z
void destroy_loaded_stabilized_box_motion(void *memory)
{
    static_cast<phys_stabilized_box_motion *>(memory)->~phys_stabilized_box_motion();
}
// @address 0x65db80 ?fixup_loaded_stabilized_box_motion@@YAXPAX@Z
void fixup_loaded_stabilized_box_motion(void *memory)
{
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 1;
    new (memory) phys_stabilized_box_motion(flag);
}
// @address 0x65dba0 ?get_stabilized_box_motion_vtable@@YAPBXXZ
const void *get_stabilized_box_motion_vtable()
{
    return phys_type_desc::get_vtable<phys_stabilized_box_motion>();
}
// @address 0x65db38 ??0phys_stabilized_box_motion@@QAA@ABVphys_vec4@@ABVphys_quat@@@Z
phys_stabilized_box_motion::phys_stabilized_box_motion(const phys_vec4 &position, const phys_quat &rotation)
    : phys_box_motion(position, rotation)
{
    m_type = BODY_MOTION_STABLE_BOX_INERTIA;
}
