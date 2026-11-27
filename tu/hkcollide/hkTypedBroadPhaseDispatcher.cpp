// hkcollide/hkTypedBroadPhaseDispatcher.obj: from the records 0x704cd0 0x704cf8 0x704d00 0x704dc0 0x704e38 0x704ef0
// @flags /TP /O2 /Oc
// @address 0x704ef0 ?remove_duplicate_pairs@phys_typed_broadphase_dispatcher@@SAXAAV?$phys_array@Vphys_broadphase_handle_pair@@@@0@Z
#include "phys/phys_base_types.h"
// @address 0x704dc0 ?remove_handle_pairs@phys_typed_broadphase_dispatcher@@QBAXPAVphys_typed_broadphase_pair@@H@Z
// @address 0x704e38 ??0phys_typed_broadphase_dispatcher@@QAA@XZ
// @address 0x704cd0 ??1phys_typed_broadphase_dispatcher@@QAA@XZ
// @address 0x704cf8 ?key_from_pair@@YA_KABVphys_broadphase_handle_pair@@@Z
// @address 0x704d00 ?add_handle_pairs@phys_typed_broadphase_dispatcher@@QBAXPAVphys_typed_broadphase_pair@@HPBVphys_collider_pair_filter@@@Z
#include "phys/phys_collide.h"
#include "phys/phys_local_buffer.h"

#include "phys/phys_thread_memory.h"
typedef unsigned __int64 phys_u64;
typedef __int64 phys_i64;
typedef unsigned int phys_u32;
typedef int phys_i32;
#include "phys/phys_broadphase_listener.h"
#include "phys/phys_null_broadphase_listener.h"
template <typename T> class phys_array;
#include "phys/phys_typed_broadphase_handle_pair.h"
#include "phys/phys_typed_broadphase_dispatcher.h"
namespace phys_math {
    }
#include "phys/phys_algorithm.h"
#include "phys/phys_ptr_map_base.h"
#include "phys/phys_memory.h"
#include "phys/phys_statistics_collector.h"

phys_typed_broadphase_dispatcher::~phys_typed_broadphase_dispatcher()
{
    phys_broadphase_listener *listener = m_null_listener;
    if (listener)
        delete listener;
}

unsigned __int64 key_from_pair(const phys_broadphase_handle_pair &pair)
{
    return *(const unsigned __int64 *)&pair;
}


void phys_typed_broadphase_dispatcher::add_handle_pairs(phys_typed_broadphase_pair *pairs, int count, const phys_collider_pair_filter *filter) const
{
    while (--count >= 0) {
        const phys_collider *pair_collider_a =
            static_cast<const phys_collider *>(static_cast<const phys_typed_broadphase_handle *>(pairs->m_a)->get_collider_owner());
        const phys_collider *pair_collider_b =
            static_cast<const phys_collider *>(static_cast<const phys_typed_broadphase_handle *>(pairs->m_b)->get_collider_owner());
        if (filter->allows_collision(*pair_collider_a, *pair_collider_b)) {
            int shape_type_a = pairs->get_element_a()->get_type_id();
            int shape_type_b = pairs->get_element_b()->get_type_id();
            m_pair_listeners[shape_type_a][shape_type_b]->add_collision_pair(*pairs);
        }
        ++pairs;
    }
}

void phys_typed_broadphase_dispatcher::remove_handle_pairs(phys_typed_broadphase_pair *pairs, int count) const
{
    while (--count >= 0) {
        int shape_type_a = pairs->get_element_a()->get_type_id();
        int shape_type_b = pairs->get_element_b()->get_type_id();
        m_pair_listeners[shape_type_a][shape_type_b]->remove_collision_pair(*pairs);
        ++pairs;
    }
}

phys_typed_broadphase_dispatcher::phys_typed_broadphase_dispatcher()
{
    m_null_listener = new phys_null_broadphase_listener;
    for (int shape_type_a = 0; shape_type_a < 8; ++shape_type_a) {
        for (int shape_type_b = 0; shape_type_b < 8; ++shape_type_b)
            m_pair_listeners[shape_type_a][shape_type_b] = m_null_listener;
    }
}

