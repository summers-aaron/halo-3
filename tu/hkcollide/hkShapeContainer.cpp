// hkcollide/hkShapeContainer.obj: from the records 0x620a80 0x620a98 0x620aa0 0x620b18 0x620b30
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x620a80 ?destroy_loaded_single_shape_container@@YAXPAX@Z
// @address 0x620b18 ?fixup_loaded_single_shape_container@@YAXPAX@Z
// @address 0x620b30 ?get_single_shape_container_vtable@@YAPBXXZ
#include "phys/phys_collide.h"

void fixup_loaded_single_shape_container(void *p)
{
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 1;
    new (p) phys_single_shape_container(flag);
}

void destroy_loaded_single_shape_container(void *p)
{
    static_cast<phys_single_shape_container *>(p)->~phys_single_shape_container();
}

const void *get_single_shape_container_vtable()
{
    union {
        __declspec(align(16)) void *ptr;
        char buf[sizeof(phys_single_shape_container)];
    } u;
    phys_fixup_tag flag;
    new (u.buf) phys_single_shape_container(flag);
    return u.ptr;
}

// @address 0x620a98 ?get_collision_filter_info@phys_shape_container@@UBAII@Z
unsigned int phys_shape_container::get_collision_filter_info(phys_shape_key key) const
{
    return 0;
}

// @address 0x620aa0 ?get_child_shape_count@phys_shape_container@@UBAHXZ
int phys_shape_container::get_child_shape_count() const
{
    int count = 0;
    for (phys_shape_key key = get_first_key(); key != PHYS_INVALID_SHAPE_KEY; key = get_next_key(key)) {
        count++;
    }
    return count;
}
