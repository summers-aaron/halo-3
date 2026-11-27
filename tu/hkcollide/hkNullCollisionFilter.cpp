// hkcollide/hkNullCollisionFilter.obj: from the records 0x7045c8 0x7045e0 0x704620 0x7046b0
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x7045c8 ?destroy_loaded_null_collision_filter@@YAXPAX@Z
// @address 0x7045e0 ??1phys_null_collision_filter@@UAA@XZ
// @address 0x704620 ?fixup_loaded_null_filter@@YAXPAX@Z
// @address 0x7046b0 ?get_null_collision_filter_vtable@@YAPBXXZ
#include "phys/phys_collide.h"

phys_null_collision_filter::~phys_null_collision_filter()
{
}

void fixup_loaded_null_filter(void *p)
{
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 1;
    new (p) phys_null_collision_filter(flag);
}

void destroy_loaded_null_collision_filter(void *p)
{
    static_cast<phys_null_collision_filter *>(p)->~phys_null_collision_filter();
}

const void *get_null_collision_filter_vtable()
{
    union {
        __declspec(align(16)) void *ptr;
        char buf[sizeof(phys_null_collision_filter)];
    } u;
    phys_fixup_tag flag;
    new (u.buf) phys_null_collision_filter(flag);
    return u.ptr;
}
