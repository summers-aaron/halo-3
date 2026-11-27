// hkcollide/hkBvTreeAgent.obj: from the records 0x104c08 0x61ae88 0x61ae98 0x61aea8 0x61af30 0x61afa0 0x61b030 0x61b080 0x61b258 0x61b350 0x61b3b0 0x61b458 0x61be48 0x61cbc8 0x61cd60 0x61cf18 0x61d0e0 0x61d190 0x61d2e0 0x61d2f8 0x61d310 0x61d328 0x61d428 0x61d440 0x61d4f8 0x61d5b0 0x61d650 0x61d6d0 0x61d760 0x61d7b0 0x61d800 0x61d8e8 0x628a10 0x628bc8 0x637448
// @flags /TP /O2 /Oc
// @address 0x61ae98 ?set_use_aabb_caching@phys_bvh_agent@@SAXVphys_bool@@@Z
// @address 0x61aea8 ?cleanup@phys_bvh_agent@@UAAXAAVphys_constraint_owner@@@Z
// @address 0x61af30 ?invalidate_sweep_info@phys_bvh_agent@@UAAXAAUphys_collision_input@@@Z
// @address 0x61afa0 ?warp_time@phys_bvh_agent@@UAAXMMAAUphys_collision_input@@@Z
// @address 0x61b030 ?default_agent_create@phys_bvh_agent@@KAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
// @address 0x61b080 ?update_compound_filter@phys_bvh_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_constraint_owner@@@Z
// @address 0x61b258 ?gather_memory_stats@phys_bvh_agent@@MBAXPAVphys_statistics_collector@@@Z
// @address 0x61b350 ??0phys_bvh_agent@@QAA@PAVphys_contact_manager@@@Z
// @address 0x61b3b0 ?create_shape_bounding_agent@phys_bvh_agent@@KAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
// @address 0x61b458 ?prepare_collision_partners@phys_bvh_agent@@IAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_constraint_owner@@@Z
// @address 0x61be48 ?process_collision@phys_bvh_agent@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
// @address 0x61cbc8 ?static_sweep_cast@phys_bvh_agent@@SAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
// @address 0x61cd60 ?static_closest_points@phys_bvh_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x61cf18 ?static_penetrations@phys_bvh_agent@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x61d0e0 ?create_bvh_shape_agent@phys_bvh_agent@@KAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
// @address 0x61d190 ?create_bvh_bvh_agent@phys_bvh_agent@@KAPAVphys_agent@@ABVphys_collision_proxy@@0ABUphys_collision_input@@PAVphys_contact_manager@@@Z
// @address 0x61d2e0 ?sweep_cast@phys_bvh_agent@@UAAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
// @address 0x61d2f8 ?get_closest_points@phys_bvh_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x61d310 ?get_penetrations@phys_bvh_agent@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x61d328 ?register_agent@phys_bvh_agent@@SAXPAVphys_dispatcher@@@Z
// @address 0x61d428 ?add_volume_callback_shape@phys_symmetric_flip_collector@@UAAXABVphys_collision_proxy@@@Z
// @address 0x61d650 ??1phys_bvh_agent@@MAA@XZ
// @address 0x61d7b0 ?static_closest_points@?$phys_symmetric_agent_linear_cast@Vphys_bvh_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
// @address 0x61d800 ?static_sweep_cast@?$phys_symmetric_agent_linear_cast@Vphys_bvh_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
#include "phys/phys_base_types.h"
#include "phys/phys_aabb_types.h"
#include "phys/phys_collision_quality_info.h"
#include "phys/phys_contact_point.h"
#include "phys/phys_collectors.h"
// @address 0x61d440 ??_Gphys_symmetric_flip_collector@@UAAPAXI@Z
// @address 0x61d4f8 ??_Gphys_symmetric_flip_pair_collector@@UAAPAXI@Z
// @address 0x6289c8 ?get_penetrations@?$phys_symmetric_agent_linear_cast@Vphys_bvh_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x5bec98 ?add_volume_callback_shape@phys_hit_collector@@UAAXABVphys_collision_proxy@@@Z
// @address 0x5bec98 ?add_volume_callback_shape@phys_symmetric_flip_cast_collector@@UAAXABVphys_collision_proxy@@@Z
// @address 0x61d760 ?static_penetrations@?$phys_symmetric_agent_linear_cast@Vphys_bvh_agent@@@@SAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_proxy_pair_collector@@@Z
// @address 0x61d6d0 ?flip@toi_info@phys_process_collision_data@@QAAXXZ
// @address 0x1b65a8 ?havok_thread_memory_allocate_stack@@YAPAXH@Z
// @address 0x1b65a8 ?havok_thread_memory_deallocate_stack@@YAXPAX@Z
#include <vectorintrinsics.h>
#include "phys/phys_algorithm_sort.h"
#include "physics/havok_memory.h"
#include "phys/phys_math.h"
#include "phys/phys_object.h"
#include "phys/phys_step_info.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_swept_transform_util_sweep.h"
#include "phys/phys_shape_type_enum.h"
#include "phys/phys_memory.h"
#include "phys/phys_bvh_agent.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_triangle_shape.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_null_agent.h"
#include "phys/phys_local_array.h"
#include "phys/phys_symmetric_agent.h"
#include "phys/phys_bvh_tree_agent.h"
#include "phys/phys_bvh_stream_agent.h"
typedef phys_bvh_agent::phys_bounding_agent_entry_info phys_bounding_agent_entry_info;





struct phys_process_collision_input;
struct phys_process_collision_output;
// match: the thread memory is read straight from its TLS slot here; spelling it hkThreadMemory::getInstancePtr()
// adds one inline level, which flips a commutative add in processCollision (0x61be48, .cod and image).
#define PHYS_TMEM() TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot)

template <typename key_t, typename entry_t, class dispatch_helper> class phys_agent_dispatch_util {
public:
    static __forceinline void update(phys_array<entry_t> &entries, phys_array<key_t> &keys, const phys_collision_proxy &cA, const phys_collision_proxy &cB,
                                     const phys_collision_input &input, phys_contact_manager *pair_contact_manager,
                                         phys_constraint_owner &constraint_owner_ref, dispatch_helper &base);
    static __forceinline void fast_update(phys_array<key_t> &hit_key_list, const phys_collision_proxy &cA,
        const phys_collision_proxy &cB, const phys_collision_input &input,
    phys_contact_manager *pair_contact_manager, phys_constraint_owner &constraint_owner_ref, dispatch_helper &base, phys_array<entry_t> &entries);
};


