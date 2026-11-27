// hkinternal/hkAgent1nMachine.obj: from the records 0x690558 0x690698 0x690808 0x6908a8 0x6909d8 0x690b00 0x691168 0x6915d8 0x691730 0x691740
// @compiler xdk5632
// @flags /TP /O2 /Oc
// @address 0x691730 phys_batch_agent_update_compound_filter
#include "phys/phys_base_types.h"
// @address 0x690808 phys_batch_agent_find
#include <vectorintrinsics.h>
#include "physics/havok_memory.h"
#include "phys/phys_batch_agent_machine.h"
#include "phys/phys_math.h"
#include "phys/phys_thread_memory.h"
#include "phys/phys_base.h"
#include "phys/phys_batch_agent_visitor_input.h"
#include "phys/phys_batch_agent_track.h"
#include "phys/phys_convex_penetration_util.h"
#include "phys/phys_collide.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_collision_proxy.h"


enum phys_contact_pair_cmp
{
    PHYS_CONTACT_PAIR_OK = 0,
    PHYS_CONTACT_PAIR_REJECT = 1,
    PHYS_CONTACT_PAIR_CONFLICT = 2
};


phys_batch_agent_entry *phys_batch_agent_find(phys_batch_agent_track &pair_agent_track, phys_shape_key collision_partner, void **agent_data_out)
{
    phys_batch_agent_sector **next_sector_ptr = pair_agent_track.m_track_sectors.m_data;
    phys_batch_agent_sector *sector = *next_sector_ptr++;
    for (;;) {
        phys_batch_agent_entry *entry = static_cast<phys_batch_agent_entry *>(sector->get_track_begin());
        phys_batch_agent_entry *sector_end = static_cast<phys_batch_agent_entry *>(sector->get_track_end());
        do {
            void *agent_data;
            switch (entry->m_stream_command) {
            case 2: case 3: case 6:
                agent_data = phys_add_byte_offset(entry, 16);
                if (entry->m_shape_key == collision_partner) { *agent_data_out = agent_data; return entry; }
                entry = phys_add_byte_offset(entry, entry->m_size);
                break;
            case 4: case 5:
                agent_data = phys_add_byte_offset(entry, 32);
                if (entry->m_shape_key == collision_partner) { *agent_data_out = agent_data; return entry; }
            case 0:
                entry = phys_add_byte_offset(entry, entry->m_size);
                break;
            case 1:
                return 0;
            }
        } while (entry < sector_end);
        sector = *next_sector_ptr++;
    }
}


extern "C" void phys_batch_agent_update_compound_filter(phys_batch_agent_track &track,
    phys_batch_agent_visitor_input &input)
{
    phys_batch_agent_visit_all(track, input, phys_batch_agent_compound_filter_visitor);
}



// @address 0x6909d8 ?contact_point_is_valid@@YA?AW4phys_contact_pair_cmp@@MABVphys_contact_point@@ABUphys_collision_quality_info@@MABUphys_process_hit_point@@@Z
__forceinline phys_contact_pair_cmp contact_point_is_valid(float radius_a, const phys_contact_point &check_point, const phys_collision_quality_info &qi,
    float radius_b, const phys_process_hit_point &ref_hit_point)
{
    const phys_contact_point &ref_contact = ref_hit_point.m_hit_contact;
    __vector4 diff = __vsubfp(ref_contact.m_position.m_simd_quad, check_point.m_position.m_simd_quad);
    float check_dot;
    float ref_dot;
    float normals_dot;
    __stvewx(__vmsum3fp(check_point.m_separating_normal.m_simd_quad, diff), &check_dot, 0);
    __stvewx(__vmsum3fp(ref_contact.m_separating_normal.m_simd_quad, diff), &ref_dot, 0);
    __stvewx(__vmsum3fp(check_point.m_separating_normal.m_simd_quad, ref_contact.m_separating_normal.m_simd_quad), &normals_dot, 0);
    float n0_n1_dot = normals_dot;
    float d0 = -ref_dot;
    d0 = d0 + radius_a * (n0_n1_dot - 1.0f);
    float d1 = check_dot;
    d1 = d1 + radius_a * (n0_n1_dot - 1.0f);
    float capsule_radius_sum = radius_a + radius_b;
    if (d0 > capsule_radius_sum) {
        return PHYS_CONTACT_PAIR_OK;
    }
    if (n0_n1_dot > 0.9999f && ref_hit_point.m_process_contact_id == 0xffff) {
        return PHYS_CONTACT_PAIR_OK;
    }
    float point_dist0 = check_point.m_separating_normal(3);
    float point_dist1 = ref_contact.m_separating_normal(3);
    if (ref_hit_point.m_process_contact_id == 0xffff && n0_n1_dot < 0.8f && d0 + d1 < 0.1f && point_dist0 + point_dist1 < -capsule_radius_sum * 2.0f) {
        return PHYS_CONTACT_PAIR_CONFLICT;
    }
    float min_accept_dist_delta = 0.0f;
    if (ref_hit_point.m_process_contact_id != 0xffff) {
        min_accept_dist_delta = qi.m_keep_contact * 0.05f;
    }
    if (point_dist0 <= point_dist1 - min_accept_dist_delta) {
        return PHYS_CONTACT_PAIR_OK;
    }
    return PHYS_CONTACT_PAIR_REJECT;
}














