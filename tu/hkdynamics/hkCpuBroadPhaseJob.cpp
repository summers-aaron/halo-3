// hkdynamics/hkCpuBroadPhaseJob.obj: from the records 0x65ff10 0x65ffe0
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x65ff10 ??_Gphys_build_jacobian_task_header@@QAAPAXI@Z
#include <ppcintrinsics.h>
#include "physics/havok_memory.h"
#include "phys/phys_thread_memory.h"
#include "phys/phys_memory.h"
#include "phys/phys_step_info.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_dynamics_jobs.h"
#include "phys/phys_agent_sector_header.h"
#include "phys/phys_single_thread_island_jobs.h"







#include "phys/phys_island.h"

#include "phys/phys_multithreaded_simulation.h"
#include "phys/phys_job_queue_util.h"

#include "phys/phys_world_op_util.h"
#include "phys/phys_constraint_solver_setup.h"
#include "phys/phys_continuous_simulation.h"
#include "phys/phys_job_queue.h"








static __forceinline void init_agent_sector_job(phys_agent_sector_job &src_job, const phys_step_info &step_info, void *sectors,
    int sector_count, int bytes_in_last_sector)
{
    src_job.m_task_idx = 0;
    src_job.m_sector_idx = 0;
    src_job.m_job_type = SIM_JOB_AGENT_SECTOR;
    src_job.m_sector_count = sector_count;
    src_job.m_track_sectors = static_cast<phys_pair_agent_sector **>(sectors);
    src_job.m_step_info = step_info;
    src_job.m_header = 0;
    src_job.m_last_sector_bytes = bytes_in_last_sector;
}

// @address 0x65ffe0 ?run_broadphase_job@phys_single_thread_island_jobs@@SA?AW4job_status@phys_job_queue@@AAUphys_mt_thread_state@@AAV3@AAUjob_queue_entry@3@@Z
phys_job_queue::job_status phys_single_thread_island_jobs::run_broadphase_job(phys_mt_thread_state &tl,
    phys_job_queue &shared_job_queue, phys_job_queue::job_queue_entry &agent_next_job_out)
{
    phys_broadphase_job &src_job = reinterpret_cast<phys_broadphase_job &>(agent_next_job_out);
    phys_island *island = src_job.m_sim_island;
    if (src_job.m_task_header->m_island_should_sleep != 0)
    {
        if (tl.m_world->m_want_sleep && island->m_active && !island->m_sparse_enabled)
        {
            phys_world_op_util::mark_island_inactive_mt(tl.m_world, island);
        }
    }
    tl.m_world_simulation->collide_entities_broadphase_ccd(
        island->m_island_entities.begin(), island->m_island_entities.getSize(), tl.m_world, src_job.m_task_header ? &src_job.m_task_header->m_export_done : 0);
    phys_build_jacobian_task_header *task = src_job.m_task_header;
    if (_InterlockedExchangeAdd(reinterpret_cast<long volatile *>(&task->m_object_ref_count), -1) == 1)
    {
        while (task->m_build_tasks.m_build_jacobian_tasks != 0)
        {
            phys_build_jacobian_task *chunk = task->m_build_tasks.m_build_jacobian_tasks;
            task->m_build_tasks.m_build_jacobian_tasks = chunk->m_next;
            phys_thread_memory::get_singleton_instance().free_chunk_fixed_size(chunk, 0x1000, PHYS_MEM_CONSTRAINT);
        }
        while (task->m_solve_tasks.m_first_solve_task != 0)
        {
            phys_solve_constraint_batch_task *chunk = task->m_solve_tasks.m_first_solve_task;
            task->m_solve_tasks.m_first_solve_task = chunk->m_next;
            phys_thread_memory::get_singleton_instance().free_chunk_fixed_size(chunk, 0x30, PHYS_MEM_CONSTRAINT);
        }
        if (task->m_buffer != 0)
        {
            phys_memory::get_singleton_instance().free_runtime_block(task->m_buffer, task->m_task_buf_size, PHYS_MEM_CONSTRAINT);
            task->m_buffer = 0;
        }
        delete task;
    }
    int sector_count = island->m_agent_track.m_track_sectors.getSize();
    if (sector_count > 0)
    {
        init_agent_sector_job(reinterpret_cast<phys_agent_sector_job &>(agent_next_job_out),
            tl.m_world->m_dynamics_step_info.m_step_info, island->m_agent_track.m_track_sectors.begin(),
            sector_count, island->m_agent_track.m_last_sector_bytes);
        if (sector_count > 16)
        {
            int count = (sector_count - 1) / 16 + 1;
            reinterpret_cast<phys_agent_sector_job &>(agent_next_job_out).m_header = phys_agent_sector_header::allocate(count);
            reinterpret_cast<phys_agent_sector_job &>(agent_next_job_out).m_task_count = count;
        }
        return shared_job_queue.add_job_get_next((phys_job_queue::job_kind)0, phys_job_queue::JOB_PRIORITY_LOW, tl.m_thread_type,
            agent_next_job_out, phys_job_queue::JOB_WAIT_FOR_NEXT);
    }
    return shared_job_queue.finish_job_get_next(tl.m_thread_type, &agent_next_job_out, agent_next_job_out, &tl, phys_job_queue::JOB_WAIT_FOR_NEXT);
}