#include "phys/phys_bvh_agent_specializations.h"
typedef unsigned int phys_shape_key;

#define PHYS_PROXY_INIT_CHILD(B, P) do { (B).m_parent=(P); (B).m_motion=(P)->m_motion; } while (0)


#define PHYS_INLINE_ARRAY_INIT(A) do { (A).m_data = (A).m_storage; (A).m_size = 0; \
    (A).m_capacity_and_flags = (int)(sizeof((A).m_storage) / sizeof((A).m_storage[0])) | PHYS_ARRAY_DONT_DEALLOCATE_FLAG; } while (0)

/* hkStatisticsCollector::addArray<T> (inlined; non-leaf so a macro) */
#define PHYS_STAT_ADD_ARRAY(C, STAT_NAME, TYPE, T, A) do { const phys_array<T> &phys_stat_array_ref = (A); \
    if ((phys_stat_array_ref.m_capacity_and_flags & PHYS_ARRAY_DONT_DEALLOCATE_FLAG) == 0) \
        (C)->add_chunk_stats(STAT_NAME, TYPE, phys_stat_array_ref.m_data, \
            phys_stat_array_ref.m_size * sizeof(T), phys_stat_array_ref.get_sector_capacity() * sizeof(T)); \
} while (0)

/* hkCollisionDispatcher::getNewCollisionAgent (EXACT 0x61b030, 0x61b080; non-leaf so a macro): R = agent.
   Inline-call semantics: the mgr argument is bound to a local FIRST, then dispatcher, typeA, typeB. */
#define PHYS_GET_NEW_COLLISION_AGENT(R, A, B, IN, contact_manager_arg) do { \
    phys_contact_manager *phys_contact_manager_ = (contact_manager_arg); \
    const phys_dispatcher *phys_dispatcher_local = (IN).m_agent_dispatcher; \
    phys_shape_type phys_type_a_ = (A).get_shape()->get_type_id(); phys_shape_type phys_type_b_ = (B).get_shape()->get_type_id(); \
    int phys_agent_idx_local = phys_dispatcher_local->get_agent_type(phys_type_a_, phys_type_b_, (IN).m_create_pred_agents); \
    phys_dispatcher::agent_funcs::agent_create_fn phys_create_func_local = \
        phys_dispatcher_local->m_agent2_funcs[phys_agent_idx_local].m_create_fn; \
    (R) = phys_create_func_local((A), (B), (IN), phys_contact_manager_); \
} while (0)

/* hkBvTreeAgent::staticCalcAabb (inlined in staticGetClosestPoints 0x61cd60) */
#define PHYS_STATIC_COMPUTE_BOUNDS(A, B, IN, out_aabb) do { \
    phys_transform phys_b_from_a_xform; \
    phys_b_from_a_xform.set_mul_inverse_mul((B).get_transform(), (A).get_transform()); \
    (A).get_shape()->get_handle_aabb(phys_b_from_a_xform, (IN).m_box_tolerance, (out_aabb)); } while (0)


typedef phys_dispatcher::agent_funcs::sweep_cast_fn phys_linear_cast_func;
typedef phys_dispatcher::agent_funcs::closest_points_fn phys_get_closest_points_func;
typedef phys_dispatcher::agent_funcs::penetrations_fn phys_get_penetrations_func;



#include "phys/phys_array_util.h"
// @address 0x72487e ?m_use_aabb_cache@phys_bvh_agent@@1Vphys_bool@@A
phys_bool phys_bvh_agent::m_use_aabb_cache = true;
// @address 0x7ce7b0 ?m_use_fast_update@phys_bvh_agent@@1Vphys_bool@@A
phys_bool phys_bvh_agent::m_use_fast_update;



class phys_agent_dispatch_helper {
public:
    __forceinline phys_agent_dispatch_helper(const phys_collision_proxy &body)
    {
        PHYS_PROXY_INIT_CHILD(m_body_b, &body);
    }
    const phys_shape_container *m_shape_container;
    phys_collision_proxy m_body_b;
    __declspec(align(16)) phys_shape_buffer m_shape_buf;

    __forceinline const phys_collision_proxy *get_body_b(const phys_collision_proxy &proxy_in, const phys_collision_input &input, phys_shape_key key)
    {
        m_body_b.set_proxy_shape(m_shape_container->get_child_shape(key, m_shape_buf), key);
        return &m_body_b;
    }
    __forceinline const phys_shape_container *get_compound_b()
    {
        return m_shape_container;
    }
};

template <typename key_t, typename entry_t, class dispatch_helper>
__forceinline void phys_agent_dispatch_util<key_t, entry_t, dispatch_helper>::update(phys_array<entry_t> &entries,
    phys_array<key_t> &keys, const phys_collision_proxy &cA, const phys_collision_proxy &cB,
                                                                   const phys_collision_input &input, phys_contact_manager *pair_contact_manager,
    phys_constraint_owner &constraint_owner_ref, dispatch_helper &base)
{
    entry_t *old_entries_iter = entries.begin();
    entry_t *old_entries_end = entries.end();
    key_t *new_keys_iter = keys.begin();
    key_t *new_keys_end = keys.end();

    phys_local_array<entry_t> new_entries(keys.getSize());
    new_entries.set_array_size(keys.getSize());

    const phys_shape_container *shape_container = base.get_compound_b();

    entry_t *new_entries_iter = new_entries.begin();

    while ((old_entries_iter != old_entries_end) && (new_keys_iter != new_keys_end))
    {
        if ((*new_keys_iter) == (*old_entries_iter).get_bvh_key())
        {
            *new_entries_iter = *old_entries_iter;
            new_entries_iter++;
            old_entries_iter++;
            new_keys_iter++;
            continue;
        }

        if ((*new_keys_iter) < (*old_entries_iter).get_bvh_key())
        {
            const phys_collision_proxy &agent_body_a = cA;
            const phys_collision_proxy &agent_body_b = cB;
            const phys_collision_proxy &modified_b = *base.get_body_b(cB, input, *new_keys_iter);
            if (input.m_compound_filter->allows_collision(input, agent_body_a, agent_body_b, *shape_container, *new_keys_iter))
            {
                PHYS_GET_NEW_COLLISION_AGENT(new_entries_iter->m_collision_agent, agent_body_a, modified_b, input, pair_contact_manager);
            }
            else
            {
                new_entries_iter->m_collision_agent = phys_null_agent::get_null_agent();
            }
            new_entries_iter->set_cached_key(*new_keys_iter);
            new_entries_iter++;
            new_keys_iter++;
            continue;
        }

        {
            if (old_entries_iter->m_collision_agent != 0)
            {
                old_entries_iter->m_collision_agent->cleanup(constraint_owner_ref);
            }
            old_entries_iter++;
        }
    }

    while (old_entries_iter != old_entries_end)
    {
        if (old_entries_iter->m_collision_agent != 0)
        {
            old_entries_iter->m_collision_agent->cleanup(constraint_owner_ref);
        }
        old_entries_iter++;
    }

    while (new_keys_iter != new_keys_end)
    {
        const phys_collision_proxy &agent_body_a = cA;
        const phys_collision_proxy &agent_body_b = cB;
        const phys_collision_proxy &modified_b = *base.get_body_b(cB, input, *new_keys_iter);
        if (input.m_compound_filter->allows_collision(input, agent_body_a, agent_body_b, *shape_container, *new_keys_iter))
        {
            PHYS_GET_NEW_COLLISION_AGENT(new_entries_iter->m_collision_agent, agent_body_a, modified_b, input, pair_contact_manager);
        }
        else
        {
            new_entries_iter->m_collision_agent = phys_null_agent::get_null_agent();
        }
        new_entries_iter->set_cached_key(*new_keys_iter);
        new_entries_iter++;
        new_keys_iter++;
    }

    entries = new_entries;
}

