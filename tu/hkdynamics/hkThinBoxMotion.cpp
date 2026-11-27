// hkdynamics/hkThinBoxMotion.obj: from the records 0x65dbb0 0x65dbc8 0x65dc10 0x65dc30
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x65dbb0 ?destroy_loaded_thin_box_motion@@YAXPAX@Z
// @address 0x65dbc8 ??0phys_thin_box_motion@@QAA@ABVphys_vec4@@ABVphys_quat@@@Z
// @address 0x65dc10 ?fixup_loaded_thin_box_motion@@YAXPAX@Z
// @address 0x65dc30 ?get_thin_box_motion_vtable@@YAPBXXZ
#include "phys/phys_dynamics.h"

void fixup_loaded_thin_box_motion(void *p)
{
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 1;
    new (p) phys_thin_box_motion(flag);
}

void destroy_loaded_thin_box_motion(void *p)
{
    static_cast<phys_thin_box_motion *>(p)->~phys_thin_box_motion();
}

const void *get_thin_box_motion_vtable()
{
    union {
        __declspec(align(16)) void *ptr;
        char buf[sizeof(phys_thin_box_motion)];
    } u;
    phys_fixup_tag flag;
    new (u.buf) phys_thin_box_motion(flag);
    return u.ptr;
}

phys_thin_box_motion::phys_thin_box_motion(const phys_vec4 &position, const phys_quat &rotation)
    : phys_box_motion(position, rotation)
{
    m_type = BODY_MOTION_THIN_BOX_INERTIA;
}