// @address 0x6915d8 ?phys_batch_agent_compound_filter_visitor@@YAPAXAAUphys_batch_agent_visitor_input@@PAUphys_batch_agent_entry@@PAX@Z
void *phys_batch_agent_compound_filter_visitor(phys_batch_agent_visitor_input &visitor_input, phys_batch_agent_entry *entry, void *agent_data)
{
    const phys_collision_proxy *agent_body_a = visitor_input.m_body_a;
    unsigned int entry_key_b = entry->m_shape_key;
    if (visitor_input.m_input->m_compound_filter->allows_collision(*visitor_input.m_input, *agent_body_a,
        *visitor_input.m_compound_proxy_b, *visitor_input.m_container_shape_b, entry_key_b)) {
        phys_narrow_agent::agent_update_filter_fn func = visitor_input.m_input->m_agent_dispatcher->get_narrow_agent_update_filter_fn(entry->m_agent_type);
        if (func) {
            __declspec(align(16)) char shape_scratch_buffer[512];
            const phys_shape *penetration_shape_b = visitor_input.m_container_shape_b->get_child_shape(entry_key_b, shape_scratch_buffer);
            phys_collision_proxy child_proxy_b(visitor_input.m_compound_proxy_b);
            child_proxy_b.set_proxy_shape(penetration_shape_b, entry_key_b);
            phys_bool flipped = entry->m_stream_command == 3 || entry->m_stream_command == 5;
            if (!flipped) {
                func(entry, agent_data, const_cast<phys_collision_proxy &>(*visitor_input.m_body_a), child_proxy_b,
                    *visitor_input.m_input, visitor_input.m_contact_manager, *visitor_input.m_constraint_owner);
            } else {
                func(entry, agent_data, child_proxy_b, const_cast<phys_collision_proxy &>(*visitor_input.m_body_a),
                    *visitor_input.m_input, visitor_input.m_contact_manager, *visitor_input.m_constraint_owner);
            }
        }
        return phys_add_byte_offset(entry, entry->m_size);
    }
    visitor_input.m_input->m_agent_dispatcher->get_narrow_agent_destroy_fn(entry->m_agent_type)(entry, agent_data,
        visitor_input.m_contact_manager, *visitor_input.m_constraint_owner, visitor_input.m_input->m_agent_dispatcher);
    return entry;
}