template <typename key_t, typename entry_t, class dispatch_helper>
__forceinline void phys_agent_dispatch_util<key_t, entry_t, dispatch_helper>::fast_update(phys_array<key_t> &hit_key_list,
    const phys_collision_proxy &cA, const phys_collision_proxy &cB,
    const phys_collision_input &input, phys_contact_manager *pair_contact_manager, phys_constraint_owner &constraint_owner_ref,
        dispatch_helper &base, phys_array<entry_t> &entries)
{
    {
        entry_t *entry_iter = entries.begin();
        entry_t *end = entries.end();

        phys_shape_key *last_hit = hit_key_list.begin();
        phys_shape_key *hit_list_end = hit_key_list.end();

        for (; entry_iter != end; entry_iter++)
        {
            if (last_hit != hit_list_end && entry_iter->get_bvh_key() == *last_hit)
            {
                last_hit++;
                continue;
            }
            {
                for (last_hit = hit_key_list.begin(); last_hit != hit_list_end; last_hit++)
                {
                    if (entry_iter->get_bvh_key() == *last_hit)
                    {
                        last_hit++;
                        goto bvh_hit_label;
                    }
                }
            }
            entry_iter->m_collision_agent->cleanup(constraint_owner_ref);
            entries.remove_at_and_copy(static_cast<int>((unsigned long)(entry_iter - entries.begin())));
            entry_iter--;
            end--;
bvh_hit_label:;
        }
    }

    const phys_shape_container *shape_container = base.get_compound_b();

    {
        if (hit_key_list.getSize() != entries.getSize())
        {
            entry_t *last_hit = entries.begin();
            entry_t *end = entries.end();

            phys_shape_key *hit_iter = hit_key_list.begin();
            phys_shape_key *hit_list_end = hit_key_list.end();

            for (; hit_iter != hit_list_end; hit_iter++)
            {
                if (last_hit != end && last_hit->get_bvh_key() == *hit_iter)
                {
                    last_hit++;
                    continue;
                }

                int index = static_cast<int>((unsigned long)(hit_iter - hit_key_list.begin()));
                last_hit = entries.expand_at(index, 1);

                const phys_collision_proxy &agent_body_a = cA;
                const phys_collision_proxy &agent_body_b = cB;
                const phys_collision_proxy &modified_b = *base.get_body_b(cB, input, *hit_iter);
                if (input.m_compound_filter->allows_collision(input, agent_body_a, agent_body_b, *shape_container, *hit_iter))
                {
                    PHYS_GET_NEW_COLLISION_AGENT(last_hit->m_collision_agent, agent_body_a, modified_b, input, pair_contact_manager);
                }
                else
                {
                    last_hit->m_collision_agent = phys_null_agent::get_null_agent();
                }
                last_hit->m_key = *hit_iter;
                end = entries.end();
                last_hit++;
            }
        }
    }
}


// @address 0x104c08 ??$quick_sort_recursive@IV?$less@I@phys_algorithm@@@phys_algorithm@@YAXPAIHHV?$less@I@0@@Z
template <>
void phys_algorithm::quick_sort_recursive(unsigned int *sort_items, int d, int h, phys_algorithm::less<unsigned int> less_than_fn)
{
    int i, j;
    unsigned int str;

    for (;;)
    {
        j = h;
        i = d;

        str = sort_items[(d + h) >> 1];

        do {
            while (less_than_fn(sort_items[i], str)) { i++; }
            while (less_than_fn(str, sort_items[j])) { j--; }

            if (i <= j)
            {
                if (j != i)
                {
                    unsigned int t = sort_items[j];
                    sort_items[j] = sort_items[i];
                    sort_items[i] = t;
                }
                j--;
                i++;
            }
        } while (i <= j);

        if (d < j)
        {
            phys_algorithm::quick_sort_recursive(sort_items, d, j, less_than_fn);
        }
        if (i < h)
        {
            d = i;
        }
        else
        {
            break;
        }
    }
}

// @address 0x61ae88 ?get_use_aabb_caching@phys_bvh_agent@@SA?AVphys_bool@@XZ
phys_bool phys_bvh_agent::get_use_aabb_caching()
{
    return m_use_aabb_cache;
}

void phys_bvh_agent::set_use_aabb_caching(phys_bool enabled)
{
    m_use_aabb_cache = enabled;
}

void phys_bvh_agent::cleanup(phys_constraint_owner &constraint_owner_ref)
{
    phys_bounding_agent_entry_info *entry_iter = m_collision_partners.begin();
    phys_bounding_agent_entry_info *end = m_collision_partners.end();

    for (; entry_iter != end; entry_iter++)
    {
        if (entry_iter->m_collision_agent != 0)
        {
            entry_iter->m_collision_agent->cleanup(constraint_owner_ref);
        }
    }
    delete this;
}