#define pair_pos_of(value) (phys_u32(value) >> 8)
#define pair_count_of(value) (0xff & int(value))
#define pack_pos_count(position, count) phys_u64((position << 8) | count)

// @address 0x704ef0
void phys_typed_broadphase_dispatcher::remove_duplicate_pairs(phys_array<phys_broadphase_handle_pair> &new_pairs,
    phys_array<phys_broadphase_handle_pair> &sweep_removed_pairs)
{
    int min = phys_math::min_of_two(new_pairs.getSize(), sweep_removed_pairs.getSize());

    if (min < 32)
    {
        int write_idx = 0;
        for (int d = 0; d < sweep_removed_pairs.getSize(); d++)
        {
            int n = 0;
            for (; n < new_pairs.getSize(); n++)
            {
                if ((new_pairs[n].m_a == sweep_removed_pairs[d].m_a && new_pairs[n].m_b == sweep_removed_pairs[d].m_b)
                    || (new_pairs[n].m_b == sweep_removed_pairs[d].m_a && new_pairs[n].m_a == sweep_removed_pairs[d].m_b))
                {
                    break;
                }
            }

            if (n == new_pairs.getSize())
            {
                if (write_idx != d)
                {
                    sweep_removed_pairs[write_idx] = sweep_removed_pairs[d];
                }
                write_idx++;
            }
            else
            {
                new_pairs.remove_at_and_copy(n);
            }
        }
        sweep_removed_pairs.set_array_size(write_idx);
        return;
    }

    {
        int map_buf_bytes = phys_ptr_map_base<phys_u64>::get_size_in_bytes_for(new_pairs.getSize());
        phys_local_buffer<char> buffer(map_buf_bytes);
        phys_ptr_map_base<phys_u64> new_pairs_map(buffer.begin(), map_buf_bytes);

        {
            for (int n = 0; n < new_pairs.getSize(); n++)
            {
                phys_broadphase_handle_pair pair = new_pairs[n];
                if (pair.m_a > pair.m_b)
                {
                    phys_algorithm::swap(pair.m_a, pair.m_b);
                }
                phys_u64 key = key_from_pair(pair);
                phys_ptr_map_base<phys_u64>::Iterator it = new_pairs_map.map_find_key(key);
                if (new_pairs_map.is_valid_aabb(it))
                {
                    phys_u64 value = new_pairs_map.get_map_value(it);
                    value++;
                    new_pairs_map.set_map_value(it, value);
                    new_pairs[n].m_a = 0;
                }
                else
                {
                    phys_i64 value = pack_pos_count(n, 1);
                    new_pairs_map.insert(key, value);
                }
            }
        }

        {
            int write_idx = 0;
            for (int d = 0; d < sweep_removed_pairs.getSize(); d++)
            {
                phys_broadphase_handle_pair pair = sweep_removed_pairs[d];
                if (pair.m_a > pair.m_b)
                {
                    phys_algorithm::swap(pair.m_a, pair.m_b);
                }

                phys_ptr_map_base<phys_u64>::Iterator it = new_pairs_map.map_find_key(key_from_pair(pair));

                if (new_pairs_map.is_valid_aabb(it))
                {
                    phys_u64 value = new_pairs_map.get_map_value(it);
                    phys_i32 count = pair_count_of(value);
                    if (count > 1)
                    {
                        value--;
                        new_pairs_map.set_map_value(it, value);
                    }
                    else
                    {
                        new_pairs_map.remove(it);
                        phys_i32 n = pair_pos_of(value);
                        new_pairs[n].m_a = 0;
                    }
                }
                else
                {
                    if (write_idx != d)
                    {
                        sweep_removed_pairs[write_idx] = sweep_removed_pairs[d];
                    }
                    write_idx++;
                }
            }
            sweep_removed_pairs.set_array_size(write_idx);
        }

        {
            int next_null_slot = 0;
            for (int i = 0; i < new_pairs.getSize(); i++)
            {
                if (new_pairs[i].m_a != 0)
                {
                    new_pairs[next_null_slot++] = new_pairs[i];
                }
            }
            new_pairs.set_array_size(next_null_slot);
        }
    }
}