// @address 0x690698 phys_batch_agent_warp_time
extern "C" void phys_batch_agent_warp_time(phys_batch_agent_track *pair_agent_track, float agent_old_time, float agent_new_time, phys_collision_input *input)
{
    phys_batch_agent_sector *read_sector = pair_agent_track->m_track_sectors[0];
    char *sector_read_data = (char *)read_sector->get_track_begin();
    char *sector_read_end = (char *)read_sector->get_track_end();
    int next_read_sector = 1;
    while (1)
    {
        phys_batch_agent_entry *entry = (phys_batch_agent_entry *)sector_read_data;
        int command = entry->m_stream_command;
        switch (command)
        {
        case 2: case 3: case 6:
        {
            void *agent_data = sector_read_data + 16;
            sector_read_data = sector_read_data + entry->m_size;
            phys_narrow_agent::agent_warp_time_fn func = input->m_agent_dispatcher->get_narrow_agent_warp_time_fn(entry->m_agent_type);
            if (func)
            {
                func(entry, agent_data, agent_old_time, agent_new_time, *input);
            }
            break;
        }
        case 4: case 5:
        {
            void *agent_data = sector_read_data + 32;
            phys_batch_agent_sweep_entry *sweep_entry = (phys_batch_agent_sweep_entry *)sector_read_data;
            if (sweep_entry->m_separating_normal_time == agent_old_time)
            {
                sweep_entry->m_separating_normal_time = agent_new_time;
            }
            else
            {
                sweep_entry->m_separating_normal_time = -1.0f;
            }
            sector_read_data = sector_read_data + entry->m_size;
            phys_narrow_agent::agent_warp_time_fn func = input->m_agent_dispatcher->get_narrow_agent_warp_time_fn(entry->m_agent_type);
            if (func)
            {
                func(entry, agent_data, agent_old_time, agent_new_time, *input);
            }
            break;
        }
        case 0:
        {
            sector_read_data = sector_read_data + entry->m_size;
            break;
        }
        case 1:
        {
            return;
        }
        default:
            break;
        }
        if (sector_read_data >= sector_read_end)
        {
            read_sector = pair_agent_track->m_track_sectors[next_read_sector];
            sector_read_data = (char *)read_sector->get_track_begin();
            sector_read_end = (char *)read_sector->get_track_end();
            next_read_sector++;
        }
    }
}

// @address 0x690558 phys_batch_agent_invalidate_sweep
extern "C" void phys_batch_agent_invalidate_sweep(phys_batch_agent_track *pair_agent_track, phys_collision_input *input)
{
    phys_batch_agent_sector *read_sector = pair_agent_track->m_track_sectors[0];
    char *sector_read_data = (char *)read_sector->get_track_begin();
    char *sector_read_end = (char *)read_sector->get_track_end();
    int next_read_sector = 1;
    while (1)
    {
        phys_batch_agent_entry *entry = (phys_batch_agent_entry *)sector_read_data;
        int command = entry->m_stream_command;
        switch (command)
        {
        case 2: case 3: case 6:
        {
            void *agent_data = sector_read_data + 16;
            sector_read_data = sector_read_data + entry->m_size;
            phys_narrow_agent::agent_invalidate_sweep_fn func = input->m_agent_dispatcher->get_narrow_agent_invalidate_sweep_fn(entry->m_agent_type);
            if (func)
            {
                func(entry, agent_data, *input);
            }
            break;
        }
        case 4: case 5:
        {
            void *agent_data = sector_read_data + 32;
            phys_batch_agent_sweep_entry *sweep_entry = (phys_batch_agent_sweep_entry *)sector_read_data;
            sweep_entry->m_separating_normal_time = -1.0f;
            sweep_entry->m_separating_normal.set_zero4();
            sector_read_data = sector_read_data + entry->m_size;
            phys_narrow_agent::agent_invalidate_sweep_fn func = input->m_agent_dispatcher->get_narrow_agent_invalidate_sweep_fn(entry->m_agent_type);
            if (func)
            {
                func(entry, agent_data, *input);
            }
            break;
        }
        case 0:
        {
            sector_read_data = sector_read_data + entry->m_size;
            break;
        }
        case 1:
        {
            return;
        }
        default:
            break;
        }
        if (sector_read_data >= sector_read_end)
        {
            read_sector = pair_agent_track->m_track_sectors[next_read_sector];
            sector_read_data = (char *)read_sector->get_track_begin();
            sector_read_end = (char *)read_sector->get_track_end();
            next_read_sector++;
        }
    }
}