void phys_bvh_agent::invalidate_sweep_info(phys_collision_input &input)
{
    phys_bounding_agent_entry_info *entry_iter = m_collision_partners.begin();
    phys_bounding_agent_entry_info *end = m_collision_partners.end();

    for (; entry_iter != end; entry_iter++)
    {
        if (entry_iter->m_collision_agent != 0)
        {
            entry_iter->m_collision_agent->invalidate_sweep_info(input);
        }
    }
}

void phys_bvh_agent::warp_time(float agent_old_time, float agent_new_time, phys_collision_input &input)
{
    phys_bounding_agent_entry_info *entry_iter = m_collision_partners.begin();
    phys_bounding_agent_entry_info *end = m_collision_partners.end();

    for (; entry_iter != end; entry_iter++)
    {
        if (entry_iter->m_collision_agent != 0)
        {
            entry_iter->m_collision_agent->warp_time(agent_old_time, agent_new_time, input);
        }
    }
}

phys_agent *phys_bvh_agent::default_agent_create(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_contact_manager *pair_contact_manager)
{
  const phys_dispatcher *dispatcher = input.m_agent_dispatcher;
  int shape_type_a = agent_body_a.get_shape()->get_type_id();
  int shape_type_b = agent_body_b.get_shape()->get_type_id();
  int dispatch_agent_type = dispatcher->get_agent_type(shape_type_a, shape_type_b, input.m_create_pred_agents);
  phys_dispatcher::agent_funcs::agent_create_fn create = dispatcher->m_agent2_funcs[dispatch_agent_type].m_create_fn;
  return create(agent_body_a, agent_body_b, input, pair_contact_manager);
}

void phys_bvh_agent::update_compound_filter(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_constraint_owner &constraint_owner_ref)
{
    const phys_bvh_shape *bvh_shape_b = (const phys_bvh_shape *)agent_body_b.get_shape();
    const phys_shape_container *compound_shape = bvh_shape_b->get_shape_container();

    for (int i = 0; i < this->m_collision_partners.getSize(); i++)
    {
        phys_shape_buffer shape_scratch_buffer;
        const phys_shape *shape = compound_shape->get_child_shape(this->m_collision_partners[i].m_key, shape_scratch_buffer);
        phys_collision_proxy modified_proxy_b;
        PHYS_PROXY_INIT_CHILD(modified_proxy_b, &agent_body_b);
        modified_proxy_b.set_proxy_shape(shape, this->m_collision_partners[i].m_key);

        if (input.m_compound_filter->allows_collision(input, agent_body_a, agent_body_b, *compound_shape, this->m_collision_partners[i].m_key))
        {
            if (this->m_collision_partners[i].m_collision_agent == phys_null_agent::get_null_agent())
            {
                phys_agent *pair_agent_entry;
                PHYS_GET_NEW_COLLISION_AGENT(pair_agent_entry, agent_body_a, modified_proxy_b, input, this->m_contact_manager);
                this->m_collision_partners[i].m_collision_agent = pair_agent_entry;
            }
            else
            {
                this->m_collision_partners[i].m_collision_agent->update_compound_filter(agent_body_a, modified_proxy_b, input, constraint_owner_ref);
            }
        }
        else
        {
            if (this->m_collision_partners[i].m_collision_agent != phys_null_agent::get_null_agent())
            {
                this->m_collision_partners[i].m_collision_agent->cleanup(constraint_owner_ref);
                this->m_collision_partners[i].m_collision_agent = phys_null_agent::get_null_agent();
            }
        }
    }
}

// @address 0x61b458
void phys_bvh_agent::prepare_collision_partners(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_constraint_owner &constraint_owner_ref)
{
    phys_aabb aabb;
    {
        phys_transform b_from_a;
        const phys_transform &world_from_b = agent_body_b.get_transform();
        const phys_transform &world_from_a = agent_body_a.get_transform();
        b_from_a.set_mul_inverse_mul(world_from_b, world_from_a);
        float half_tolerance = input.m_box_tolerance * 0.5f;
        agent_body_a.get_shape()->get_handle_aabb(b_from_a, half_tolerance + input.m_box_tolerance, aabb);
    }

    if (m_use_aabb_cache)
    {
        if (m_cached_aabb.contains(aabb))
        {
            return;
        }
        m_cached_aabb = aabb;
    }

    const phys_bvh_shape *bvh_shape_b = (const phys_bvh_shape *)agent_body_b.get_shape();
    phys_inline_array<phys_shape_key, 128> hit_key_list;
    bvh_shape_b->query_aabb(aabb, hit_key_list);

    // Keep the partner-count snapshot and the output array last in fastUpdate: native c2 operand ranks.
    const int new_partner_count = hit_key_list.getSize() - m_collision_partners.getSize();
    if (new_partner_count * 128 > havok_get_available_memory())
    {
        phys_memory::get_singleton_instance().m_memory_state = phys_memory::MEMORY_STATE_EXHAUSTED;

        return;
    }

    {
        const phys_shape_container *compound_shape = bvh_shape_b->get_shape_container();
        phys_agent_dispatch_helper helper(agent_body_b);
        helper.m_shape_container = compound_shape;

        if (m_use_fast_update)
        {
            phys_agent_dispatch_util<phys_shape_key, phys_bounding_agent_entry_info,
                phys_agent_dispatch_helper>::fast_update(hit_key_list, agent_body_a, agent_body_b, input,
                m_contact_manager, constraint_owner_ref, helper, m_collision_partners);
        }
        else
        {
            phys_algorithm::quick_sort(hit_key_list.begin(), hit_key_list.getSize());
            phys_agent_dispatch_util<phys_shape_key, phys_bounding_agent_entry_info, phys_agent_dispatch_helper>::update(m_collision_partners, hit_key_list,
                agent_body_a, agent_body_b, input, m_contact_manager, constraint_owner_ref, helper);
        }
    }

}




