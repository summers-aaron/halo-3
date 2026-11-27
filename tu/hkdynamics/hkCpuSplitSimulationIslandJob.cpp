// hkdynamics/hkCpuSplitSimulationIslandJob.obj: from the records 0x6602b8 0x660328
// @flags /TP /O1 /J
// @address 0x660328 ?phys_cpu_split_island_job@@YA?AW4job_status@phys_job_queue@@AAUphys_mt_thread_state@@AAV2@AAUjob_queue_entry@2@@Z
#include "phys/phys_cpu_split_island_job.h"

#include "phys/phys_dynamics.h"
#include "phys/phys_island.h"
#include "phys/phys_world_op_util.h"
#include "phys/phys_job_queue_util.h"
#include "phys/phys_dynamics_jobs.h"

phys_job_queue::job_status phys_cpu_split_island_job(phys_mt_thread_state &thread,
    phys_job_queue &task_queue, phys_job_queue::job_queue_entry &src_job)
{
    phys_split_island_job &split_job = reinterpret_cast<phys_split_island_job &>(src_job);
    phys_island *island = split_job.m_sim_island;
    phys_array<phys_island *> *job_split_islands = &split_job.m_task_header->m_new_split_islands;
    island->m_split_check_requested = false;
    char entity_array_storage[sizeof(phys_array<phys_entity *>)];
    phys_array<phys_entity *> *entities = new (entity_array_storage) phys_array<phys_entity *>;
    island->mark_for_write();
    phys_world_op_util::split_island(island, island->m_world, *job_split_islands, entities);
    return task_queue.finish_job_get_next(thread.m_thread_type, &src_job, src_job, &thread, phys_job_queue::JOB_WAIT_FOR_NEXT);
}
// @address 0x6602b8 ?phys_cpu_split_island_job_impl@@YAXPAVphys_island@@AAV?$phys_array@PAVphys_island@@@@@Z
void phys_cpu_split_island_job_impl(phys_island *island, phys_array<phys_island *> &job_split_islands)
{
    island->m_split_check_requested = false;
    char entity_array_storage[sizeof(phys_array<phys_entity *>)];
    phys_array<phys_entity *> *entities = new (entity_array_storage) phys_array<phys_entity *>;
    island->mark_for_write();
    phys_world_op_util::split_island(island, island->m_world, job_split_islands, entities);
}
