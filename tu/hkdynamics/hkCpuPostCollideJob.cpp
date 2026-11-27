// hkdynamics/hkCpuPostCollideJob.obj: from the records 0x65f868
// @compiler xdk6130
// @flags /TP /O2
// @address 0x65f868 ?phys_cpu_post_collide_job@@YA?AW4job_status@phys_job_queue@@AAUphys_mt_thread_state@@AAV2@AAUjob_queue_entry@2@@Z
#include "phys/phys_cpu_post_collide_job.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_physics_command_machine.h"
#include "phys/phys_island.h"
#include "phys/phys_job_queue.h"
#include "phys/phys_job_queue_util.h"
#include <vectorintrinsics.h>
#include "phys/phys_agent_sector_header.h"
#include "phys/phys_dynamics_jobs.h"
phys_job_queue::job_status phys_cpu_post_collide_job(phys_mt_thread_state &thread, phys_job_queue &queue, phys_job_queue::job_queue_entry &entry)
{
    phys_post_collide_job &src_job = reinterpret_cast<phys_post_collide_job &>(entry);
    phys_island *island = src_job.m_sim_island;
    thread.m_world->lock_island_for_constraints(island);
    int job_count = src_job.m_header->m_job_count;
    for (int i = 0; i < job_count; ++i) {
        phys_agent_sector_header::sector_job_info *info = src_job.m_header->get_job_info(i);
        island->m_constraint_info.merge(info->m_constraint_info);
        if (info->m_command_queue.m_size != 0) {
            phys_command_machine_process(thread.m_world, info->m_command_queue.m_start, info->m_command_queue.m_size);
        }
    }
    src_job.m_header->deallocate();
    thread.m_world->unlock_island_constraints(island);
    return queue.finish_job_get_next(thread.m_thread_type, &entry, entry, &thread, phys_job_queue::JOB_WAIT_FOR_NEXT);
}