__forceinline phys_result phys_bvh_agent::compute_bounds_and_query_tree(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b, const phys_transform &b_from_a,
                                                           const phys_vec4 &lin_sweep_info, const phys_process_collision_input &input,
                                                           phys_aabb *cached_aabb, phys_array<phys_shape_key> &out_hit_list)
{
    phys_aabb aabb;
    {
        const phys_motion_state *motion_a_state = agent_body_a.get_motion_state();
        const phys_motion_state *motion_b_state = agent_body_b.get_motion_state();

        phys_vec4 sweep_info;
        sweep_info.set_rotated_inverse_dir(agent_body_b.get_transform().get_rotation(), lin_sweep_info);

        phys_vec4 aabb_extents;
        if (input.m_quality_info->m_use_ccd)
        {
            const float second_order_err_a = (motion_a_state->m_delta_angle(3) + motion_b_state->m_delta_angle(3)) * motion_a_state->m_object_radius;
            const float second_order_err_b = motion_b_state->m_delta_angle(3) * motion_b_state->m_delta_angle(3) * motion_b_state->m_object_radius;
            const float check_epsilon = input.m_box_tolerance * 0.5f + (second_order_err_a + second_order_err_b);
            agent_body_a.get_shape()->get_handle_aabb(b_from_a, check_epsilon, aabb);

            phys_vec4 mass_center_a_in_b;
            {
                phys_vec4 radius_vec4; radius_vec4.set_all3(motion_a_state->m_object_radius + input.m_box_tolerance * 0.5f + second_order_err_b);
                mass_center_a_in_b.set_transformed_inverse_pos(agent_body_b.get_transform(), motion_a_state->get_swept_transform().m_center_of_mass1);
                phys_vec4 max_reach; max_reach.set_add4(mass_center_a_in_b, radius_vec4);
                phys_vec4 min_radius_corner; min_radius_corner.set_sub4(mass_center_a_in_b, radius_vec4);
                aabb.m_min_limit.set_max4(aabb.m_min_limit, min_radius_corner);
                aabb.m_max_limit.set_min4(aabb.m_max_limit, max_reach);
            }

            aabb_extents.set_sub4(aabb.m_max_limit, aabb.m_min_limit);

            {
                if (motion_b_state->m_delta_angle(3) > 0.0f)
                {
                    phys_vec4 rel_pos; rel_pos.set_sub4(mass_center_a_in_b, motion_b_state->get_swept_transform().m_center_of_mass_local);
                    phys_vec4 offset_out; offset_out.set_cross(rel_pos, motion_b_state->m_delta_angle);
                    const float f = input.m_step_info.m_delta_time * motion_b_state->get_swept_transform().get_inv_delta_time();
                    sweep_info.add_scaled4(f, offset_out);
                }

                phys_vec4 zero; zero.set_zero4();
                phys_vec4 min_path; min_path.set_min4(zero, sweep_info);
                phys_vec4 bvh_max_path; bvh_max_path.set_max4(zero, sweep_info);

                aabb.m_min_limit.add4(min_path);
                aabb.m_max_limit.add4(bvh_max_path);
            }
        }
        else
        {
            const float check_epsilon = input.m_box_tolerance * 0.5f;
            agent_body_a.get_shape()->get_handle_aabb(b_from_a, check_epsilon, aabb);
            aabb_extents.set_sub4(aabb.m_max_limit, aabb.m_min_limit);
        }

        if (cached_aabb)
        {
            if (cached_aabb->contains(aabb))
            {
                return PHYS_FAILURE;
            }

            phys_vec4 zero; zero.set_zero4();
            phys_vec4 min_path; min_path.set_min4(zero, sweep_info);
            phys_vec4 bvh_max_path; bvh_max_path.set_max4(zero, sweep_info);

            phys_vec4 expand_vec; expand_vec.set_all3(input.m_box_tolerance * 0.5f);
            aabb.m_min_limit.sub4(expand_vec);
            aabb.m_max_limit.add4(expand_vec);

            const float max_expand = 0.4f;
            const float frames_look_ahead = -2.0f;

            phys_vec4 min_extent_path; min_extent_path.set_mul4(frames_look_ahead, bvh_max_path);
            phys_vec4 max_extent_path; max_extent_path.set_mul4(frames_look_ahead, min_path);

            phys_vec4 max_expand_vec; max_expand_vec.set_mul4(max_expand, aabb_extents);
            max_extent_path.set_min4(max_extent_path, max_expand_vec);
            phys_vec4 min_expand4; min_expand4.set_neg4(max_expand_vec);
            min_extent_path.set_max4(min_extent_path, min_expand4);

            aabb.m_min_limit.add4(min_extent_path);
            aabb.m_max_limit.add4(max_extent_path);
            *cached_aabb = aabb;
        }
    }

    const phys_bvh_shape *bvh_shape_b = (const phys_bvh_shape *)agent_body_b.get_shape();
    bvh_shape_b->query_aabb(aabb, out_hit_list);
    return PHYS_SUCCESS;
}

__forceinline void phys_bvh_agent::prepare_process_partners(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_process_collision_input &input, phys_constraint_owner &constraint_owner_ref)
{
    phys_inline_array<phys_shape_key, 128> hit_key_list;
    phys_result prepare_ok;
    {
        phys_transform b_from_a;
        {
            const phys_transform &world_from_b = agent_body_b.get_transform();
            const phys_transform &world_from_a = agent_body_a.get_transform();
            b_from_a.set_mul_inverse_mul(world_from_b, world_from_a);
        }

        phys_vec4 lin_sweep_info;
        {
            const phys_motion_state *motion_a_state = agent_body_a.get_motion_state();
            const phys_motion_state *motion_b_state = agent_body_b.get_motion_state();
            phys_swept_transform_util::compute_sweep_info(*motion_a_state, *motion_b_state, input.m_step_info.m_delta_time, lin_sweep_info);
        }
        phys_aabb *cached_aabb = (m_use_aabb_cache) ? &m_cached_aabb : 0;

        prepare_ok = compute_bounds_and_query_tree(agent_body_a, agent_body_b, b_from_a, lin_sweep_info, input, cached_aabb, hit_key_list);
    }

    if (prepare_ok != PHYS_SUCCESS)
    {

        return;
    }

    {
        const phys_bvh_shape *bvh_shape_b = (const phys_bvh_shape *)agent_body_b.get_shape();
        const phys_shape_container *compound_shape = bvh_shape_b->get_shape_container();
        phys_agent_dispatch_helper helper(agent_body_b);
        helper.m_shape_container = compound_shape;

        if (m_use_fast_update)
        {
            phys_agent_dispatch_util<phys_shape_key, phys_bounding_agent_entry_info,
                phys_agent_dispatch_helper>::fast_update(hit_key_list, agent_body_a, agent_body_b, input,
                m_contact_manager, constraint_owner_ref, helper, m_collision_partners);
        }
        else
        {
            phys_algorithm::quick_sort(hit_key_list.begin(), hit_key_list.getSize());
            phys_agent_dispatch_util<phys_shape_key, phys_bounding_agent_entry_info, phys_agent_dispatch_helper>::update(m_collision_partners, hit_key_list,
                agent_body_a, agent_body_b, input, m_contact_manager, constraint_owner_ref, helper);
        }
    }

}

