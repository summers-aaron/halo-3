// hkdynamics/hkCpuAgentSectorJob.obj: from the records 0x65f968 0x65f980 0x65f998 0x65f9c8 0x65feb0
// @flags /TP /O2 /Oc
#include "phys/phys_base_types.h"
#include "phys/phys_cpu_agent_sector_job.h"
#include <ppcintrinsics.h>
#include <stdlib.h>
#include "physics/havok_memory.h"
#include "phys/phys_pair_agent_machine.h"
#include "phys/phys_physics_command_machine.h"
#include "phys/phys_thread_memory.h"
#include "phys/phys_collide.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_atoms.h"
#include "phys/phys_constraint.h"
#include "phys/phys_memory.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_island.h"
#include "phys/phys_math_prefetch.h"
#include "phys/phys_critical_section.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_collision_input.h"
#include "phys/phys_pair_agent_entry.h"
#include "phys/phys_pair_agent_sector.h"
#include "phys/phys_multithreaded_simulation.h"
#include "phys/phys_cpu_process_agent.h"
#include "phys/phys_agent_sector_header.h"
#include "phys/phys_dynamics_jobs.h"
#include "phys/phys_job_queue.h"
#include "phys/phys_job_queue_util.h"
// @address 0x65f968 ?add_constraint_to_locked_island@phys_deferred_constraint_owner@@UAAXPAVphys_constraint@@@Z
// @address 0x65f980 ?remove_constraint_locked_island@phys_deferred_constraint_owner@@UAAXPAVphys_constraint@@@Z
// @address 0x65f998 ?add_callback_request@phys_deferred_constraint_owner@@UAAXPAVphys_constraint@@H@Z
// @address 0x65feb0 ??_Gphys_deferred_constraint_owner@@UAAPAXI@Z

namespace phys_math {
}

typedef phys_dual_object_command<phys_physics_command::CMD_ADD_CONSTRAINT_TO_LOCKED_ISLAND, class phys_constraint *, int>
    phys_add_constraint_island_command;
typedef phys_single_object_command<phys_physics_command::CMD_REMOVE_CONSTRAINT_FROM_LOCKED_ISLAND, class phys_constraint *>
    phys_remove_constraint_locked_cmd;

#include "phys/phys_physics_command_queue.h"
#include "phys/phys_statistics_collector.h"

class phys_deferred_constraint_owner : public phys_constraint_owner {
public:
    __forceinline phys_deferred_constraint_owner()
    {
        m_constraint_info.clear();
        m_critical_locked_island_cnt = 0;
        m_callback_request = 0;
    }
    virtual void add_constraint_to_locked_island(phys_constraint *constraint);
    virtual void remove_constraint_locked_island(phys_constraint *constraint);
    virtual void add_callback_request(phys_constraint *constraint, int constraint_callback_request);
    virtual void check_access_rw() {}
    phys_padded<int> m_critical_locked_island_cnt;
    phys_padded<phys_constraint *> m_critical_locked_constraint;
    phys_padded<int> m_callback_request;
    phys_fixed_size_command_queue<1024> m_command_queue;
};

void phys_deferred_constraint_owner::add_constraint_to_locked_island(phys_constraint *constraint)
{
    m_critical_locked_constraint = constraint;
    m_critical_locked_island_cnt = m_critical_locked_island_cnt + 1;
}

void phys_deferred_constraint_owner::remove_constraint_locked_island(phys_constraint *constraint)
{
    m_critical_locked_constraint = constraint;
    m_critical_locked_island_cnt = m_critical_locked_island_cnt - 1;
}

void phys_deferred_constraint_owner::add_callback_request(phys_constraint *constraint, int constraint_callback_request)
{
    phys_constraint_internal *internal = constraint->m_constraint_internal;
    if (internal != 0) {
        phys_constraint_internal *callback_target = (phys_constraint_internal *)_rotl64((unsigned __int64)internal, 0);
        callback_target->m_callback_request |= constraint_callback_request;
        return;
    }
    m_callback_request = m_callback_request | constraint_callback_request;
}