// @address 0x6908a8 phys_batch_agent_create
extern "C" void phys_batch_agent_create(void *data)
{
    phys_array<phys_batch_agent_sector *> &sectors = static_cast<phys_batch_agent_track *>(data)->m_track_sectors;
    sectors.m_size = 0;
    const int capacity = sectors.m_capacity_and_flags & 0x3fffffff;
    if (capacity < 4) {
        int doubled_cap = 2 * capacity;
        int array_new_size = (4 < doubled_cap) ? doubled_cap : 4;
        phys_array_util::reserve_array_storage(data, array_new_size, (int)sizeof(phys_batch_agent_sector *));
    }
    if (sectors.m_size == (sectors.m_capacity_and_flags & 0x3fffffff)) {
        phys_array_util::grow_array_storage(data, (int)sizeof(phys_batch_agent_sector *));
    }
    sectors.m_size = sectors.m_size + 1;

    phys_batch_agent_sector *sector = new phys_batch_agent_sector;
    sectors.m_data[0] = sector;

    phys_batch_agent_entry *entry = (phys_batch_agent_entry *)sector->get_track_begin();
    entry->m_size = 16;
    entry->m_stream_command = 1;
    entry->m_shape_key = 0xffffffff;
    unsigned int *marker = reinterpret_cast<unsigned int *>(entry + 1);
    marker[0] = 0xabcdabcd;
    marker[1] = 0xabcdabcd;
    sector->m_bytes_allocated = 16;
}

static __forceinline phys_thread_memory &phys_batch_agent_thread_memory()
{
    phys_thread_memory *instance = phys_thread_memory::get_instance_ptr();
    return *instance;
}



// @address 0x691168 ?phys_batch_agent_visit_all@@YAXAAUphys_batch_agent_track@@AAUphys_batch_agent_visitor_input@@P6APAX1PAUphys_batch_agent_entry@@PAX@Z@Z
void phys_batch_agent_visit_all(phys_batch_agent_track &pair_agent_track, phys_batch_agent_visitor_input &visitor_input,
    void *(*visitor)(phys_batch_agent_visitor_input &, phys_batch_agent_entry *, void *))
{
    phys_batch_agent_sector *read_sector = pair_agent_track.m_track_sectors[0];
    void *sector_read_data = read_sector->get_track_begin();
    void *sector_read_end = read_sector->get_track_end();
    int next_read_sector = 1;
    int cur_sector_idx = 0;
    phys_batch_agent_sector *spare_sector = 0;
    phys_batch_agent_sector *cur_sector = new phys_batch_agent_sector;
    void *cur_body_desc = cur_sector->get_track_begin();

    while (1)
    {
        {
            phys_batch_agent_entry *entry = (phys_batch_agent_entry *)sector_read_data;
            phys_string::mem_copy16(cur_body_desc, sector_read_data, entry->m_size >> 4);
            sector_read_data = phys_add_byte_offset(sector_read_data, entry->m_size);
        }

        phys_batch_agent_visitor_input visitor_input_copy = visitor_input;
        {
            int command = ((phys_batch_agent_entry *)cur_body_desc)->m_stream_command;
            int entry_bytes = 16;
            switch (command)
            {
            case 4:
            case 5:
                entry_bytes = 32;
            case 2:
            case 3:
            case 6:
            {
                phys_batch_agent_entry *entry = (phys_batch_agent_entry *)cur_body_desc;
                void *agent_data = phys_add_byte_offset(entry, entry_bytes);
                cur_body_desc = visitor(visitor_input_copy, entry, agent_data);
                break;
            }
            case 0:
                break;
            case 1:
                cur_body_desc = phys_add_byte_offset(cur_body_desc, 16);
                break;
            default:
                break;
            }
        }

        if (sector_read_data == sector_read_end)
        {
            if (spare_sector) {
                delete spare_sector;
            }
            spare_sector = read_sector;
            if (next_read_sector < pair_agent_track.m_track_sectors.getSize()) {
                read_sector = pair_agent_track.m_track_sectors[next_read_sector];
                sector_read_data = read_sector->get_track_begin();
                sector_read_end = read_sector->get_track_end();
                next_read_sector++;
            } else {
                break;
            }
        }

        {
            int next_entry_size = ((phys_batch_agent_entry *)sector_read_data)->m_size;
            int bytes_used = int(phys_get_byte_offset(cur_sector->get_track_begin(), cur_body_desc));
            if (next_entry_size + bytes_used <= cur_sector->get_sector_capacity() - 80) {
                continue;
            }
            cur_sector->m_bytes_allocated = bytes_used;
            if (cur_sector_idx >= next_read_sector) {
                const int new_sector_count = 1 + pair_agent_track.m_track_sectors.m_size;
                const int sectors_to_move = pair_agent_track.m_track_sectors.m_size - next_read_sector;
                if (new_sector_count > pair_agent_track.m_track_sectors.get_sector_capacity()) {
                    pair_agent_track.m_track_sectors.reserve(new_sector_count);
                }
                phys_array<phys_batch_agent_sector *>::copy_backwards(
                    pair_agent_track.m_track_sectors.m_data + next_read_sector + 1,
                        pair_agent_track.m_track_sectors.m_data + next_read_sector, sectors_to_move);
                pair_agent_track.m_track_sectors.m_size = new_sector_count;
                next_read_sector++;
            }
            pair_agent_track.m_track_sectors[cur_sector_idx] = cur_sector;
            cur_sector_idx++;
            if (spare_sector) {
                cur_sector = spare_sector;
                spare_sector = 0;
            } else {
                cur_sector = new phys_batch_agent_sector;
            }
            cur_body_desc = cur_sector->get_track_begin();
        }
    }

    int bytes_used = int(phys_get_byte_offset(cur_sector->get_track_begin(), cur_body_desc));
    cur_sector->m_bytes_allocated = bytes_used;
    pair_agent_track.m_track_sectors.set_array_size(cur_sector_idx + 1);
    pair_agent_track.m_track_sectors[cur_sector_idx] = cur_sector;
    if (spare_sector) {
        delete spare_sector;
    }
}