// @address 0x61be48
void phys_bvh_agent::process_collision(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_process_collision_input &input, phys_process_collision_output &result)
{
    prepare_process_partners(agent_body_a, agent_body_b, input, *result.m_constraint_owner.val());

    phys_shape_buffer shape_scratch_buffer;

    phys_bounding_agent_entry_info *entry_iter = m_collision_partners.begin();
    phys_bounding_agent_entry_info *end = m_collision_partners.end();

    phys_collision_proxy modified_proxy_b;
    PHYS_PROXY_INIT_CHILD(modified_proxy_b, &agent_body_b);

    const phys_bvh_shape *bvh_shape_b = (const phys_bvh_shape *)agent_body_b.get_shape();
    const phys_shape_container *compound_shape = bvh_shape_b->get_shape_container();
    while (entry_iter != end)
    {
        const phys_shape *shape = compound_shape->get_child_shape(entry_iter->m_key, shape_scratch_buffer);
        modified_proxy_b.set_proxy_shape(shape, entry_iter->m_key);

        if (0)
        {
            const phys_triangle_shape *t = static_cast<const phys_triangle_shape *>(shape);

            phys_vec4 offset = agent_body_b.get_transform().get_translation();
            phys_vec4 a; a.set_add4(t->get_vertex(0), offset);
            phys_vec4 b; b.set_add4(t->get_vertex(1), offset);
            phys_vec4 c; c.set_add4(t->get_vertex(2), offset);

            phys_vec4 center; center.set_add4(a, b);
            center.add4(c);
            center.mul4(1.0f / 3.0f);
        }

        entry_iter->m_collision_agent->process_collision(agent_body_a, modified_proxy_b, input, result);
        entry_iter++;
    }
}


void phys_bvh_agent::gather_memory_stats(phys_statistics_collector *collector) const
{
    collector->begin_stats_object("BvTreeAgt", phys_statistics_collector::stat_category_runtime, this);
    PHYS_STAT_ADD_ARRAY(collector, "AgentPtrs", phys_statistics_collector::stat_category_runtime, phys_bounding_agent_entry_info, m_collision_partners);
    const phys_bounding_agent_entry_info *entry_iter = m_collision_partners.begin();
    const phys_bounding_agent_entry_info *end = m_collision_partners.end();
    for (; entry_iter != end; entry_iter++)
    {
        if (entry_iter->m_collision_agent)
        {
            collector->add_child_object("Agent", phys_statistics_collector::stat_category_runtime, entry_iter->m_collision_agent);
        }
    }
}

// match: 0x61d190: inline expands the constructor into the three create*Agent factories; out of line each
// construction becomes a call (84 bytes shorter per construction)
#if defined(_XBOX) && !defined(__clang__)
inline phys_bvh_agent::phys_bvh_agent(phys_contact_manager *pair_contact_manager) : phys_agent(pair_contact_manager)
#else
phys_bvh_agent::phys_bvh_agent(phys_contact_manager *pair_contact_manager) : phys_agent(pair_contact_manager)
#endif
{
    m_cached_aabb.m_max_limit.set_all3(3.40282e38f);
    m_cached_aabb.m_min_limit.set_all3(3.40282e38f);
}

phys_agent *phys_bvh_agent::create_shape_bounding_agent(const phys_collision_proxy &, const phys_collision_proxy &,
    const phys_collision_input &, phys_contact_manager *pair_contact_manager)
{
    return new phys_bvh_agent(pair_contact_manager);
}

void phys_bvh_agent::static_sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input,
    phys_hit_collector &collector, phys_hit_collector *start_collector)
{
    phys_aabb aabb;
    compute_sweep_cast_bounds(agent_body_a, agent_body_b, input, aabb);

    const phys_bvh_shape *bvh_shape_b = (const phys_bvh_shape *)agent_body_b.get_shape();
    phys_inline_array<phys_shape_key, 128> hit_key_list;
    bvh_shape_b->query_aabb(aabb, hit_key_list);

    phys_shape_type shape_type_a = agent_body_a.get_shape()->get_type_id();
    phys_shape_key *entry_iter = hit_key_list.begin();
    phys_shape_key *end = hit_key_list.end();
    phys_collision_proxy modified_proxy_b;
    PHYS_PROXY_INIT_CHILD(modified_proxy_b, &agent_body_b);
    const phys_shape_container *compound_shape = bvh_shape_b->get_shape_container();

    for (; entry_iter != end; entry_iter++)
    {
        if (input.m_compound_filter->allows_collision(input, agent_body_a, agent_body_b, *compound_shape, *entry_iter))
        {
            phys_shape_buffer shape_scratch_buffer;
            const phys_shape *shape = compound_shape->get_child_shape(*entry_iter, shape_scratch_buffer);
            modified_proxy_b.set_proxy_shape(shape, *entry_iter);
            phys_shape_type shape_type_b = shape->get_type_id();
            phys_linear_cast_func lin_cast_fn = input.m_agent_dispatcher->get_linear_cast_fn(shape_type_a, shape_type_b);
            lin_cast_fn(agent_body_a, modified_proxy_b, input, collector, start_collector);
        }
    }

}

void phys_bvh_agent::static_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_aabb aabb;
    PHYS_STATIC_COMPUTE_BOUNDS(agent_body_a, agent_body_b, input, aabb);

    const phys_bvh_shape *bvh_shape_b = (const phys_bvh_shape *)agent_body_b.get_shape();
    phys_inline_array<phys_shape_key, 128> hit_key_list;
    bvh_shape_b->query_aabb(aabb, hit_key_list);

    phys_shape_type shape_type_a = agent_body_a.get_shape()->get_type_id();
    phys_shape_key *entry_iter = hit_key_list.begin();
    phys_shape_key *end = hit_key_list.end();
    phys_collision_proxy modified_proxy_b;
    PHYS_PROXY_INIT_CHILD(modified_proxy_b, &agent_body_b);
    const phys_shape_container *compound_shape = bvh_shape_b->get_shape_container();

    for (; entry_iter != end; entry_iter++)
    {
        if (input.m_compound_filter->allows_collision(input, agent_body_a, agent_body_b, *compound_shape, *entry_iter))
        {
            phys_shape_buffer shape_scratch_buffer;
            const phys_shape *shape = compound_shape->get_child_shape(*entry_iter, shape_scratch_buffer);
            modified_proxy_b.set_proxy_shape(shape, *entry_iter);
            phys_shape_type shape_type_b = shape->get_type_id();
            phys_get_closest_points_func func = input.m_agent_dispatcher->get_closest_points_fn(shape_type_a, shape_type_b);
            func(agent_body_a, modified_proxy_b, input, collector);
        }
    }

}

