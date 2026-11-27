// hkinternal/hkAgent1nMachineProcess.obj: from the records 0x691c90 0x691ce8 0x691da8
// @compiler xdk5632
// @flags /TP /O2 /Oc
// @address 0x691c90 ?phys_batch_agent_flip_input@@YAXAAUphys_narrow_agent_process_input@@0@Z
// @address 0x691ce8 ?phys_batch_agent_init_input_at_time@@YAXAAUphys_narrow_agent_input@@AAUphys_batch_agent_body_temp@@0@Z
#include <vectorintrinsics.h>
#include <ppcintrinsics.h>
#include "physics/havok_memory.h"
#include "phys/phys_batch_agent_machine.h"

#include "phys/phys_base_types.h"
#include "phys/phys_math.h"
#include "phys/phys_thread_memory.h"
#include "phys/phys_step_info.h"
#include "phys/phys_base.h"
#include "phys/phys_agent.h"
#include "phys/phys_agent_entry.h"
#include "phys/phys_batch_agent_track.h"
#include "phys/phys_math_prefetch.h"
#include "phys/phys_collision_proxy.h"
#include "phys/phys_shape_type_enum.h"



#include "phys/phys_collide.h"
#include "phys/phys_swept_transform_util.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_constraint_owner.h"
struct phys_batch_agent_body_temp
{
    phys_collision_proxy m_body_a;
    phys_collision_proxy m_body_b;
    phys_transform m_transform_a;
    phys_transform m_transform_b;
};

void phys_batch_agent_flip_input(phys_narrow_agent_process_input &input, phys_narrow_agent_process_input &flipped)
{
    flipped.m_body_a = input.m_body_b;
    flipped.m_body_b = input.m_body_a;
    flipped.m_input = input.m_input;
    flipped.m_sector_offset_delta = input.m_sector_offset_delta;
    flipped.m_contact_manager = input.m_contact_manager;
    flipped.m_lin_sweep_info.set_neg3(input.m_lin_sweep_info);
    flipped.m_a_to_b.set_to_inverse(input.m_a_to_b);
}


void phys_batch_agent_init_input_at_time(phys_narrow_agent_input &in, phys_batch_agent_body_temp &temp, phys_narrow_agent_input &out)
{
    phys_swept_transform_util::lerp_swept(in.m_body_a->get_motion_state()->get_swept_transform(),
        in.m_input->m_step_info.m_step_start_time, temp.m_transform_a);
    phys_swept_transform_util::lerp_swept(in.m_body_b->get_motion_state()->get_swept_transform(),
        in.m_input->m_step_info.m_step_start_time, temp.m_transform_b);

    out.m_body_a = &temp.m_body_a;
    out.m_body_b = &temp.m_body_b;
    out.m_contact_manager = in.m_contact_manager;
    out.m_input = in.m_input;

    temp.m_body_a.set_proxy_shape(in.m_body_a->get_shape(), in.m_body_a->get_shape_key());

    new (&temp.m_body_a) phys_collision_proxy(in.m_body_a, &temp.m_transform_a);
    new (&temp.m_body_b) phys_collision_proxy(in.m_body_b, &temp.m_transform_b);
    out.m_a_to_b.set_mul_inverse_mul(temp.m_transform_a, temp.m_transform_b);
}

static __forceinline void phys_batch_agent_process_flip_input(phys_narrow_agent_process_input &input, phys_narrow_agent_process_input &flipped)
{
    flipped.m_body_a = input.m_body_b;
    flipped.m_body_b = input.m_body_a;
    flipped.m_input = input.m_input;
    flipped.m_sector_offset_delta = input.m_sector_offset_delta;
    flipped.m_contact_manager = input.m_contact_manager;
    flipped.m_lin_sweep_info.set_neg3(input.m_lin_sweep_info);
    flipped.m_a_to_b.set_to_inverse(input.m_a_to_b);
}

namespace phys_math
{
        }