// @address 0x691740 phys_batch_agent_destroy
extern "C" void phys_batch_agent_destroy(
    phys_batch_agent_track &pair_agent_track, phys_dispatcher *dispatch, phys_contact_manager *pair_contact_manager,
        phys_constraint_owner &constraint_owner_ref)
{
    phys_batch_agent_sector *read_sector = pair_agent_track.m_track_sectors[0];
    void *sector_read_data = read_sector->get_track_begin();
    void *sector_read_end = read_sector->get_track_end();
    int next_read_sector = 1;
    while (1)
    {
        phys_batch_agent_entry *entry = (phys_batch_agent_entry *)sector_read_data;
        int command = entry->m_stream_command;
        switch (command)
        {
        case 2:
        case 3:
        case 6:
        {
            void *agent_data = phys_add_byte_offset(sector_read_data, 16);
            sector_read_data = phys_add_byte_offset(sector_read_data, entry->m_size);
            dispatch->get_narrow_agent_destroy_fn(entry->m_agent_type)(entry, agent_data, pair_contact_manager, constraint_owner_ref, dispatch);
            break;
        }
        case 4:
        case 5:
        {
            void *agent_data = phys_add_byte_offset(sector_read_data, 32);
            sector_read_data = phys_add_byte_offset(sector_read_data, entry->m_size);
            dispatch->get_narrow_agent_destroy_fn(entry->m_agent_type)(entry, agent_data, pair_contact_manager, constraint_owner_ref, dispatch);
            break;
        }
        case 0:
            sector_read_data = phys_add_byte_offset(sector_read_data, entry->m_size);
            break;
        case 1:
            delete read_sector;
            if ((pair_agent_track.m_track_sectors.m_capacity_and_flags & PHYS_ARRAY_DONT_DEALLOCATE_FLAG) == 0) {
                phys_batch_agent_thread_memory().free_chunk(
                    pair_agent_track.m_track_sectors.m_data,
                        pair_agent_track.m_track_sectors.get_sector_capacity() * sizeof(phys_batch_agent_sector *), PHYS_MEM_ARRAY);
            }
            pair_agent_track.m_track_sectors.m_data = 0;
            pair_agent_track.m_track_sectors.m_size = 0;
            pair_agent_track.m_track_sectors.m_capacity_and_flags =
                PHYS_ARRAY_DONT_DEALLOCATE_FLAG | (pair_agent_track.m_track_sectors.m_capacity_and_flags & PHYS_ARRAY_LOCKED_FLAG);
            return;
        default:
            break;
        }
        if (sector_read_data >= sector_read_end)
        {
            delete read_sector;
            read_sector = pair_agent_track.m_track_sectors[next_read_sector];
            next_read_sector++;
            sector_read_data = read_sector->get_track_begin();
            sector_read_end = read_sector->get_track_end();
        }
    }
}










