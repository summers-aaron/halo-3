// hkcollide/hkGroupFilter.obj: from the records 0x625878 0x6258c0 0x625908 0x625950 0x625998 0x6259b0 0x625a48 0x625a98 0x625af0 0x625b28 0x625b60 0x625bb8 0x625c40 0x625c50 0x625d00 0x625d40 0x625d50 0x625d60 0x625ec8 0x625f48 0x625f50 0x625f58 0x625f60 0x625f68
// @flags /TP /O2 /Oc
#include "phys/phys_base_types.h"
#include "phys/phys_group_filter.h"
#include "phys/phys_object.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_shape_type_enum.h"
// @address 0x60a380 ?free_chunk@phys_thread_memory@@QAAXPAXHW4phys_memory_class@@@Z
// @address 0x5bec98 ?gather_memory_stats@phys_ref_object@@UBAXPAVphys_statistics_collector@@@Z
// @address 0x625998 ?destroy_loaded_group_filter@@YAXPAX@Z
// @address 0x6259b0 ?allows_collision@phys_group_filter@@QBA?AVphys_bool@@II@Z
// @address 0x625bb8 ?fixup_loaded_group_filter@@YAXPAX@Z
// @address 0x625c50 ??0phys_group_filter@@QAA@XZ

// @address 0x625998 ?destroy_loaded_group_filter@@YAXPAX@Z
void destroy_loaded_group_filter(void *object)
{
    static_cast<phys_group_filter *>(object)->~phys_group_filter();
}

// @address 0x625b60 ?disable_layer_bits@phys_group_filter@@QAAXII@Z
// @address 0x625c40 ?get_group_filter_vtable@@YAPBXXZ

// @address 0x6259b0 ?allows_collision@phys_group_filter@@QBA?AVphys_bool@@II@Z
phys_bool phys_group_filter::allows_collision(unsigned int group_filter_info_a, unsigned int group_filter_info_b) const
{
    if (((group_filter_info_a ^ group_filter_info_b) & 0xffff0000) == 0 && (group_filter_info_a & 0xffff0000) != 0) {
        int subsys_id_a = (group_filter_info_a >> 5) & 31;
        int subsys_skip_b = (group_filter_info_b >> 10) & 31;
        if (subsys_id_a == subsys_skip_b) {
            return phys_bool(false);
        }
        int subsys_id_b = (group_filter_info_b >> 5) & 31;
        int subsys_skip_a = (group_filter_info_a >> 10) & 31;
        if (subsys_id_b == subsys_skip_a) {
            return phys_bool(false);
        }
        return phys_bool(true);
    }
    int layer_mask = 31;
    int layer_bits_a = m_collision_lookup[group_filter_info_a & layer_mask];
    int layer_bit_b = 1 << (group_filter_info_b & layer_mask);
    return 0 != (layer_bits_a & layer_bit_b);
}

// @address 0x625a48 ?allows_collision@phys_group_filter@@UBA?AVphys_bool@@ABUphys_ray_input@@ABVphys_shape@@ABVphys_shape_container@@I@Z
phys_bool phys_group_filter::allows_collision(const phys_ray_input &input, const phys_shape &, const phys_shape_container &container, phys_shape_key key) const
{
    unsigned int list_filter_info = container.get_collision_filter_info(key);
    return allows_collision(input.m_filter_info, list_filter_info);
}

// @address 0x625a98 ?enable_collisions_by_mask@phys_group_filter@@QAAXII@Z
// @address 0x625af0 ?enable_collisions_between@phys_group_filter@@QAAXHH@Z
// @address 0x625b28 ?disable_layer_pair@phys_group_filter@@QAAXHH@Z
void phys_group_filter::enable_collisions_by_mask(unsigned int layer_bits_a, unsigned int layer_bits_b)
{
    for (int layer = 0; layer < 32; ++layer) {
        int layer_bit = 1 << layer;
        if ((layer_bit & layer_bits_a) != 0) {
            m_collision_lookup[layer] |= layer_bits_b;
        }
        if ((layer_bit & layer_bits_b) != 0) {
            m_collision_lookup[layer] |= layer_bits_a;
        }
    }
}
void phys_group_filter::enable_collisions_between(int layer_a, int layer_b)
{
    m_collision_lookup[layer_a] |= 1 << layer_b;
    m_collision_lookup[layer_b] |= 1 << layer_a;
}
void phys_group_filter::disable_layer_pair(int layer_a, int layer_b)
{
    m_collision_lookup[layer_a] &= ~(1 << layer_b);
    m_collision_lookup[layer_b] &= ~(1 << layer_a);
}

void phys_group_filter::disable_layer_bits(unsigned int layer_bits_a, unsigned int layer_bits_b)
{
    for (int layer = 0; layer < 32; ++layer) {
        int layer_bit = 1 << layer;
        if ((layer_bit & layer_bits_a) != 0) {
            m_collision_lookup[layer] &= ~layer_bits_b;
        }
        if ((layer_bit & layer_bits_b) != 0) {
            m_collision_lookup[layer] &= ~layer_bits_a;
        }
    }
}

