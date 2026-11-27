// hkcollide/hkDefaultConvexListFilter.obj: from the records 0x7046c0 0x7046d8 0x7046f8 0x704708
// @flags /TP /O2 /Oc
#include "phys/phys_base_types.h"
#include "phys/phys_object.h"
#include "phys/phys_collide.h"
#include "phys/phys_collision_input.h"
// @address 0x7046c0 ?destroy_loaded_convex_list_filter@@YAXPAX@Z
// @address 0x7046f8 ?get_default_convex_list_filter_vtable@@YAPBXXZ
// @address 0x704708 ?get_convex_list_mode@phys_default_convex_list_filter@@UBA?AW4convex_list_collision_kind@phys_convex_list_filter@@ABVphys_collision_proxy@@0ABUphys_collision_input@@@Z
// @address 0x7046d8 ?fixup_loaded_convex_list_filter@@YAXPAX@Z

// Same layout as the accepted havok_component.cpp declaration.

#include "phys/phys_dispatcher.h"
#include "phys/phys_default_convex_list_filter.h"
#include "phys/phys_statistics_collector.h"

void destroy_loaded_convex_list_filter(void *storage)
{
    static_cast<phys_default_convex_list_filter *>(storage)->~phys_default_convex_list_filter();
}

void fixup_loaded_convex_list_filter(void *storage)
{
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 1;
    new (storage) phys_default_convex_list_filter(flag);
}

const void *get_default_convex_list_filter_vtable()
{
    phys_fixup_tag flag;
    union { __vector4 alignment; char storage[sizeof(phys_default_convex_list_filter)]; } buffer;
    new (buffer.storage) phys_default_convex_list_filter(flag);
    return *(const void **)buffer.storage;
}

phys_convex_list_filter::convex_list_collision_kind phys_default_convex_list_filter::get_convex_list_mode(
    const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_collision_input &input) const
{
    phys_shape_type type = agent_body_b.get_shape()->get_type_id();
    if (input.m_agent_dispatcher->has_alternate_type(type, PHYS_SHAPE_BVH)) {
        return phys_convex_list_filter::CONVEX_LIST_MODE_LIST;
    } else {
        return phys_convex_list_filter::CONVEX_LIST_MODE_DEFAULT;
    }
}