// @address 0x691da8 phys_batch_agent_process
extern "C" void phys_batch_agent_process(phys_batch_agent_track &pair_agent_track, phys_batch_agent_track *agent_track_on_ppu,
    phys_narrow_agent_process_input &input,
    const phys_shape_container *shape_container_b, const unsigned int *hit_key_list, int bvh_hit_count, phys_process_collision_output &output)
{
    phys_math::prefetch_128(pair_agent_track.m_track_sectors.begin());

    phys_process_collision_output::potential_info *old_potential_info = output.m_potential_contacts;
    phys_process_collision_output::potential_info potential_contacts;
    if (!old_potential_info && shape_container_b->edge_weld_enabled())
    {
        potential_contacts.reset();
        output.m_potential_contacts = &potential_contacts;
    }
    else
    {
        output.m_potential_contacts = 0;
    }

    phys_dispatcher *dispatcher = input.m_input->m_agent_dispatcher;

    phys_collision_proxy child_proxy_b(input.m_body_b);
    input.m_body_b = &child_proxy_b;

    int sweep_entry_count = 0;

    phys_batch_agent_sector *read_sector = pair_agent_track.m_track_sectors[0];
    phys_math::force_prefetch<512>(read_sector);
    phys_batch_agent_sector *cur_sector = new phys_batch_agent_sector;
    int cur_sector_idx = 0;

    bool prev_input_ready = false;
    bool prev_flipped_ready = false;

    phys_narrow_agent_input prev_agent_input;
    phys_narrow_agent_input prev_flipped_input;

    phys_batch_agent_body_temp prev_body_temp;
    phys_batch_agent_body_temp prev_flipped_temp;

    int next_read_sector = 1;
    void *sector_read_data = read_sector->get_track_begin();
    void *sector_read_end = read_sector->get_track_end();
    void *cur_body_desc = cur_sector->get_track_begin();

    phys_shape_buffer shape_scratch_buffer;

    while (1)
    {
        unsigned int kdop_shape_key;
        if (!hit_key_list)
        {
            phys_batch_agent_entry *entry = reinterpret_cast<phys_batch_agent_entry *>(sector_read_data);
            phys_string::mem_copy16(cur_body_desc, sector_read_data, entry->m_size >> 4);
            sector_read_data = phys_add_byte_offset(sector_read_data, entry->m_size);
            kdop_shape_key = entry->m_shape_key;
        }
        else
        {
            phys_batch_agent_entry *read_entry = reinterpret_cast<phys_batch_agent_entry *>(sector_read_data);
            kdop_shape_key = *hit_key_list;
            if (read_entry->m_shape_key == kdop_shape_key)
            {
                phys_string::mem_copy16(cur_body_desc, sector_read_data, read_entry->m_size >> 4);
                sector_read_data = phys_add_byte_offset(sector_read_data, read_entry->m_size);
                hit_key_list++;
            }
            else
            {
                if (read_entry->m_shape_key < kdop_shape_key)
                {
                    void *agent_data = ((read_entry->m_stream_command & 0xe) == 4)
                        ? (void *)phys_add_byte_offset(read_entry, 32)
                        : (void *)phys_add_byte_offset(read_entry, 16);
                    sector_read_data = phys_add_byte_offset(sector_read_data, read_entry->m_size);
                    dispatcher->get_narrow_agent_destroy_fn(read_entry->m_agent_type)(
                        read_entry, agent_data, input.m_contact_manager, *output.m_constraint_owner.val(), dispatcher);
                    goto check_read_sector;
                }
                else
                {
                    if (input.m_input->m_compound_filter->allows_collision(*input.m_input, *input.m_body_a,
                        *input.m_body_b->get_parent_proxy(), *shape_container_b, kdop_shape_key))
                    {
                        const phys_shape *penetration_shape_b = shape_container_b->get_child_shape(kdop_shape_key, shape_scratch_buffer);
                        child_proxy_b.set_proxy_shape(penetration_shape_b, kdop_shape_key);

                        phys_batch_agent_entry *h = reinterpret_cast<phys_batch_agent_entry *>(cur_body_desc);
                        h->m_shape_key = kdop_shape_key;
                        h->m_agent_type = (unsigned char)dispatcher->get_narrow_agent_type(
                            input.m_body_a->get_shape()->get_type_id(), penetration_shape_b->get_type_id(), input.m_input->m_create_pred_agents);
                        h->m_contact_point_count = 0;

                        phys_narrow_agent_process_input flipped_input;
                        phys_narrow_agent_process_input *in = &input;
                        int is_flipped = 0;
                        if (dispatcher->get_narrow_agent_symmetric(h->m_agent_type) == 2)
                        {
                            is_flipped = 1;
                            in = &flipped_input;
                            phys_batch_agent_flip_input(input, flipped_input);
                        }

                        if (dispatcher->get_narrow_agent_sep_normal_fn(h->m_agent_type) != 0)
                        {
                            phys_batch_agent_sweep_entry *entry = reinterpret_cast<phys_batch_agent_sweep_entry *>(cur_body_desc);
                            entry->m_stream_command = (unsigned char)(4 + is_flipped);
                            entry->m_separating_normal_time = -1.0f;
                            entry->m_separating_normal.set_zero4();
                            void *agent_data = reinterpret_cast<void *>(entry + 1);
                            void *agent_end = dispatcher->get_narrow_agent_create_fn(entry->m_agent_type)(*in, entry, agent_data);
                            unsigned long size = phys_get_byte_offset(entry, agent_end);
                            entry->m_size = (unsigned char)size;
                        }
                        else
                        {
                            phys_batch_agent_padded_entry *entry = reinterpret_cast<phys_batch_agent_padded_entry *>(cur_body_desc);
                            entry->m_stream_command = (unsigned char)(2 + is_flipped);
                            void *agent_data = reinterpret_cast<void *>(entry + 1);
                            void *agent_end = dispatcher->get_narrow_agent_create_fn(entry->m_agent_type)(*in, entry, agent_data);
                            unsigned long size = phys_get_byte_offset(entry, agent_end);
                            entry->m_size = (unsigned char)size;
                        }
                    }
                    else
                    {
                        phys_batch_agent_padded_entry *h = reinterpret_cast<phys_batch_agent_padded_entry *>(cur_body_desc);
                        h->m_shape_key = kdop_shape_key;
                        h->m_agent_type = 0;
                        h->m_contact_point_count = 0;
                        h->m_stream_command = 0;
                        h->m_size = 16;
                        unsigned int *marker = reinterpret_cast<unsigned int *>(static_cast<phys_batch_agent_entry *>(h) + 1);
                        marker[0] = 0xabcdabcd;
                        marker[1] = 0xabcdabcd;
                    }
                    hit_key_list++;
                }
            }
        }

        int command = reinterpret_cast<phys_agent_entry *>(cur_body_desc)->m_stream_command;
        switch (command)
        {
        case 6:
            {
                phys_batch_agent_padded_entry *entry = reinterpret_cast<phys_batch_agent_padded_entry *>(cur_body_desc);
                const phys_shape *shape = shape_container_b->get_child_shape(kdop_shape_key, shape_scratch_buffer);
                child_proxy_b.set_proxy_shape(shape, kdop_shape_key);
                phys_agent *pair_agent_entry = entry->m_stream_agent;
                pair_agent_entry->process_collision(*input.m_body_a, child_proxy_b, *input.m_input, output);
                cur_body_desc = reinterpret_cast<void *>(entry + 1);
                break;
            }
        case 2:
            {
                phys_batch_agent_padded_entry *entry = reinterpret_cast<phys_batch_agent_padded_entry *>(cur_body_desc);
                const phys_shape *shape = shape_container_b->get_child_shape(kdop_shape_key, shape_scratch_buffer);
                child_proxy_b.set_proxy_shape(shape, kdop_shape_key);
                void *agent_data = reinterpret_cast<void *>(entry + 1);
                cur_body_desc = dispatcher->get_narrow_agent_process_fn(entry->m_agent_type)(input, entry, agent_data, 0, output);
                entry->m_size = (unsigned char)phys_get_byte_offset(entry, cur_body_desc);
                break;
            }
        case 3:
            {
                phys_narrow_agent_process_input flipped_input;
                phys_batch_agent_process_flip_input(input, flipped_input);
                phys_batch_agent_padded_entry *entry = reinterpret_cast<phys_batch_agent_padded_entry *>(cur_body_desc);
                const phys_shape *shape = shape_container_b->get_child_shape(kdop_shape_key, shape_scratch_buffer);
                child_proxy_b.set_proxy_shape(shape, kdop_shape_key);
                float old_toi = output.m_collision_toi.m_time;
                void *agent_data = reinterpret_cast<void *>(entry + 1);
                phys_process_hit_point *old_result_begin = output.m_next_free_contact;
                cur_body_desc = dispatcher->get_narrow_agent_process_fn(entry->m_agent_type)(flipped_input, entry, agent_data, 0, output);
                entry->m_size = (unsigned char)phys_get_byte_offset(entry, cur_body_desc);
                while (old_result_begin < output.m_next_free_contact)
                {
                    old_result_begin->m_hit_contact.flip();
                    old_result_begin++;
                }
                if (old_toi != output.m_collision_toi.m_time)
                {
                    output.m_collision_toi.flip();
                }
                break;
            }
        case 5:
            {
                phys_narrow_agent_process_input flipped_input;
                phys_batch_agent_process_flip_input(input, flipped_input);
                phys_batch_agent_sweep_entry *entry = reinterpret_cast<phys_batch_agent_sweep_entry *>(cur_body_desc);
                float dist_at_t1;
                if (!(entry->m_separating_normal_time == flipped_input.m_input->m_step_info.m_step_start_time))
                {
                    const phys_collision_quality_info &ci = *flipped_input.m_input->m_quality_info;
                    if (!ci.m_use_ccd)
                    {
                        entry->m_separating_normal_time = flipped_input.m_input->m_step_info.m_end_time;
                        dist_at_t1 = 0.5f * ci.m_min_separation;
                        entry->m_separating_normal.set_zero4();
                        entry->m_separating_normal(3) = dist_at_t1;
                        goto process_at_t1_flipped;
                    }
                    if (!prev_flipped_ready)
                    {
                        prev_flipped_ready = true;
                        phys_batch_agent_init_input_at_time(flipped_input, prev_flipped_temp, prev_flipped_input);
                    }
                    const phys_shape *shape = shape_container_b->get_child_shape(kdop_shape_key, shape_scratch_buffer);
                    prev_flipped_temp.m_body_a.set_proxy_shape(shape, kdop_shape_key);
                    void *agent_data = reinterpret_cast<void *>(entry + 1);
                    dispatcher->get_narrow_agent_sep_normal_fn(entry->m_agent_type)(prev_flipped_input, agent_data, entry->m_separating_normal);
                }
                entry->m_separating_normal_time = flipped_input.m_input->m_step_info.m_end_time;
                {
                    const float worst_delta_dist = flipped_input.m_lin_sweep_info.dot4_xyz1(entry->m_separating_normal);
                    dist_at_t1 = entry->m_separating_normal(3) - worst_delta_dist;
                }
                if (dist_at_t1 >= flipped_input.m_input->get_tolerance())
                {
                    entry->m_separating_normal(3) = dist_at_t1;
                    if (!entry->m_contact_point_count)
                    {
                        cur_body_desc = phys_add_byte_offset(cur_body_desc, entry->m_size);
                    }
                    else
                    {
                        void *agent_data = reinterpret_cast<void *>(entry + 1);
                        cur_body_desc = dispatcher->get_narrow_agent_cleanup_fn(entry->m_agent_type)(
                            entry, agent_data, flipped_input.m_contact_manager, *output.m_constraint_owner.val());
                        entry->m_size = (unsigned char)phys_get_byte_offset(entry, cur_body_desc);
                    }
                    sweep_entry_count++;
                    break;
                }
process_at_t1_flipped:
                flipped_input.m_dist_at_t1 = dist_at_t1;
                {
                    const phys_shape *shape = shape_container_b->get_child_shape(kdop_shape_key, shape_scratch_buffer);
                    child_proxy_b.set_proxy_shape(shape, kdop_shape_key);
                    phys_process_hit_point *old_result_begin = output.m_next_free_contact;
                    float old_toi = output.m_collision_toi.m_time;
                    void *agent_data = reinterpret_cast<void *>(entry + 1);
                    cur_body_desc = dispatcher->get_narrow_agent_process_fn(entry->m_agent_type)(flipped_input, entry, agent_data,
                        &entry->m_separating_normal, output);
                    entry->m_size = (unsigned char)phys_get_byte_offset(entry, cur_body_desc);
                    while (old_result_begin < output.m_next_free_contact)
                    {
                        old_result_begin->m_hit_contact.flip();
                        old_result_begin++;
                    }
                    if (old_toi != output.m_collision_toi.m_time)
                    {
                        output.m_collision_toi.flip();
                    }
                }
                break;
            }
        case 4:
            {
                phys_batch_agent_sweep_entry *entry = reinterpret_cast<phys_batch_agent_sweep_entry *>(cur_body_desc);
                float dist_at_t1;
                if (!(entry->m_separating_normal_time == input.m_input->m_step_info.m_step_start_time))
                {
                    const phys_collision_quality_info &ci = *input.m_input->m_quality_info;
                    if (!ci.m_use_ccd)
                    {
                        entry->m_separating_normal_time = input.m_input->m_step_info.m_end_time;
                        dist_at_t1 = 0.1f * ci.m_min_separation;
                        entry->m_separating_normal.set_zero4();
                        entry->m_separating_normal(3) = dist_at_t1;
                        goto process_at_t1;
                    }
                    if (!prev_input_ready)
                    {
                        prev_input_ready = true;
                        phys_batch_agent_init_input_at_time(input, prev_body_temp, prev_agent_input);
                    }
                    const phys_shape *shape = shape_container_b->get_child_shape(kdop_shape_key, shape_scratch_buffer);
                    prev_body_temp.m_body_b.set_proxy_shape(shape, kdop_shape_key);
                    void *agent_data = reinterpret_cast<void *>(entry + 1);
                    dispatcher->get_narrow_agent_sep_normal_fn(entry->m_agent_type)(prev_agent_input, agent_data, entry->m_separating_normal);
                }
                entry->m_separating_normal_time = input.m_input->m_step_info.m_end_time;
                {
                    const float worst_delta_dist = input.m_lin_sweep_info.dot4_xyz1(entry->m_separating_normal);
                    dist_at_t1 = entry->m_separating_normal(3) - worst_delta_dist;
                }
                if (dist_at_t1 >= input.m_input->get_tolerance())
                {
                    entry->m_separating_normal(3) = dist_at_t1;
                    if (!entry->m_contact_point_count)
                    {
                        cur_body_desc = phys_add_byte_offset(cur_body_desc, entry->m_size);
                    }
                    else
                    {
                        void *agent_data = reinterpret_cast<void *>(entry + 1);
                        cur_body_desc = dispatcher->get_narrow_agent_cleanup_fn(entry->m_agent_type)(
                            entry, agent_data, input.m_contact_manager, *output.m_constraint_owner.val());
                        entry->m_size = (unsigned char)phys_get_byte_offset(entry, cur_body_desc);
                    }
                    sweep_entry_count++;
                    break;
                }
process_at_t1:
                input.m_dist_at_t1 = dist_at_t1;
                {
                    const phys_shape *shape = shape_container_b->get_child_shape(kdop_shape_key, shape_scratch_buffer);
                    child_proxy_b.set_proxy_shape(shape, kdop_shape_key);
                    void *agent_data = reinterpret_cast<void *>(entry + 1);
                    cur_body_desc = dispatcher->get_narrow_agent_process_fn(entry->m_agent_type)(input, entry, agent_data, &entry->m_separating_normal, output);
                    entry->m_size = (unsigned char)phys_get_byte_offset(entry, cur_body_desc);
                }
                break;
            }
        case 0:
            {
                cur_body_desc = phys_add_byte_offset(cur_body_desc, 16);
                break;
            }
        case 1:
            {
                cur_body_desc = phys_add_byte_offset(cur_body_desc, 16);
                break;
            }
        default:
            break;
        }

check_read_sector:
        if (sector_read_data == sector_read_end)
        {
            delete read_sector;
            if (next_read_sector < pair_agent_track.m_track_sectors.getSize())
            {
                read_sector = pair_agent_track.m_track_sectors[next_read_sector];
                sector_read_data = read_sector->get_track_begin();
                sector_read_end = read_sector->get_track_end();
                next_read_sector++;
            }
            else
            {
                break;
            }
        }

        {
            int next_entry_size = reinterpret_cast<phys_agent_entry *>(sector_read_data)->m_size;
            int bytes_used = int(phys_get_byte_offset(cur_sector->get_track_begin(), cur_body_desc));
            if (next_entry_size + bytes_used <= cur_sector->get_sector_capacity() - 80)
            {
                continue;
            }
            cur_sector->m_bytes_allocated = bytes_used;
            if (cur_sector_idx >= next_read_sector)
            {
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
            cur_sector = new phys_batch_agent_sector;
            cur_body_desc = cur_sector->get_track_begin();
        }
    }

    {
        int bytes_used = int(phys_get_byte_offset(cur_sector->get_track_begin(), cur_body_desc));
        cur_sector->m_bytes_allocated = bytes_used;
        pair_agent_track.m_track_sectors.set_array_size(cur_sector_idx + 1);
        pair_agent_track.m_track_sectors[cur_sector_idx] = cur_sector;
    }

    input.m_body_b = child_proxy_b.get_parent_proxy();
    if (output.m_potential_contacts)
    {
        if (output.m_potential_contacts->m_next_free_potential > &output.m_potential_contacts->m_potential_contacts[0])
        {
            phys_batch_agent_weld(input, shape_container_b, output);
        }
    }
    output.m_potential_contacts = old_potential_info;
}