// @address 0x65f9c8 ?phys_cpu_agent_sector_job@@YA?AW4job_status@phys_job_queue@@AAUphys_mt_thread_state@@AAV2@AAUjob_queue_entry@2@@Z
phys_job_queue::job_status phys_cpu_agent_sector_job(phys_mt_thread_state &tl, phys_job_queue &shared_job_queue,
    phys_job_queue::job_queue_entry &agent_next_job_out)
{
    phys_agent_sector_job &src_job = reinterpret_cast<phys_agent_sector_job &>(agent_next_job_out);

    tl.m_collision_input.m_step_info = src_job.m_step_info;

    phys_island *island = src_job.m_sim_island;
    {
        phys_deferred_constraint_owner constraint_owner_ref;
        __declspec(align(16)) phys_process_collision_output process_output(&constraint_owner_ref);

        phys_pair_agent_track &track = island->m_agent_track;
        int track_sector_index = src_job.m_sector_idx;

        int sector_count = src_job.m_sector_count - 1;
        phys_pair_agent_entry *entry = track.m_track_sectors[track_sector_index]->get_track_begin();
        int sector_size = track.get_sector_size(track_sector_index);
        phys_pair_agent_entry *end_entry = phys_add_byte_offset(entry, sector_size);

        phys_pair_agent_entry *next_entry = phys_add_byte_offset(entry, 128);
        phys_pair_agent_entry *second_next_entry = phys_add_byte_offset(next_entry, 128);
        if (next_entry >= end_entry) { next_entry = 0; second_next_entry = 0; }
        else if (second_next_entry >= end_entry) { second_next_entry = 0; }

        do {
            {
                phys_cpu_process_agent_helper(entry, tl.m_collision_input, process_output, tl.m_world_simulation);

                if (constraint_owner_ref.m_critical_locked_island_cnt == 0) {
                    constraint_owner_ref.m_callback_request = 0;
                } else if (constraint_owner_ref.m_critical_locked_island_cnt > 0) {
                    constraint_owner_ref.m_command_queue.add_command(phys_add_constraint_island_command(
                        constraint_owner_ref.m_critical_locked_constraint, constraint_owner_ref.m_callback_request));
                    constraint_owner_ref.m_callback_request = 0;
                } else {
                    constraint_owner_ref.m_command_queue.add_command(
                        phys_remove_constraint_locked_cmd(constraint_owner_ref.m_critical_locked_constraint));
                }
                constraint_owner_ref.m_critical_locked_island_cnt = 0;
            }

            entry = next_entry;
            next_entry = second_next_entry;

            if (second_next_entry) {
                second_next_entry = phys_add_byte_offset(second_next_entry, 128);
                if (second_next_entry >= end_entry) {
                    ++track_sector_index;
                    if (--sector_count >= 0) {
                        phys_pair_agent_sector *track_sector = track.m_track_sectors[track_sector_index];
                        second_next_entry = track_sector->get_track_begin();
                        if (sector_count > 0) {
                            phys_math::force_prefetch<512>(track.m_track_sectors[track_sector_index + 1]);
                        }
                        sector_size = track.get_sector_size(track_sector_index);
                        end_entry = phys_add_byte_offset(second_next_entry, sector_size);
                    } else {
                        second_next_entry = 0;
                        continue;
                    }
                }
                const int collider_offset = 16;
                const int motion_offset = 160;
                const int offset = motion_offset - collider_offset;
                phys_math::prefetch_128(second_next_entry->m_contact_manager);
                phys_math::prefetch_128(second_next_entry->m_collider[0]);
                phys_math::prefetch_128(phys_add_byte_offset_const((const phys_linked_collider *)second_next_entry->m_collider[0], offset));
                phys_math::prefetch_128(second_next_entry->m_collider[1]);
                phys_math::prefetch_128(phys_add_byte_offset_const((const phys_linked_collider *)second_next_entry->m_collider[1], offset));
            }
        } while (entry);

        if (!src_job.m_header) {
            tl.m_world->lock_island_for_constraints(island);
            island->m_constraint_info.merge(constraint_owner_ref.m_constraint_info);
            if (constraint_owner_ref.m_command_queue.m_size) {
                phys_command_machine_process(tl.m_world, constraint_owner_ref.m_command_queue.m_start, constraint_owner_ref.m_command_queue.m_size);
            }
            tl.m_world->unlock_island_constraints(island);
        } else {
            phys_agent_sector_header::sector_job_info *sector_task_info = src_job.m_header->get_job_info(src_job.m_task_idx);
            sector_task_info->m_constraint_info = constraint_owner_ref.m_constraint_info;

            int size = constraint_owner_ref.m_command_queue.m_size;
            if (size) {
                phys_string::mem_copy16(sector_task_info->m_command_queue.m_start, constraint_owner_ref.m_command_queue.m_start, size >> 4);
                sector_task_info->m_command_queue.m_size = size;
            }
        }
    }

    return shared_job_queue.finish_job_get_next(tl.m_thread_type, (const phys_job_queue::job_queue_entry *)&src_job, agent_next_job_out, &tl,
        phys_job_queue::JOB_WAIT_FOR_NEXT);
}