void phys_bvh_agent::static_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_aabb aabb;
    PHYS_STATIC_COMPUTE_BOUNDS(agent_body_a, agent_body_b, input, aabb);

    const phys_bvh_shape *bvh_shape_b = (const phys_bvh_shape *)agent_body_b.get_shape();
    phys_inline_array<phys_shape_key, 128> hit_key_list;
    bvh_shape_b->query_aabb(aabb, hit_key_list);

    phys_shape_type shape_type_a = agent_body_a.get_shape()->get_type_id();
    phys_shape_key *entry_iter = hit_key_list.begin();
    phys_shape_key *end = hit_key_list.end();
    phys_collision_proxy modified_proxy_b;
    PHYS_PROXY_INIT_CHILD(modified_proxy_b, &agent_body_b);
    const phys_shape_container *compound_shape = bvh_shape_b->get_shape_container();

    for (; entry_iter != end; entry_iter++)
    {
        if (input.m_compound_filter->allows_collision(input, agent_body_a, agent_body_b, *compound_shape, *entry_iter))
        {
            phys_shape_buffer shape_scratch_buffer;
            const phys_shape *shape = compound_shape->get_child_shape(*entry_iter, shape_scratch_buffer);
            modified_proxy_b.set_proxy_shape(shape, *entry_iter);
            phys_shape_type shape_type_b = shape->get_type_id();
            phys_get_penetrations_func func = input.m_agent_dispatcher->get_penetrations_fn(shape_type_a, shape_type_b);
            func(agent_body_a, modified_proxy_b, input, collector);
            if (collector.get_early_out())
            {
                break;
            }
        }
    }

}

phys_agent *phys_bvh_agent::create_bvh_shape_agent(const phys_collision_proxy &, const phys_collision_proxy &,
    const phys_collision_input &, phys_contact_manager *pair_contact_manager)
{
    return new phys_symmetric_agent<phys_bvh_agent>(pair_contact_manager);
}

phys_agent *phys_bvh_agent::create_bvh_bvh_agent(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &, phys_contact_manager *pair_contact_manager)
{
    float radius_a = static_cast<const phys_motion_state *>(agent_body_a.m_motion)->m_object_radius;
    float radius_b = static_cast<const phys_motion_state *>(agent_body_b.m_motion)->m_object_radius;
    if (radius_a < radius_b)
    {
        return new phys_bvh_agent(pair_contact_manager);
    }
    else
    {
        return new phys_symmetric_agent<phys_bvh_agent>(pair_contact_manager);
    }
}

void phys_bvh_agent::sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input,
    phys_hit_collector &collector, phys_hit_collector *start_collector)
{
    static_sweep_cast(agent_body_a, agent_body_b, input, collector, start_collector);
}

void phys_bvh_agent::get_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_hit_collector &collector)
{
    static_closest_points(agent_body_a, agent_body_b, input, collector);
}

void phys_bvh_agent::get_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    static_penetrations(agent_body_a, agent_body_b, input, collector);
}

void phys_bvh_agent::register_agent(phys_dispatcher *dispatcher)
{
    phys_dispatcher::agent_funcs forward;
    forward.m_create_fn = create_bvh_shape_agent;
    forward.m_penetrations_fn = phys_symmetric_agent_linear_cast<phys_bvh_agent>::static_penetrations;
    forward.m_closest_points_fn = phys_symmetric_agent_linear_cast<phys_bvh_agent>::static_closest_points;
    forward.m_linear_cast_func = phys_symmetric_agent_linear_cast<phys_bvh_agent>::static_sweep_cast;
    forward.m_flipped = true;
    forward.m_predictive = true;
    dispatcher->register_collision_agent(forward, PHYS_SHAPE_BVH, (phys_shape_type)-1);

    phys_dispatcher::agent_funcs backward;
    backward.m_create_fn = create_shape_bounding_agent;
    backward.m_penetrations_fn = static_penetrations;
    backward.m_closest_points_fn = static_closest_points;
    backward.m_linear_cast_func = static_sweep_cast;
    backward.m_flipped = false;
    backward.m_predictive = true;
    dispatcher->register_collision_agent(backward, (phys_shape_type)-1, PHYS_SHAPE_BVH);

    phys_dispatcher::agent_funcs tree_tree_funcs;
    tree_tree_funcs.m_create_fn = create_bvh_bvh_agent;
    tree_tree_funcs.m_penetrations_fn = static_penetrations;
    tree_tree_funcs.m_closest_points_fn = static_closest_points;
    tree_tree_funcs.m_linear_cast_func = static_sweep_cast;
    tree_tree_funcs.m_flipped = false;
    tree_tree_funcs.m_predictive = true;
    dispatcher->register_collision_agent(tree_tree_funcs, PHYS_SHAPE_BVH, PHYS_SHAPE_BVH);
}

void phys_symmetric_flip_collector::add_volume_callback_shape(const phys_collision_proxy &body)
{
    m_collector.add_volume_callback_shape(body);
}

// @address 0x61d5b0 ?compute_sweep_cast_bounds@phys_bvh_agent@@KAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_aabb@@@Z
void phys_bvh_agent::compute_sweep_cast_bounds(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input, phys_aabb &aabb_out)
{
    phys_transform b_from_a;
    const phys_transform &world_from_b = agent_body_b.get_transform();
    const phys_transform &world_from_a = agent_body_a.get_transform();
    b_from_a.set_mul_inverse_mul(world_from_b, world_from_a);
    agent_body_a.get_shape()->get_handle_aabb(b_from_a, input.m_box_tolerance, aabb_out);

    phys_vec4 bvh_path_b;
    bvh_path_b.vec_set_rotated_inverse_dir(agent_body_b.get_transform().get_rotation(), input.m_path);
    phys_vec4 zero;
    zero.set_zero4();
    phys_vec4 path_min;
    path_min.set_min4(zero, bvh_path_b);
    phys_vec4 path_max;
    path_max.set_max4(zero, bvh_path_b);
    aabb_out.m_min_limit.add4(path_min);
    aabb_out.m_max_limit.add4(path_max);
}

phys_bvh_agent::~phys_bvh_agent()
{
}