static __forceinline float phys_batch_agent_dot3(const __vector4 &a, const __vector4 &b)
{
    float d;
    __stvewx(__vmsum3fp(a, b), &d, 0);
    return d;
}

// @address 0x690b00 phys_batch_agent_weld
extern "C" void phys_batch_agent_weld(phys_narrow_agent_input &input, const phys_shape_container *compound_shape, phys_process_collision_output &output)
{
    __declspec(align(16)) char shape_scratch_buffer[512];
    int conflict_contact_cnt = 0;
    unsigned short new_contact_ids[256];
    unsigned short conflict_contacts[256];

    phys_process_collision_output::potential_info &potential_contacts = *output.m_potential_contacts;

    {
        const phys_convex_shape *penetration_shape_a = reinterpret_cast<const phys_convex_shape *>(input.m_body_a->m_proxy_shape);
        phys_process_collision_output::contact_ref *contact_ref_ptr = &potential_contacts.m_potential_contacts[0];
        for (int i = 0; contact_ref_ptr < potential_contacts.m_next_free_potential; i++, contact_ref_ptr++)
        {
            new_contact_ids[i] = 0xffff;

            phys_process_hit_point *check = contact_ref_ptr->m_contact_point;

            {
                phys_bool has_conflict = false;
                for (phys_process_hit_point **pref = &potential_contacts.m_representative_contacts[0];
                    pref < potential_contacts.m_next_free_representative; pref++)
                {
                    phys_process_hit_point *ref = *pref;
                    if (check != ref)
                    {
                        float radius_b = 0.0f;
                        phys_contact_pair_cmp pair_check_result = contact_point_is_valid(
                            penetration_shape_a->get_convex_radius(), check->m_hit_contact, *input.m_input->m_quality_info, radius_b, *ref);
                        if (pair_check_result != PHYS_CONTACT_PAIR_OK)
                        {
                            if (pair_check_result == PHYS_CONTACT_PAIR_REJECT)
                            {
                                goto try_next_point;
                            }
                            has_conflict = true;
                        }
                    }
                }
                if (has_conflict)
                {
                    conflict_contacts[conflict_contact_cnt++] = (unsigned short)i;
                }
            }

            {
                phys_process_collision_output::contact_ref &commit_ref = potential_contacts.m_potential_contacts[i];
                phys_batch_agent_entry *entry = static_cast<phys_batch_agent_entry *>(commit_ref.m_agent_entry);

                const phys_shape *shape = compound_shape->get_child_shape(entry->m_shape_key, shape_scratch_buffer);
                phys_collision_proxy modified_proxy_b(input.m_body_b);
                modified_proxy_b.set_proxy_shape(shape, entry->m_shape_key);

                new_contact_ids[i] = input.m_contact_manager->add_manifold_contact_point(*input.m_body_a, modified_proxy_b,
                    *input.m_input, output, 0, commit_ref.m_contact_point->m_hit_contact);
                if (new_contact_ids[i] != 0xffff)
                {
                    input.m_contact_manager->reserve_contact_points(-1);
                }
            }
try_next_point:
            continue;
        }
    }

    {
        phys_dispatcher *world_dispatcher = input.m_input->m_agent_dispatcher;
        for (int a = 0; a < conflict_contact_cnt; a++)
        {
            int i = conflict_contacts[a];
            phys_process_collision_output::contact_ref &cr = potential_contacts.m_potential_contacts[i];
            world_dispatcher->get_narrow_agent_create_zombie_fn(cr.m_agent_entry->m_agent_type)(cr.m_agent_entry, cr.m_agent_data, 0xffff);
        }
    }

    while (conflict_contact_cnt >= 2)
    {
        int p0 = 0;
        int p1 = 1;
        {
            float min_dot = 2.0f;
            for (int a = 0; a < conflict_contact_cnt; a++)
            {
                phys_process_collision_output::contact_ref &contact_ref_a = potential_contacts.m_potential_contacts[conflict_contacts[a]];
                const __vector4 &box_normal_a = contact_ref_a.m_contact_point->m_hit_contact.m_separating_normal.m_simd_quad;
                for (int b = a + 1; b < conflict_contact_cnt; b++)
                {
                    phys_process_collision_output::contact_ref &contact_ref_b = potential_contacts.m_potential_contacts[conflict_contacts[b]];
                    const float d = phys_batch_agent_dot3(box_normal_a, contact_ref_b.m_contact_point->m_hit_contact.m_separating_normal.m_simd_quad);
                    if (d < min_dot)
                    {
                        min_dot = d;
                        p0 = a;
                        p1 = b;
                    }
                }
            }
        }

        {
            phys_process_collision_output::contact_ref &contact_ref0 = potential_contacts.m_potential_contacts[conflict_contacts[p0]];
            phys_process_collision_output::contact_ref &contact_ref1 = potential_contacts.m_potential_contacts[conflict_contacts[p1]];

            phys_batch_agent_entry *entry_header0 = static_cast<phys_batch_agent_entry *>(contact_ref0.m_agent_entry);
            phys_batch_agent_entry *header1 = static_cast<phys_batch_agent_entry *>(contact_ref1.m_agent_entry);

            const phys_convex_shape *penetration_shape_a = reinterpret_cast<const phys_convex_shape *>(input.m_body_a->m_proxy_shape);
            if (!input.m_input->m_agent_dispatcher->has_alternate_type(penetration_shape_a->get_type_id(), PHYS_SHAPE_CONVEX)) { break; }
            const phys_convex_shape *convex_shape0 = reinterpret_cast<const phys_convex_shape *>(
                compound_shape->get_child_shape(entry_header0->m_shape_key, shape_scratch_buffer));
            if (!input.m_input->m_agent_dispatcher->has_alternate_type(convex_shape0->get_type_id(), PHYS_SHAPE_CONVEX)) { break; }
            __declspec(align(16)) char shape_buf2[512];
            const phys_convex_shape *convex_shape1 = reinterpret_cast<const phys_convex_shape *>(
                compound_shape->get_child_shape(header1->m_shape_key, shape_buf2));
            if (!input.m_input->m_agent_dispatcher->has_alternate_type(convex_shape1->get_type_id(), PHYS_SHAPE_CONVEX)) { break; }

            const phys_convex_shape *shapes_b[2] = { convex_shape0, convex_shape1 };
            phys_contact_point *points[2] = { &contact_ref0.m_contact_point->m_hit_contact, &contact_ref1.m_contact_point->m_hit_contact };

            phys_compute_multi_penetration_depth(*(const phys_transform *)input.m_body_a->m_motion, penetration_shape_a, &shapes_b[0],
                2, input.m_a_to_b, &points[0]);
        }

        conflict_contacts[p1] = conflict_contacts[--conflict_contact_cnt];
        conflict_contacts[p0] = conflict_contacts[--conflict_contact_cnt];
    }

    if (conflict_contact_cnt)
    {
        if (new_contact_ids[conflict_contacts[0]] != 0xffff)
        {
            input.m_contact_manager->contact_remove_point(new_contact_ids[conflict_contacts[0]], *output.m_constraint_owner);
            new_contact_ids[conflict_contacts[0]] = 0xffff;
            input.m_contact_manager->reserve_contact_points(1);
        }
    }

    {
        phys_dispatcher *world_dispatcher = input.m_input->m_agent_dispatcher;

        phys_process_collision_output::contact_ref *cr = potential_contacts.m_next_free_potential;
        int potential_cnt = int(cr - &potential_contacts.m_potential_contacts[0]);

        for (int i = potential_cnt - 1; i >= 0; i--)
        {
            cr--;
            unsigned short alloc_contact_id = new_contact_ids[i];

            if (alloc_contact_id != 0xffff)
            {
                world_dispatcher->get_narrow_agent_commit_potential_fn(cr->m_agent_entry->m_agent_type)(cr->m_agent_entry, cr->m_agent_data, alloc_contact_id);
                cr->m_contact_point->m_process_contact_id = alloc_contact_id;
            }
            else
            {
                input.m_contact_manager->reserve_contact_points(-1);
                world_dispatcher->get_narrow_agent_remove_point_fn(cr->m_agent_entry->m_agent_type)(cr->m_agent_entry, cr->m_agent_data, alloc_contact_id);
                output.m_next_free_contact = output.m_next_free_contact - 1;
                *cr->m_contact_point = *output.m_next_free_contact;
            }
        }
    }
}