void fixup_loaded_group_filter(void *object)
{
    phys_fixup_tag flag;
    new (object) phys_group_filter(flag);
}

const void *get_group_filter_vtable()
{
    union {
        __declspec(align(16)) void *ptr;
        char buf[sizeof(phys_group_filter)];
    } u;
    phys_fixup_tag flag;
    new (u.buf) phys_group_filter(flag);
    return u.ptr;
}

phys_group_filter::phys_group_filter()
{
    m_filter_kind = filter_kind_group;
    for (int i = 0; i < 32; i++) {
        m_collision_lookup[i] = 0xffffffff;
    }
    m_next_free_sys_group = 0;
}

// @address 0x625d00 ??1phys_group_filter@@UAA@XZ
phys_group_filter::~phys_group_filter() {}

// @address 0x625d40 ?allows_collision@phys_group_filter@@UBA?AVphys_bool@@ABVphys_collider@@0@Z
phys_bool phys_group_filter::allows_collision(const phys_collider &a, const phys_collider &b) const
{
    return allows_collision(a.get_collision_filter_info(), b.get_collision_filter_info());
}

// @address 0x625d50 ?allows_collision@phys_group_filter@@UBA?AVphys_bool@@ABUphys_world_ray_input@@ABVphys_collider@@@Z
phys_bool phys_group_filter::allows_collision(const phys_world_ray_input &input, const phys_collider &collidable) const
{
    return allows_collision(input.m_filter_info, collidable.get_collision_filter_info());
}

// @address 0x625d60 ?allows_collision@phys_group_filter@@UBA?AVphys_bool@@ABUphys_collision_input@@ABVphys_collision_proxy@@1ABVphys_shape_container@@I@Z
phys_bool phys_group_filter::allows_collision(
    const phys_collision_input &input, const phys_collision_proxy &agent_body_a, const phys_collision_proxy &,
        const phys_shape_container &collector_container_b, phys_shape_key container_key_b) const
{
    unsigned int filter_bits_b = collector_container_b.get_collision_filter_info(container_key_b);
    unsigned int filter_bits_a;
    if (agent_body_a.get_shape_key() == PHYS_INVALID_SHAPE_KEY) {
        filter_bits_a = agent_body_a.get_root_collider()->get_collision_filter_info();
    } else {
        const phys_collision_proxy *parent = agent_body_a.get_parent_proxy();
        do {
            phys_shape_type type = parent->get_shape()->get_type_id();
            if (input.m_agent_dispatcher->has_alternate_type(type, PHYS_SHAPE_COMPOUND)) {
                const phys_compound_shape *collection = static_cast<const phys_compound_shape *>(parent->get_shape());
                filter_bits_a = collection->get_collision_filter_info(agent_body_a.get_shape_key());
                break;
            } else if (input.m_agent_dispatcher->has_alternate_type(type, PHYS_SHAPE_BVH)) {
                const phys_shape *bvh_shape_ptr = parent->get_shape();
                const phys_shape_container *container = bvh_shape_ptr->get_shape_container();
                filter_bits_a = container->get_collision_filter_info(agent_body_a.get_shape_key());
                break;
            } else if (input.m_agent_dispatcher->has_alternate_type(type, PHYS_SHAPE_MULTI_SPHERE)) {
                filter_bits_a = agent_body_a.get_root_collider()->get_collision_filter_info();
                break;
            } else if (input.m_agent_dispatcher->has_alternate_type(type, PHYS_SHAPE_CONVEX_LIST)) {
                return true;
            } else {
                parent = parent->get_parent_proxy();
            }
        } while (parent);
        if (!parent) filter_bits_a = 0;
    }
    return allows_collision(filter_bits_a, filter_bits_b);
}

// Compiler-generated: the inline interface destructors of hkCollisionFilter, the deleting destructors of the
// interfaces and of hkGroupFilter, and the this-adjusting thunks of its secondary vtables.
// @address 0x625ec8 ??1phys_collision_filter@@UAA@XZ
// @address 0x625878 ??_Gphys_collider_pair_filter@@UAAPAXI@Z
// @address 0x6258c0 ??_Gphys_ray_collider_filter@@UAAPAXI@Z
// @address 0x625908 ??_Gphys_compound_shape_filter@@UAAPAXI@Z
// @address 0x625950 ??_Gphys_ray_compound_shape_filter@@UAAPAXI@Z
// @address 0x625f48 ??_Ephys_group_filter@@WBE@AAPAXI@Z
// @address 0x625f50 ??_Ephys_group_filter@@W7AAPAXI@Z
// @address 0x625f58 ??_Ephys_group_filter@@WM@AAPAXI@Z
// @address 0x625f60 ??_Ephys_group_filter@@WBA@AAPAXI@Z
// @address 0x625f68 ??_Gphys_group_filter@@UAAPAXI@Z