template <>
void phys_symmetric_agent_linear_cast<phys_bvh_agent>::static_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_proxy_pair_collector &collector)
{
    phys_symmetric_flip_pair_collector flipped_collector(collector);
    phys_bvh_agent::static_penetrations(agent_body_b, agent_body_a, input, flipped_collector);
}

template <>
void phys_symmetric_agent_linear_cast<phys_bvh_agent>::static_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_symmetric_flip_collector flipped_collector(collector);
    phys_bvh_agent::static_closest_points(agent_body_b, agent_body_a, input, flipped_collector);
}

template <>
void phys_symmetric_agent_linear_cast<phys_bvh_agent>::static_sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input, phys_hit_collector &collector, phys_hit_collector *start_collector)
{
    phys_linear_cast_collision_input flipped_input = input;
    flipped_input.m_path.set_neg4(input.m_path);
    phys_symmetric_flip_cast_collector flipped_collector(input.m_path, collector);
    if (start_collector)
    {
        phys_symmetric_flip_cast_collector flipped_start_collector(input.m_path, *start_collector);
        phys_bvh_agent::static_sweep_cast(agent_body_b, agent_body_a, flipped_input, flipped_collector, &flipped_start_collector);
    }
    else
    {
        phys_bvh_agent::static_sweep_cast(agent_body_b, agent_body_a, flipped_input, flipped_collector, 0);
    }
}

template <>
void phys_symmetric_agent_linear_cast<phys_bvh_agent>::process_collision(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_process_collision_input &input, phys_process_collision_output &output)
{
    phys_process_hit_point *point = output.m_next_free_contact;
    float old_toi = output.m_collision_toi.m_time;
    phys_bvh_agent::process_collision(agent_body_b, agent_body_a, input, output);
    for (; point < output.m_next_free_contact; ++point)
    {
        point->m_hit_contact.set_flipped_contact(point->m_hit_contact);
    }
    if (old_toi != output.m_collision_toi.m_time)
    {
        output.m_collision_toi.flip();
    }
}

template <>
void phys_symmetric_agent_linear_cast<phys_bvh_agent>::get_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_symmetric_flip_collector flipped_collector(collector);
    phys_bvh_agent::get_closest_points(agent_body_b, agent_body_a, input, flipped_collector);
}

template <>
void phys_symmetric_agent<phys_bvh_agent>::sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input, phys_hit_collector &collector, phys_hit_collector *start_collector)
{
    phys_linear_cast_collision_input flipped_input = input;
    flipped_input.m_path.set_neg4(input.m_path);
    phys_symmetric_flip_cast_collector flipped_collector(input.m_path, collector);
    if (start_collector)
    {
        phys_symmetric_flip_cast_collector flipped_start_collector(input.m_path, *start_collector);
        phys_bvh_agent::sweep_cast(agent_body_b, agent_body_a, flipped_input, flipped_collector, &flipped_start_collector);
    }
    else
    {
        phys_bvh_agent::sweep_cast(agent_body_b, agent_body_a, flipped_input, flipped_collector, 0);
    }
}

template <>
void phys_symmetric_agent_linear_cast<phys_bvh_agent>::update_compound_filter(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_constraint_owner &owner)
{
  phys_bvh_agent::update_compound_filter(agent_body_b, agent_body_a, input, owner);
}

// @address 0x61d8e8 ?process_collision@?$phys_symmetric_agent_linear_cast@Vphys_bvh_tree_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_process_collision_input@@AAUphys_process_collision_output@@@Z
template <>
void phys_symmetric_agent_linear_cast<phys_bvh_tree_agent>::process_collision(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b,
    const phys_process_collision_input &input, phys_process_collision_output &output)
{
    phys_process_hit_point *point = output.m_next_free_contact;
    float old_toi = output.m_collision_toi.m_time;
    phys_bvh_agent::process_collision(agent_body_b, agent_body_a, input, output);
    for (; point < output.m_next_free_contact; ++point)
    {
        point->m_hit_contact.set_flipped_contact(point->m_hit_contact);
    }
    if (old_toi != output.m_collision_toi.m_time)
    {
        output.m_collision_toi.flip();
    }
}

// @address 0x628a10 ?get_closest_points@?$phys_symmetric_agent_linear_cast@Vphys_bvh_stream_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_hit_collector@@@Z
template <>
void phys_symmetric_agent_linear_cast<phys_bvh_stream_agent>::get_closest_points(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_hit_collector &collector)
{
    phys_symmetric_flip_collector flipped_collector(collector);
    phys_bvh_agent::static_closest_points(agent_body_b, agent_body_a, input, flipped_collector);
}

// @address 0x628bc8 ?sweep_cast@?$phys_symmetric_agent@Vphys_bvh_stream_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_linear_cast_collision_input@@AAVphys_hit_collector@@PAV4@@Z
template <>
void phys_symmetric_agent<phys_bvh_stream_agent>::sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
    const phys_linear_cast_collision_input &input, phys_hit_collector &collector, phys_hit_collector *start_collector)
{
    phys_linear_cast_collision_input flipped_input = input;
    flipped_input.m_path.set_neg4(input.m_path);
    phys_symmetric_flip_cast_collector flipped_collector(input.m_path, collector);
    if (start_collector)
    {
        phys_symmetric_flip_cast_collector flipped_start_collector(input.m_path, *start_collector);
        phys_bvh_agent::static_sweep_cast(agent_body_b, agent_body_a, flipped_input, flipped_collector, &flipped_start_collector);
    }
    else
    {
        phys_bvh_agent::static_sweep_cast(agent_body_b, agent_body_a, flipped_input, flipped_collector, 0);
    }
}

// @address 0x637448 ?update_compound_filter@?$phys_symmetric_agent_linear_cast@Vphys_bvh_tree_agent@@@@UAAXABVphys_collision_proxy@@0ABUphys_collision_input@@AAVphys_constraint_owner@@@Z
template <>
void phys_symmetric_agent_linear_cast<phys_bvh_tree_agent>::update_compound_filter(const phys_collision_proxy &agent_body_a,
    const phys_collision_proxy &agent_body_b,
    const phys_collision_input &input, phys_constraint_owner &owner)
{
    phys_bvh_agent::update_compound_filter(agent_body_b, agent_body_a, input, owner);
}

void phys_process_collision_data::toi_info::flip()
{
    phys_vec4 &norm = m_contact_point.m_separating_normal;
    norm.set_neg3(norm);
    m_simplex_cache.flip();
}

